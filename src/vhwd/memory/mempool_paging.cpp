#include "mempool_impl.h"
#include "vhwd/threading/thread_manager.h"
#include "vhwd/threading/thread.h"
#include "vhwd/basic/lockguard.h"
#include "vhwd/basic/system.h"


#ifdef VHWD_WINDOWS
#include <windows.h>
#else
#include <sys/mman.h>
#include <cstring>
#endif


VHWD_ENTER


void MpAllocGlobal::init()
{
	spanalloc.alloc_batch();

	size_t k1=0;
	size_t k2=0;
	for(size_t i=0;i<sl_size;i++)
	{
		size_t sz=FixAllocUnits[i];
		aSlots[i].init(sz);

		if(sz<=sz_slot1)
		{
			size_t kn=sz>>sz_bits1;
			for(size_t k=k1;k<=kn;k++)
			{
				pSlot1[k]=&aSlots[i];
			}
			k1=kn+1;
		}
		else if(sz<=sz_slot2)
		{
			size_t kn=sz>>sz_bits2;
			for(size_t k=k2;k<=kn;k++)
			{
				pSlot2[k]=&aSlots[i];
			}
			k2=kn+1;
		}
	}
}


MpAllocNode* MpAllocGlobal::dealloc_batch_nolock(MpAllocSlot& sl,MpAllocNode* fp,size_t sz)
{

	while(fp)
	{
		if(sz==0)
		{
			return fp;
		}
		sz--;

		MpAllocNode* fn=fp;fp=fp->nd_next;
		MpAllocBucket* pbk=pgmap.find_bucket(fn);

		if(!pbk||pbk->get()->sl_slot!=&sl)
		{
			System::LogError("dealloc_batch failed");
			// drop link;
			return NULL;
		}

		MpAllocSpan* sp=pbk->get();
		fn->nd_next=sp->nd_free;
		sp->nd_free=fn;

		if(!fn->nd_next)
		{
			sp->sl_next=sl.sp_head;
			sp->sl_prev=NULL;
			sl.sp_head=sp;
			if(sp->sl_next)
			{
				sp->sl_next->sl_prev=sp;
			}
		}

		if(--sp->nd_nnum!=0||sp->sl_next==NULL)
		{
			continue;
		}

		if(sl.sp_head==sp)
		{
			sl.sp_head=sp->sl_next;
			sp->sl_next->sl_prev=NULL;
		}
		else
		{
			sp->sl_next->sl_prev=sp->sl_prev;
			sp->sl_prev->sl_next=sp->sl_next;
		}		


		pbk->set(NULL);

		if(sp->sp_flag.get_fly())
		{
			page_free(reinterpret_cast<void*>(sp->sp_base),sp->sp_size);
			span_spin.lock();
			spanalloc.dealloc(sp);
			span_spin.unlock();
		}
		else
		{
			page_free(reinterpret_cast<void*>(sp->sp_base),sp->sp_size);
		}
	}

	return NULL;
}


MpAllocNode* MpAllocGlobal::alloc_small_batch(MpAllocSlot& sl,size_t& sz)
{
	sl.sl_spin.lock();

	if(!sl.sp_head)
	{
		sl.sp_head=create_span_nolock(sl);
		if(!sl.sp_head)
		{
			sl.sl_spin.unlock();
			return NULL;
		}
	}
	
	sz=sl.nd_nall-sl.sp_head->nd_nnum;

	MpAllocNode* nd=sl.sp_head->nd_free;
	sl.sp_head->nd_free=NULL;
	sl.sp_head->nd_nnum=sl.nd_nall;
	
	sl.sp_head=sl.sp_head->sl_next;
	if(sl.sp_head)
	{
		sl.sp_head->sl_prev=NULL;
	}


	sl.sl_spin.unlock();
	return nd;
}



void* MpAllocGlobal::realloc_real(void* p,size_t n,MpAllocBucket& bk)
{
	MpAllocSpan* sp=bk.get();
	size_t n0=0;
	if(sp->sl_slot)
	{
		n0=sp->sl_slot->nd_size;
	}
	else if(sp->sp_flag.get_fly())
	{
		n0=sp->sp_size-sizeof(MpAllocSpan);
	}
	else
	{
		n0=sp->sp_size;
	}

	if(n<=n0)
	{
		return p;
	}

	void* m=alloc(n);

	if(!m)
	{
		return NULL;
	}

	memcpy(m,p,n0);
	dealloc_real(p,bk);
	return m;
}



