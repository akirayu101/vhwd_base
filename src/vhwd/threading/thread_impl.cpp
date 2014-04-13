#include "thread_impl.h"
#include "vhwd/threading/lockguard.h"
#include "vhwd/threading/thread_pool.h"
VHWD_ENTER


ThreadImpl_detail::key_t threaddata_buffer_key;

void ThreadMain::wait()
{
	ThreadPool::current().wait();
}

bool ThreadMain::m_bReqExit=false;

ThreadImpl& ThreadImpl::data()
{
	ThreadImpl* p=(ThreadImpl*)ThreadImpl_detail::key_get(threaddata_buffer_key);
	if(!p)
	{
		static ThreadImpl d(ThreadPool::current());
		d.thrd_rank=0;
		d.thrd_ptr=&Thread::main_thread();
		return d;
	}
	return *p;
}


void ThreadImpl::set_priority(int n)
{
	if(n<0) n=0;
	if(n>100) n=100;
	thrd_priority=n;
	ThreadImpl_detail::set_priority(n);
}

void ThreadImpl::set_affinity(int n)
{
	//if(n<0) n=0;
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


bool ThreadImpl::activate(ThreadPool& pool,Thread& thrd,ThreadEx::InvokerGroup& g)
{
	int n=(int)g.size();
	if(n<=0) return false;

	LockGuard<Mutex> lock1(pool.m_thrd_mutex);
	if(pool.m_nFlags.get(ThreadPool::POOL_DISABLED))
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

	for(int i=0;i<n;i++)
	{
		_aThreads[i]=ThreadImpl::get_thread(pool);
		if(!_aThreads[i])
		{
			for(int j=0;j<i;j++)
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
	pool.m_thrd_attached.notify_all();

	return true;
}

bool ThreadImpl::activate(ThreadPool& pool,Thread& thrd,int n)
{
	if(n<=0) return false;

	LockGuard<Mutex> lock1(pool.m_thrd_mutex);
	if(pool.m_nFlags.get(ThreadPool::POOL_DISABLED))
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

	for(int i=0;i<n;i++)
	{
		_aThreads[i]=ThreadImpl::get_thread(pool);
		if(!_aThreads[i])
		{
			for(int j=0;j<i;j++)
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
	pool.m_thrd_attached.notify_all();

	return true;
}

ThreadImpl* ThreadImpl::get_thread(ThreadPool& pool)
{
	ThreadImpl* impl=NULL;

	if(pool.m_pThreads_free)
	{
		impl=pool.m_pThreads_free;
		pool.m_pThreads_free=impl->pNext;
		if(pool.m_pThreads_free)
		{
			pool.m_pThreads_free->pPrev=NULL;
		}

	}
	else
	{
		impl= new ThreadImpl(pool);
		if(!impl->create())
		{
			delete impl;
			return NULL;
		}
	}
	return impl;
}

bool ThreadImpl::put_thread(ThreadPool& pool,ThreadImpl* impl)
{
	if( !pool.m_nFlags.get(ThreadPool::POOL_DISABLED) )
	{
		impl->pNext=pool.m_pThreads_free;
		impl->pPrev=NULL;
		if(pool.m_pThreads_free)
		{
			pool.m_pThreads_free->pPrev=impl;
		}
		pool.m_pThreads_free=impl;
		return true;
	}
	else
	{
		return false;
	}
}

ThreadImpl::ThreadImpl(ThreadPool& p):pool(p)
{

	static bool first=true;
	if(first)
	{
		first=false;
		ThreadImpl_detail::key_create(threaddata_buffer_key);
	}

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

	{
		LockGuard<Mutex> lock(pool.m_thrd_mutex);
		if(!pool.m_nFlags.get(ThreadPool::POOL_DISABLED))
		{
			pool.m_nThreadNum++;
		}
		else
		{
			delete this;
			return false;
		}
	}

	ThreadImpl_detail::key_set(threaddata_buffer_key,this);
	return true;
}
void ThreadImpl::svc_leave()
{
	ThreadImpl_detail::key_set(threaddata_buffer_key,NULL);
	ThreadPool& mypool(pool);

	delete this;

	{
		LockGuard<Mutex> lock1(mypool.m_thrd_mutex);
		int nc=--mypool.m_nThreadNum;
		if(nc==0)
		{
			mypool.m_cond_thrd_empty.notify_all();
		}
	}
}

void ThreadImpl::svc()
{

	for(;;)
	{
		{
			LockGuard<Mutex> lock1(pool.m_thrd_mutex);
			while(thrd_ptr==NULL)
			{
				if(pool.m_nFlags.get(ThreadPool::POOL_NOCACHE|ThreadPool::POOL_DISABLED))
				{
					if(pPrev) pPrev->pNext=pNext;
					if(pNext) pNext->pPrev=pPrev;

					//for(size_t i=0;i<pool.m_aThreads_free.size();i++)
					//{
					//	if(pool.m_aThreads_free[i]!=this) continue;
					//	std::swap(pool.m_aThreads_free[i],pool.m_aThreads_free.back());
					//	pool.m_aThreads_free.pop_back();break;
					//}

					return;
				}
				pool.m_thrd_attached.wait(lock1);
			}
			pool.m_nThreadJob++;
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
			LockGuard<Mutex> lock1(pool.m_thrd_mutex);
			pool.m_nThreadJob--;
			if(!put_thread(pool,this))
			{
				return;
			}
		}
	}


}


VHWD_LEAVE
