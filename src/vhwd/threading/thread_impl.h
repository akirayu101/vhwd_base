#include "vhwd/threading/thread.h"
#include "vhwd/logging/logger.h"
#include "thread_impl_detail.h"
#include "vhwd/threading/coroutine.h"


VHWD_ENTER

class VHWD_DLLIMPEXP MpAllocCachedNoLock;

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

	//static ThreadImpl& data();

	static bool activate(Thread& thrd,int n);
	static bool activate(Thread& thrd,ThreadEx::InvokerGroup& g);


	ThreadImpl();
	ThreadImpl(ThreadManager& tm);
	~ThreadImpl();

	void set_thread(Thread* p,ThreadEx::factor_type v,int i);

	void svc();
	bool svc_enter();
	void svc_leave();
	bool create();

	void set_priority(int n);
	void set_affinity(int n);


	Thread* thrd_ptr;
	Logger thrd_logger;

	int thrd_rank;
	int thrd_priority;
	int thrd_affinity;

	bool thrd_created;

	ThreadImpl_detail::thread_t thrd_id;
	ThreadEx::factor_type invoker;

	ThreadManager& tmgr;

	CoroutineMain cort_main;

	LitePtrT<ThreadImpl> pNext;
	LitePtrT<ThreadImpl> pPrev;

	static bool m_bReqExit;

};



VHWD_LEAVE

