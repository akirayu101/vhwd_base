#include "vhwd/threading.h"
#include "thread_impl.h"

VHWD_ENTER


ThreadManager& ThreadManager::current()
{
	static class StaticThreadPool : public StaticObjectWithoutDeletorT<ThreadManager>
	{
	public:

		StaticThreadPool() {}

		~StaticThreadPool()
		{

		}

	} _g_thread_pool;
	return _g_thread_pool;
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
	return m_nThreadNum;
}

void ThreadManager::close()
{
	LockGuard<Mutex> lock1(m_thrd_mutex);
	m_nFlags.add(ThreadImpl::THREADMANAGER_DISABLED);
	m_thrd_attached.notify_all();
}

void ThreadManager::wait()
{
	LockGuard<Mutex> lock1(m_thrd_mutex);
	while(m_nThreadNum!=0)
	{
		m_cond_thrd_empty.wait(lock1);
	}
}


VHWD_LEAVE
