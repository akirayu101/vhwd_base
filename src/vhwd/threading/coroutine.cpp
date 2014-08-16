#include "vhwd/threading/coroutine.h"

#ifdef VHWD_USE_COROUTINE


#include "thread_impl.h"

extern "C" void asm_swap_context(void* callee,void* caller);


VHWD_ENTER


class VHWD_DLLIMPEXP CoroutineContext : private NonCopyable
{
public:

	CoroutineContext(size_t stksize_);
	~CoroutineContext();

	//Coroutine entry point
	static void raw_proc_spawn(CoroutineContext* rc);

	void init();

	void push(void* dat)
	{
		*(--nsp)=dat;
	}

	void** nsp;

	LitePtrT<Coroutine> m_pRoutine;
	char* m_pStackPointer;
	size_t m_nStackSize;

};



CoroutineMain::CoroutineMain():Coroutine(0)
{
	m_nState.store(Coroutine::STATE_RUNNING);
	m_pThisRoutine.reset(this);
}


Coroutine& Coroutine::this_coroutine()
{
	return *Thread::this_data().cort_main.m_pThisRoutine;
}

CoroutineMain& Coroutine::main_coroutine()
{
	return Thread::this_data().cort_main;
}


void* page_alloc(size_t n);
void page_free(void* p,size_t);
void heap_protect(void* p,size_t n,bool f);


void CoroutineContext::init()
{

	if(!m_pStackPointer)
	{
		return;
	}

	void** nbp=(void**)(m_pStackPointer+m_nStackSize);

	nsp=nbp;
	push(0); //padding
	push(0);
	push((void*)CoroutineContext::raw_proc_spawn);// coroutine entry point
	push(NULL); // return address <-- [nsp]

#ifdef _X86_

	push(nbp-5);  // nbp <-- [nbp]
	push(0); // nbx
	push(0); // nsi;
	push(0); // ndi;

	push((void*)(-1));		// fs:[0]
	push(nbp);				// fs:[4], stack hi
	push(m_pStackPointer);  // fs:[8], stack lo

	nsp=nbp-4;

#else

	push(nbp-5);	// nbp
	push(0);	// nbx
	push(0);	// nsi;
	push(0);	// ndi;

	push(0);	// r12
	push(0);	// r13
	push(0);	// r14
	push(0);	// r15;

	push((void*)(NULL));	// fs:[0]
	push(nbp);				// fs:[4], stack hi
	push(m_pStackPointer);  // fs:[8], stack lo

	//xmm6 to xmm15¡£

	nsp=nbp-4;

#endif

}

CoroutineContext::CoroutineContext(size_t stksize_)
{
	if(stksize_>0)
	{
		int pagesize=System::GetPageSize();
		stksize_=sz_helper::adj(stksize_,pagesize)+pagesize;
		m_pStackPointer=(char*)page_alloc(stksize_);
		if(!m_pStackPointer)
		{
			Exception::XBadAlloc();
		}

		// protect first page to detect stack overflow
		heap_protect(m_pStackPointer,pagesize,true);

	}
	else
	{
		m_pStackPointer=NULL;
		nsp=0;
	}

	m_nStackSize=stksize_;

}

CoroutineContext::~CoroutineContext()
{
	if(m_pStackPointer)	page_free(m_pStackPointer,m_nStackSize);
}


Coroutine::~Coroutine()
{
	delete m_pContext;
}

int Coroutine::state() const
{
	return m_nState.get();
}

size_t Coroutine::stack_size() const
{
	return m_pContext->m_nStackSize;
}

bool Coroutine::spawn(Coroutine* pcortctx_)
{
	if(!pcortctx_) return false;
	Coroutine& callee(*pcortctx_);
	int32_t val=STATE_STOPPED;

	if(!callee.m_nState.compare_exchange(val,STATE_PENDING))
	{
		return false;
	}

	callee.m_pContext->init();
	callee.m_nState.store(STATE_PAUSED);

	return true;
}


Coroutine::Coroutine(size_t stksize_)
{
	m_pContext=NULL;
	m_pContext=new CoroutineContext(stksize_);
	m_pContext->m_pRoutine.reset(this);
	m_nState.store(STATE_STOPPED);
}

bool Coroutine::yield_main()
{
	return yield(&main_coroutine());
}

bool Coroutine::yield_last()
{
	return yield(this_coroutine().m_pLastRoutine);
}

bool Coroutine::yield(Coroutine* pcortctx_)
{
	if(!pcortctx_) return false;

	Coroutine& caller(this_coroutine());
	Coroutine& callee(*pcortctx_);

	if(&caller==&callee) return false;

	int32_t val;

	val=STATE_RUNNING;
	if(!caller.m_nState.compare_exchange(val,STATE_PENDING))
	{
		return false;
	}

	val=STATE_PAUSED;
	if(!callee.m_nState.compare_exchange(val,STATE_PENDING))
	{
		caller.m_nState.store(STATE_RUNNING);
		return false;
	}

	callee.m_pLastRoutine.reset(&caller);
	callee.m_pExtraParam.swap(caller.m_pExtraParam);

	asm_swap_context(callee.m_pContext,caller.m_pContext);


	main_coroutine().m_pThisRoutine.reset(&caller);
	caller.m_nState.store(STATE_RUNNING);

	// Coroutine::svc return and yield to main, in this case, m_pLastRoutine will be NULL
	if(caller.m_pLastRoutine)
	{
		caller.m_pLastRoutine->m_nState.store(STATE_PAUSED);
	}

	return true;
}


