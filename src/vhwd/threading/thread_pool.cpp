#include "vhwd/threading/thread.h"
#include "vhwd/threading/thread_pool.h"
#include "vhwd/basic/lockguard.h"
#include "thread_impl.h"


VHWD_ENTER

ThreadPool::ThreadPool()
{
	flags().add(Thread::FLAG_DYNAMIC);
	m_nWorkerMin=1;
	m_nWorkerMax=4;
	m_nWorkerNum=0;
	m_nWorkerWait=0;
	m_nTaskHint=20;
}

void ThreadPool::set_worker_min(int n)
{
	if(n<1) return;
	m_nWorkerMin=n;
}

void ThreadPool::set_worker_max(int n)
{
	if(n<m_nWorkerMin) return;
	m_nWorkerMax=n;
}

int ThreadPool::get_worker_min()
{
	return m_nWorkerMin;
}

int ThreadPool::get_worker_max()
{
	return m_nWorkerMax;
}

int ThreadPool::get_worker_num()
{
	return m_nWorkerNum;
}

void ThreadPool::putq(ITask* hjob,void* pdat)
{
	size_t qs;

	LockGuard<Mutex> lock1(m_tMutex);
	m_tTaskQueue.append(new TaskItem(hjob,pdat));
	qs=m_tTaskQueue.size();

	if(m_nWorkerWait>0)
	{
		m_tCond.notify_one();
	}

	if(qs%m_nTaskHint==0)
	{	
		if(m_nWorkerNum<m_nWorkerMax)
		{
			this_logger().LogMessage("Too many tasks, creating new worker");
			if(basetype::activate())
			{
				m_nWorkerNum++;
			}
		}	
	}
}

void ThreadPool::reqexit()
{
	LockGuard<Mutex> lock1(m_tMutex);
	for(;;)
	{
		TaskItem* q=m_tTaskQueue.pop_front();
		if(!q) break;
		q->hjob->del(q->pdat);
		delete q;
	}

	basetype::reqexit();
	m_tCond.notify_all();
}

bool ThreadPool::activate()
{
	LockGuard<Mutex> lock1(m_tMutex);
	if(m_nWorkerNum!=0)
	{
		return false;
	}

	if(!ThreadImpl::activate(*this,m_nWorkerMin))
	{
		return false;
	}

	m_nWorkerNum=m_nWorkerMin;
	return true;

}


ThreadPool::TaskItem* ThreadPool::getq()
{
	LockGuard<Mutex> lock1(m_tMutex);
	for(;;)
	{
		if(test_destroy())
		{
			m_nWorkerNum--;
			return NULL;
		}

		TaskItem* q=m_tTaskQueue.pop_front();
		if(q!=NULL)
		{
			return q;
		}

		if(m_nWorkerWait>0 && m_nWorkerNum>m_nWorkerMin)
		{
			this_logger().LogMessage("too many workers, leaving");
			m_nWorkerNum--;
			return NULL;
		}

		m_nWorkerWait++;
		m_tCond.wait(m_tMutex);
		m_nWorkerWait--;
	}
}

void ThreadPool::svc()
{
	this_logger().LogMessage("WorkerThread enter");

	TaskItem* q=NULL;
	for(;;)
	{
		q=getq();
		if(!q)	break;
		q->hjob->svc(q->pdat);
		delete q;
	}

	this_logger().LogMessage("WorkerThread leave");
}

VHWD_LEAVE

