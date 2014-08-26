#include "vhwd/threading.h"
#include "thread_impl.h"

#undef new

VHWD_ENTER

ThreadImpl_detail::key_t threaddata_buffer_key;

class ThreadMain : public Thread
{
public:
	ThreadMain(){}
	bool test_destroy(){return ThreadImpl::m_bReqExit;}
	void reqexit(){ThreadImpl::m_bReqExit=1;}
	void wait(){ThreadManager::current().wait();}
	bool activate(){return false;}
	bool alive(){return true;}

};

ThreadMain g_MainThread;

Thread& Thread::main_thread()
{
	return g_MainThread;
}

class ThreadMainImpl : public ThreadImpl
{
public:

	ThreadMainImpl()
	{
		thrd_rank=0;
		thrd_ptr=&g_MainThread;

		ThreadManager::current();

		if(threaddata_buffer_key==0)
		{
			ThreadImpl_detail::key_create(threaddata_buffer_key);
			ThreadImpl_detail::key_set(threaddata_buffer_key,this);
		}

	}
	
	~ThreadMainImpl(){}
};

ThreadMainImpl gMainThreadImpl;


ThreadImpl& Thread::this_data()
{
	ThreadImpl* p=(ThreadImpl*)ThreadImpl_detail::key_get(threaddata_buffer_key);
	if(!p)
	{
		if(threaddata_buffer_key==0)
		{
			return gMainThreadImpl;
		}

		if(!p)
		{
			System::LogTrace("unknown thread");
			static ThreadImpl d;
			static Thread t;
			d.thrd_rank=0;
			d.thrd_ptr=&t;
			return d;
		}
	}
	return *p;
}


ThreadManager& ThreadManager::current()
{
	static ThreadManager tm;
	return tm;
}


ThreadManager::ThreadManager()
{
	m_nThreadMax=10;
	m_nThreadNum=0;
	m_nThreadJob=0;
}

ThreadManager::~ThreadManager()
{
	close();
	wait();
}


int ThreadManager::count()
{
	return m_nThreadNum.get();
}

void ThreadManager::close()
{
	LockGuard<Mutex> lock1(m_thrd_mutex);
	m_nFlags.add(ThreadImpl::THREADMANAGER_DISABLED);
	m_thrd_attached.notify_all();
}

void ThreadManager::wait()
{
	BitFlags oldflag=m_nFlags;
	LockGuard<Mutex> lock1(m_thrd_mutex);
	m_nFlags.add(ThreadImpl::THREADMANAGER_DISABLED);
	m_thrd_attached.notify_all();
	while(m_nThreadNum.load()!=0)
	{
		m_cond_thrd_empty.wait_for(lock1,100);
	}
	m_nFlags=oldflag;
}


VHWD_LEAVE
