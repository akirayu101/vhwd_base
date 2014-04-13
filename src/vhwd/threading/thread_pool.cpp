#include "vhwd/threading/thread.h"
#include "vhwd/threading/thread_pool.h"
#include "thread_impl.h"
#include "vhwd/threading/lockguard.h"

VHWD_ENTER


ThreadPool& ThreadPool::current()
{
	static class StaticThreadPool : public StaticObjectWithoutDeletorT<ThreadPool>
	{
	public:

		StaticThreadPool(){}

		~StaticThreadPool()
		{

		}
		
	}_g_thread_pool;
	return _g_thread_pool;
}

ThreadPool::ThreadPool()
{
	m_nThreadMax=10;
	m_nThreadNum=0;
	m_nThreadJob=0;
}

ThreadPool::~ThreadPool()
{
	close();
	wait();
}

int ThreadPool::count()
{
	return m_nThreadNum;
}

void ThreadPool::close()
{
	LockGuard<Mutex> lock1(m_thrd_mutex);
	m_nFlags.add(POOL_DISABLED);
	m_thrd_attached.notify_all();
}

void ThreadPool::wait()
{
	LockGuard<Mutex> lock1(m_thrd_mutex);
	while(m_nThreadNum!=0)
	{
		m_cond_thrd_empty.wait(lock1);
	}
}



};//namespace
