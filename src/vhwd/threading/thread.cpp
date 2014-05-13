#include "vhwd/threading/thread.h"
#include "vhwd/basic/lockguard.h"
#include "vhwd/threading/thread_pool.h"
#include "thread_impl.h"

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
		return ThreadMain::m_bReqExit;
	}

	LockGuard<Mutex> lock1(m_thrd_mutex);

	for(;;)
	{
		if((m_nState&STATE_CANCEL)!=0||ThreadMain::m_bReqExit)
		{
			return true;
		}

		if(m_nState&STATE_PAUSED)
		{
			m_cond_state_changed.wait_for(lock1,1000);
			continue;
		}

		break;
	}

	return ThreadMain::m_bReqExit;
}


Thread& Thread::main_thread()
{
	static ThreadMain t;
	return t;
}

ThreadImpl& Thread::this_data()
{
	return ThreadImpl::data();
}

Thread& Thread::this_thread()
{
	return *this_data().thrd_ptr;
}


Logger& this_logger()
{
	return Thread::this_data().thrd_logger;	
}




int Thread::rank()
{
	return this_data().thrd_rank;
}


void Thread::set_priority(int n)
{
	this_data().set_priority(n);
}

void Thread::set_affinity(int n)
{
	this_data().set_affinity(n);
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
#ifdef _WIN32
	_sleep(milliseconds);
#else
	struct timespec req;
	req.tv_sec = milliseconds/1000;
	req.tv_nsec = (milliseconds%1000) * 1000000L;
	nanosleep (&req, &req);
#endif
}

void Thread::yield()
{
#ifdef _WIN32
	_sleep(0);
#else
	sched_yield();
#endif
}



VHWD_LEAVE