void CoroutineContext::raw_proc_spawn(CoroutineContext* pcortctx_)
{

	Coroutine& caller(*pcortctx_->m_pRoutine);

	for(;;)
	{

		Coroutine::main_coroutine().m_pThisRoutine.reset(&caller);
		caller.m_nState.store(Coroutine::STATE_RUNNING);

		// Coroutine::svc return and yield to main, in this case, m_pLastRoutine will be NULL
		if(caller.m_pLastRoutine)
		{
			caller.m_pLastRoutine->m_nState.store(Coroutine::STATE_PAUSED);
		}

		try
		{
			caller.svc();
		}
		catch(std::exception& e)
		{
			System::LogError("Unhandled exception in Coroutine::svc",e.what());
		}

		Coroutine& callee(Coroutine::main_coroutine());

		if(&caller==&callee)
		{
			System::LogFetal("Coroutine::main_coroutine enter CoroutineContext::raw_proc_spawn???");
		}

		int32_t val=Coroutine::STATE_PAUSED;
		if(!callee.m_nState.compare_exchange(val,Coroutine::STATE_PENDING))
		{
			System::LogFetal("MainCoroutine is busy???");
		}

		caller.m_nState.store(Coroutine::STATE_STOPPED);
		callee.m_pLastRoutine.reset(NULL);

		asm_swap_context(callee.m_pContext,caller.m_pContext);

	}


}

VHWD_LEAVE



#if !defined(_WIN32)

// move to external asm  file
/*
extern "C" void asm_swap_context(void* callee,void* caller)
{
__asm__ __volatile__ (
	"mov %rdi,%rcx;"
	"mov %rsi,%rdx;"
	"mov %rbp,-0x08(%rsp);"
	"mov %rbx,-0x10(%rsp);"
	"mov %rsi,-0x18(%rsp);"
	"mov %rdi,-0x20(%rsp);"
	"mov %r12,-0x28(%rsp);"
	"mov %r13,-0x30(%rsp);"
	"mov %r14,-0x38(%rsp);"
	"mov %r15,-0x40(%rsp);"

	"mov %rsp,(%rdx);"
	"mov (%rcx),%rsp;"

	"mov -0x08(%rsp),%rbp;"
	"mov -0x10(%rsp),%rbx;"
	"mov -0x18(%rsp),%rsi;"
	"mov -0x20(%rsp),%rdi;"
	"mov -0x28(%rsp),%r12;"
	"mov -0x30(%rsp),%r13;"
	"mov -0x38(%rsp),%r14;"
	"mov -0x40(%rsp),%r15;"

	"mov (%rsp),%rdx;"
	"cmpq $0,%rdx;"
	"jne .L1;"
	"mov %rcx,%rdi;"
	"call 0x8(%rsp);"
	".L1:"
	)
	;
}
*/

#elif defined(_X86_) && defined(_MSC_VER)

#pragma warning(disable:4731 4733)

extern "C" void asm_swap_context(void* callee,void* caller)
{
	__asm
	{
		mov ecx,dword ptr[callee];
		mov edx,dword ptr[caller];

		// ebp,ebx,esi,edi already in stack,
		// modify esp and make esp->return address
		add esp,10h;
		//mov dword ptr[esp-04h],ebp;
		//mov dword ptr[esp-08h],ebx;
		//mov dword ptr[esp-0Ch],esi;
		//mov dword ptr[esp-10h],edi;

		mov eax,fs:[0];
		mov dword ptr[esp-14h],eax;
		mov eax,fs:[4];
		mov dword ptr[esp-18h],eax;
		mov eax,fs:[8];
		mov dword ptr[esp-1Ch],eax;

		// switch stack
		mov dword ptr[edx],esp;
		mov esp,dword ptr[ecx];

		// restore stack
		mov eax,dword ptr[esp-1Ch];
		mov fs:[8],eax;
		mov eax,dword ptr[esp-18h];
		mov fs:[4],eax;
		mov eax,dword ptr[esp-14h];
		mov fs:[0],eax;

		mov edi,dword ptr[esp-10h];
		mov esi,dword ptr[esp-0Ch];
		mov ebx,dword ptr[esp-08h];
		mov ebp,dword ptr[esp-04h];

		// [esp] is return address
		mov edx,dword ptr[esp];
		cmp edx,0;
		jne lb_exit;

		// [esp] is zero means starting a new coroutine, call entry point at [esp+4]
		// this function call will never return;
		mov edx,dword ptr[esp+4h];
		push ecx;
		call edx;

		lb_exit:
		// esp already modified, return directly.
		ret;

	}
}
#endif


#endif // VHWD_USE_COROUTINE