void MpAllocGlobal::dealloc_real(void* p,MpAllocBucket& bk)
{
	MpAllocSpan* sp=bk.get();
	if(sp->sl_slot)
	{
		MpAllocSlot& sl(*sp->sl_slot);
		sl.sl_spin.lock();

		MpAllocNode* fn=(MpAllocNode*)p;
		fn->nd_next=sp->nd_free;
		sp->nd_free=fn;

		if(!fn->nd_next)
		{
			sp->sl_next=sl.sp_head;
			sp->sl_prev=NULL;
			sl.sp_head=sp;
			if(sp->sl_next)
			{
				sp->sl_next->sl_prev=sp;
			}
		}

		if(--sp->nd_nnum!=0||sp->sl_next==NULL)
		{
			sl.sl_spin.unlock();
			return;
		}

		if(sl.sp_head==sp)
		{
			sl.sp_head=sp->sl_next;
			sp->sl_next->sl_prev=NULL;
		}
		else
		{
			sp->sl_next->sl_prev=sp->sl_prev;
			sp->sl_prev->sl_next=sp->sl_next;
		}

		sl.sl_spin.unlock();
	}

	bk.set(NULL);
	if(sp->sp_flag.get_fly())
	{
		page_free(reinterpret_cast<void*>(sp->sp_base),sp->sp_size);
		span_spin.lock();
		spanalloc.dealloc(sp);
		span_spin.unlock();
	}
	else
	{
		page_free(reinterpret_cast<void*>(sp->sp_base),sp->sp_size);
	}

}


void* MpAllocGlobal::alloc_small(size_t n,MpAllocSlot& sl)
{
	wassert(n<=sl.nd_size);

	sl.sl_spin.lock();

	if(!sl.sp_head)
	{
		sl.sp_head=create_span_nolock(sl);
		if(!sl.sp_head)
		{
			sl.sl_spin.unlock();
			return NULL;
		}
	}
		
	MpAllocNode* nd=sl.sp_head->nd_free;
	sl.sp_head->nd_free=nd->nd_next;
	sl.sp_head->nd_nnum++;

	if(!sl.sp_head->nd_free)
	{
		sl.sp_head=sl.sp_head->sl_next;
		if(sl.sp_head)
		{
			sl.sp_head->sl_prev=NULL;
		}
	}

	sl.sl_spin.unlock();
	return nd;
}

void* MpAllocGlobal::alloc_large(size_t n)
{
	size_t sz=(n+MpAllocConfig::pg_mask)&(~MpAllocConfig::pg_mask);
	char* p1=(char*)page_alloc(sz);

	if(sz-n>=sizeof(MpAllocSpan))
	{
		MpAllocSpan* sp=(MpAllocSpan*)(p1+sz-sizeof(MpAllocSpan));
		sp->sp_base=reinterpret_cast<uintptr_t>(p1);
		sp->sp_size=sz-sizeof(MpAllocSpan);
		sp->sp_flag.set_fly(false);
		sp->sl_slot=NULL;

		pgmap.insert_span_nolock(sp);
		return p1;
	}
	else
	{
		MpAllocSpan* sp;
		{
			span_spin.lock();
			sp=spanalloc.alloc();
			span_spin.unlock();
		}

		if(!sp)
		{
			page_free(p1,sz);
			return NULL;
		}

		sp->sp_base=reinterpret_cast<uintptr_t>(p1);
		sp->sp_size=sz;
		sp->sl_slot=NULL;
		sp->sp_flag.set_fly(true);

		pgmap.insert_span_nolock(sp);
		return p1;
	}
}


