#include "thread_impl.h"
#include "vhwd/basic/lockguard.h"
#include "vhwd/threading/thread_pool.h"
VHWD_ENTER


VHWD_THREAD_TLS ThreadImpl* tc_impl_data;


void tc_init();
void tc_gc();
void tc_cleanup();

bool ThreadImpl::m_bReqExit=false;

void ThreadImpl::set_priority(int n)
{
	if(n<0) n=0;
	if(n>100) n=100;
	thrd_priority=n;
	ThreadImpl_detail::set_priority(n);
}

void ThreadImpl::set_affinity(int n)
{
	thrd_affinity=n;
	ThreadImpl_detail::set_affinity(n);
}

void thread_impl_entry_real(ThreadImpl* impl)
{

	if(impl->svc_enter())
	{
		impl->svc();
		impl->svc_leave();
	}

}


bool ThreadImpl::activate(Thread& thrd,ThreadEx::InvokerGroup& g)
{
	ThreadManager& tmgr(ThreadManager::current());

	int n=(int)g.size();
	if(n<=0) return false;

	LockGuard<Mutex> lock1(tmgr.m_thrd_mutex);
	if(tmgr.m_nFlags.get(ThreadImpl::THREADMANAGER_DISABLED))
	{
		return false;
	}

	LockGuard<Mutex> lock2(thrd.m_thrd_mutex);
	if(!thrd.m_nFlags.get(Thread::FLAG_DYNAMIC) && thrd.m_nAlive!=0)
	{
		return false;
	}

	arr_1t<ThreadImpl*> _aThreads;
	_aThreads.resize(n,NULL);

	for(int i=0; i<n; i++)
	{
		_aThreads[i]=ThreadImpl::get_thread();
		if(!_aThreads[i])
		{
			for(int j=0; j<i; j++)
			{
				_aThreads[i]->set_thread(NULL,ThreadEx::factor_type(),-1);
			}
			return false;
		}
		if(i<(int)thrd.m_aBindCpu.size())
		{
			_aThreads[i]->thrd_affinity=thrd.m_aBindCpu[i];
		}
		else
		{
			_aThreads[i]->thrd_affinity=-1;
		}
		_aThreads[i]->set_thread(&thrd,g[i],i);
	}

	thrd.m_nAlive+=n;
	tmgr.m_thrd_attached.notify_all();

	return true;
}

bool ThreadImpl::activate(Thread& thrd,int n)
{
	ThreadManager& tmgr(ThreadManager::current());

	if(n<=0) return false;

	LockGuard<Mutex> lock1(tmgr.m_thrd_mutex);
	if(tmgr.m_nFlags.get(ThreadImpl::THREADMANAGER_DISABLED))
	{
		return false;
	}

	LockGuard<Mutex> lock2(thrd.m_thrd_mutex);
	if(!thrd.m_nFlags.get(Thread::FLAG_DYNAMIC) && thrd.m_nAlive!=0)
	{
		return false;
	}

	arr_1t<ThreadImpl*> _aThreads;
	_aThreads.resize(n,NULL);

	for(int i=0; i<n; i++)
	{
		_aThreads[i]=ThreadImpl::get_thread();
		if(!_aThreads[i])
		{
			for(int j=0; j<i; j++)
			{
				_aThreads[i]->set_thread(NULL,ThreadEx::factor_type(),-1);
			}
			return false;
		}
		if(i<(int)thrd.m_aBindCpu.size())
		{
			_aThreads[i]->thrd_affinity=thrd.m_aBindCpu[i];
		}
		else
		{
			_aThreads[i]->thrd_affinity=-1;
		}
		_aThreads[i]->set_thread(&thrd,ThreadEx::factor_type(),i);
	}

	thrd.m_aBindCpu.clear();
	thrd.m_nAlive+=n;
	tmgr.m_thrd_attached.notify_all();

	return true;
}

ThreadImpl* ThreadImpl::get_thread()
{
	ThreadManager& tmgr(ThreadManager::current());

	ThreadImpl* impl=NULL;

	if(tmgr.m_pThreads_free)
	{
		impl=tmgr.m_pThreads_free;
		tmgr.m_pThreads_free=impl->pNext;
		if(tmgr.m_pThreads_free)
		{
			tmgr.m_pThreads_free->pPrev=NULL;
		}

	}
	else
	{
		impl= new ThreadImpl();
		if(!impl->create())
		{
			delete impl;
			return NULL;
		}
	}
	return impl;
}

