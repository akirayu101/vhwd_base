#include "vhwd/threading/thread.h"
#include "vhwd/basic/lockguard.h"
#include "vhwd/threading/thread_pool.h"
#include "thread_impl.h"

#include <errno.h>

VHWD_ENTER


void Thread::_init()
{
	m_nState=0;
	m_nAlive=0;
	m_nFlags=0;
}

Thread::Thread()
{
	_init();
}

Thread::Thread(const Thread& o)
{
	_init();
}


Thread::~Thread()
{

}

bool Thread::alive()
{
	return m_nAlive>0;
}

int Thread::count()
{
	return m_nAlive;
}

void Thread::reqexit()
{
	LockGuard<Mutex> lock1(m_thrd_mutex);
	m_nState=STATE_CANCEL;
	m_cond_state_changed.notify_all();
}

void Thread::pause()
{
	m_nState|=STATE_PAUSED;
}

void Thread::resume()
{
	LockGuard<Mutex> lock1(m_thrd_mutex);
	m_nState|=~STATE_PAUSED;
	m_cond_state_changed.notify_all();
}

bool Thread::test_destroy()
{
	if((m_nState&STATE_ANY)==0)
	{
		return ThreadImpl::m_bReqExit;
	}

	m_thrd_mutex.lock();

	for(;;)
	{
		if((m_nState&STATE_CANCEL)!=0||ThreadImpl::m_bReqExit)
		{
			m_thrd_mutex.unlock();
			return true;
		}

		if(m_nState&STATE_PAUSED)
		{
			m_cond_state_changed.wait_for(m_thrd_mutex,100);
			continue;
		}

		break;
	}

	m_thrd_mutex.unlock();

	return ThreadImpl::m_bReqExit;
}





Thread& Thread::this_thread()
{
	return *ThreadImpl::this_data().thrd_ptr;
}


Logger& this_logger()
{
	return ThreadImpl::this_data().thrd_logger;
}


int Thread::rank()
{
	return ThreadImpl::this_data().thrd_rank;
}

uintptr_t Thread::id()
{
	return ThreadImpl_detail::get_id();
}


void Thread::set_priority(int n)
{
	ThreadImpl::this_data().set_priority(n);
}

void Thread::set_affinity(int n)
{
	ThreadImpl::this_data().set_affinity(n);
}

void Thread::svc()
{

}

void Thread::on_exception()
{
	try
	{
		throw;
	}
	catch(std::exception& e)
	{
		this_logger().LogError("unhandled_exception:%s",e.what());
	}
	catch(...)
	{
		this_logger().LogFetal("unhandled_exception");
	}
}

ThreadMulti::ThreadMulti()
{

}

bool ThreadEx::activate(factor_type& fac)
{
	InvokerGroup g;
	g.append(fac);
	return activate(g);
}

bool ThreadEx::activate(InvokerGroup& g)
{
	return ThreadImpl::activate(*this,g);
}


bool Thread::activate()
{
	return ThreadImpl::activate(*this,1);
}

bool ThreadMulti::activate(int n)
{
	return ThreadImpl::activate(*this,n);
}

bool ThreadMulti::activate()
{
	return ThreadImpl::activate(*this,1);
}

bool ThreadCustom::activate()
{
	return basetype::activate(m_aThreads);
}


void Thread::wait()
{
	LockGuard<Mutex> lock1(m_thrd_mutex);
	while(m_nAlive!=0)
	{
		m_cond_thrd_empty.wait(lock1);
	}
}

bool Thread::wait_for(int rel_time)
{
	LockGuard<Mutex> lock(m_thrd_mutex);
	if(m_nAlive!=0)
	{
		m_cond_thrd_empty.wait_for(m_thrd_mutex,rel_time);
	}
	return m_nAlive==0;
}

bool Thread::wait_for(const TimeSpan& ts)
{
	LockGuard<Mutex> lock(m_thrd_mutex);
	if(m_nAlive!=0)
	{
		m_cond_thrd_empty.wait_for(m_thrd_mutex,ts);
	}
	return m_nAlive==0;
}

bool Thread::wait_until(const TimePoint& tp)
{
	LockGuard<Mutex> lock(m_thrd_mutex);
	if(m_nAlive!=0)
	{
		m_cond_thrd_empty.wait_until(m_thrd_mutex,tp);
	}
	return m_nAlive==0;
}

void Thread::sleep_for(const TimeSpan& ts)
{
	sleep_for(ts.GetMilliSeconds());
}

void Thread::sleep_until(const TimePoint& tp)
{
	sleep_for((tp-Clock::now()).GetMilliSeconds());
}

void Thread::sleep_for(int milliseconds)
{
#ifdef VHWD_WINDOWS
	_sleep(milliseconds);
#else
	struct timespec req;
	req.tv_sec = milliseconds / 1000;
	req.tv_nsec = (milliseconds % 1000) * 1000000;
	while (nanosleep(&req, &req) != 0 && errno == EINTR);
#endif
}

void Thread::yield()
{
#ifdef VHWD_WINDOWS
	_sleep(0);
#else
	sched_yield();
#endif
}



VHWD_LEAVE