MpAllocSpan* MpAllocGlobal::create_span_nolock(MpAllocSlot& sl)
{
	size_t sz=MpAllocConfig::sp_size;

	MpAllocNode* p1=(MpAllocNode*)page_alloc(sz);

	if(!p1)
	{
		return NULL;
	}

	MpAllocSpan* sp;
	if(sl.sp_flag.get_fly())
	{
		span_spin.lock();

		sp=spanalloc.alloc();
		if(!sp)
		{
			page_free(p1,sz);
			span_spin.unlock();
			return NULL;
		}
		sp->sp_flag.set_fly(true);

		span_spin.unlock();
	}
	else
	{
		sp=(MpAllocSpan*)(((char*)p1)+sz-sizeof(MpAllocSpan));
		sp->sp_flag.set_fly(false);
	}

	sp->sp_base=reinterpret_cast<uintptr_t>(p1);
	sp->sp_size=sz;
	sp->nd_nnum=0;
	sp->nd_free=p1;
	sp->sl_slot=&sl;
	sp->sl_next=NULL;
	sp->sl_prev=NULL;

	MpAllocNode::link(p1,sl.nd_nall*sl.nd_size,sl.nd_size,NULL);

	pgmap.insert_span_nolock(sp);
	return sp;
}

void tc_gc();

void MpAllocGlobal::gc()
{

	tc_gc();

	for(size_t i=0;i<sl_size;i++)
	{
		MpAllocSlot& sl(aSlots[i]);
		if(!sl.sl_spin.try_lock())
		{
			continue;
		}

		MpAllocSpan* sp=sl.sp_head;
		while(sp)
		{
			if(sp->nd_nnum!=0)
			{
				sp=sp->sl_next;
				continue;
			}

			MpAllocSpan* kk=sp;
			sp=sp->sl_next;

			if(sp) sp->sl_prev=kk->sl_prev;
			if(kk->sl_prev)
			{
				kk->sl_prev->sl_next=sp;
			}
			else
			{
				sl.sp_head=sp;
			}

			MpAllocBucket* pbk=g_myalloc_impl->pgmap.find_bucket(reinterpret_cast<void*>(kk->sp_base));

			if(pbk)
			{
				wassert(pbk->get()==kk);
				pbk->set(NULL);
			}
			else
			{
				System::LogError("MpAllocSpan not in pgmap");
			}

			if(kk->sp_flag.get_fly())
			{
				page_free(reinterpret_cast<void*>(kk->sp_base),kk->sp_size);
				span_spin.lock();
				spanalloc.dealloc(kk);
				span_spin.unlock();
			}
			else
			{
				page_free(reinterpret_cast<void*>(kk->sp_base),kk->sp_size);
			}
		}
		sl.sl_spin.unlock();
	}

}



MpAllocGlobal *g_myalloc_impl=NULL;
AtomicSpin g_myalloc_spin;

void tc_init();

void mp_init()
{
	if(!g_myalloc_impl)
	{

		LockGuard<AtomicSpin> lock1(g_myalloc_spin);
		if(g_myalloc_impl) return;

#if defined(VHWD_WINDOWS) && defined(_DEBUG)
		_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG)|_CRTDBG_LEAK_CHECK_DF);
#endif
		g_myalloc_impl=(MpAllocGlobal*)page_alloc(sizeof(MpAllocGlobal));
		if(!g_myalloc_impl)
		{
			System::LogFetal("Cannot initialize MpAllocGlobal");
		}
		g_myalloc_impl->init();

		tc_init();
	}
}

VHWD_DLLIMPEXP void* mp_alloc(size_t n)
{
	if(!g_myalloc_impl)
	{
		mp_init();
	}
	void* m=g_myalloc_impl->alloc(n);
	if(!m)
	{
		errno=ENOMEM;
	}
	return m;
}

VHWD_DLLIMPEXP void mp_free(void* p)
{
	if(!g_myalloc_impl)
	{
		mp_init();
	}
	g_myalloc_impl->dealloc(p);
}

VHWD_DLLIMPEXP void* mp_realloc(void* p,size_t n)
{
	if(!g_myalloc_impl)
	{
		mp_init();
	}
	void* m=g_myalloc_impl->realloc(p,n);
	if(!m&&n>0)
	{
		errno=ENOMEM;
	}
	return m;
}

void mp_init();

void* MemPoolPaging::allocate(size_t n)
{
	if(!g_myalloc_impl)
	{
		mp_init();
	}

	void* p=g_myalloc_impl->alloc(n);
	if(!p)
	{
		Exception::XBadAlloc();
	}
	return p;
}

void MemPoolPaging::deallocate(void* p)
{
	if(!g_myalloc_impl)
	{
		::free(p);
		return;
	}

	g_myalloc_impl->dealloc(p);
}



VHWD_LEAVE