bool ThreadImpl::put_thread(ThreadImpl* impl)
{
	ThreadManager& tmgr(ThreadManager::current());

	if( !tmgr.m_nFlags.get(ThreadImpl::THREADMANAGER_DISABLED) )
	{
		impl->pNext=tmgr.m_pThreads_free;
		impl->pPrev=NULL;
		if(tmgr.m_pThreads_free)
		{
			tmgr.m_pThreads_free->pPrev=impl;
		}
		tmgr.m_pThreads_free=impl;
		return true;
	}
	else
	{
		return false;
	}
}



ThreadImpl::ThreadImpl()
{
	thrd_rank=-1;
	thrd_ptr=NULL;
	thrd_affinity=0;
	thrd_priority=50;
}

ThreadImpl::~ThreadImpl()
{
	if(thrd_created)
	{
		ThreadImpl_detail::thread_destroy(thrd_id);
	}
}

bool ThreadImpl::create()
{
	thrd_created=ThreadImpl_detail::thread_create(thrd_id,this);
	return thrd_created;
}


void ThreadImpl::set_thread(Thread* p,ThreadEx::factor_type v,int i)
{
	thrd_ptr=p;
	invoker=v;
	thrd_rank=i;
}


bool ThreadImpl::svc_enter()
{
	ThreadManager& tmgr(ThreadManager::current());
	{
		LockGuard<Mutex> lock(tmgr.m_thrd_mutex);
		if(!tmgr.m_nFlags.get(ThreadImpl::THREADMANAGER_DISABLED))
		{
			tmgr.m_nThreadNum++;
		}
		else
		{
			delete this;
			return false;
		}
	}

	tc_impl_data=this;

	tc_init();
	return true;
}

void ThreadImpl::svc_leave()
{
	ThreadManager& tmgr(ThreadManager::current());

	tc_cleanup();
	tc_impl_data=NULL;

	delete this;

	{
		LockGuard<Mutex> lock1(tmgr.m_thrd_mutex);
		int nc=--tmgr.m_nThreadNum;
		if(nc==0)
		{
			tmgr.m_cond_thrd_empty.notify_all();
		}
	}
}

void ThreadImpl::svc()
{
	ThreadManager& tmgr(ThreadManager::current());
	for(;;)
	{
		{
			LockGuard<Mutex> lock1(tmgr.m_thrd_mutex);
			while(thrd_ptr==NULL)
			{
				if(tmgr.m_nFlags.get(ThreadImpl::THREADMANAGER_NOCAHCING|ThreadImpl::THREADMANAGER_DISABLED))
				{
					if(pPrev) pPrev->pNext=pNext;
					if(pNext) pNext->pPrev=pPrev;
					return;
				}
				tmgr.m_thrd_attached.wait(lock1);
			}
			tmgr.m_nThreadJob++;
		}


		if(thrd_priority!=50) set_priority(50);
		set_affinity(thrd_affinity);


		thrd_ptr->test_destroy();
		{
			try
			{
				if(!invoker)
				{
					thrd_ptr->svc();
				}
				else
				{
					invoker();
				}
			}
			catch(...)
			{
				thrd_ptr->on_exception();
			}

			tc_gc();
		}

		{
			LockGuard<Mutex> lock1(thrd_ptr->m_thrd_mutex);
			int tc=--thrd_ptr->m_nAlive;
			if(tc==0)
			{
				thrd_ptr->m_nState=0;
				thrd_ptr->m_cond_thrd_empty.notify_all();
			}
			thrd_ptr=NULL;
			invoker.clear();

		}

		{
			LockGuard<Mutex> lock1(tmgr.m_thrd_mutex);
			tmgr.m_nThreadJob--;
			if(!put_thread(this))
			{
				return;
			}
		}
	}


}


ThreadImpl* _p_MainThreadImplData=NULL;
ThreadImpl* _p_DummyThreadImplData=NULL;

class ThreadImplMain : public ThreadImpl
{
public:

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

	ThreadMain thrd;

	ThreadImplMain()
	{
		thrd_rank=0;
		thrd_ptr=&thrd;
	}


} _g_MainThreadImpl;


Thread& Thread::main_thread()
{
	return _g_MainThreadImpl.thrd;
}

ThreadImpl& ThreadImpl::dummy_data()
{
	if(_p_MainThreadImplData==NULL)
	{
		tc_impl_data=&_g_MainThreadImpl;
		_p_MainThreadImplData=&_g_MainThreadImpl;
		return _g_MainThreadImpl;
	}

	if(_p_DummyThreadImplData==NULL)
	{
		System::LogTrace("unknown thread calling this_data");

		static ThreadImpl d;
		static Thread t;
		d.thrd_rank=0;
		d.thrd_ptr=&t;

		_p_DummyThreadImplData=&d;
	}

	return *_p_DummyThreadImplData;
}

VHWD_LEAVE
