#include "vhwd/threading/thread.h"
#include "vhwd/logging/logger.h"
#include "thread_impl_detail.h"


#ifdef VHWD_USE_COROUTINE
#include "vhwd/threading/coroutine.h"
#endif

VHWD_ENTER


class VHWD_DLLIMPEXP ThreadImpl : public NonCopyable
{
public:

	enum
	{
		THREADMANAGER_DISABLED	=1<<0,
		THREADMANAGER_NOCAHCING	=1<<1,
	};

	static ThreadImpl* get_thread();
	static bool put_thread(ThreadImpl* impl);

	static ThreadImpl& data();

	static bool activate(Thread& thrd,int n);
	static bool activate(Thread& thrd,ThreadEx::InvokerGroup& g);


	ThreadImpl();
	~ThreadImpl();

	void set_thread(Thread* p,ThreadEx::factor_type v,int i);

	void svc();
	bool svc_enter();
	void svc_leave();
	bool create();

	void set_priority(int n);
	void set_affinity(int n);

	int thrd_rank;
	Thread* thrd_ptr;
	Logger thrd_logger;

	int thrd_priority;
	int thrd_affinity;

	bool thrd_created;

	ThreadImpl_detail::thread_t thrd_id;
	ThreadEx::factor_type invoker;

	ThreadManager& tmgr;

#ifdef VHWD_USE_COROUTINE
	CoroutineMain cort_main;
#endif

	LitePtrT<ThreadImpl> pNext;
	LitePtrT<ThreadImpl> pPrev;

};

class ThreadMain : public Thread
{
public:
	static bool m_bReqExit;

	ThreadMain(){}
	ThreadMain(const ThreadMain&);

	bool test_destroy()
	{
		return m_bReqExit;
	}

	void reqexit()
	{
		m_bReqExit=true;
	}

	void wait();

};

//extern ThreadMain _g_main_thread;

};//namespace

