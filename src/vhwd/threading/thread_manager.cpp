#include "vhwd/threading.h"
#include "thread_impl.h"

#undef new

VHWD_ENTER

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
