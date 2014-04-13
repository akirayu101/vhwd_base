#include "vhwd/threading/thread.h"
#include "vhwd/threading/lockguard.h"
#include "vhwd/threading/thread_pool.h"
#include "thread_impl.h"

VHWD_ENTER

ThreadRoutine& ThreadRoutine::current()
{
	static ThreadRoutine gInstance;
	return gInstance;
}

ThreadRoutine::ThreadRoutine()
{
	reset(1024);
}

void ThreadRoutine::reset(int qmax)
{
	m_lfqSignals.reset(qmax*4);
	m_aFunctions.resize(qmax);
	m_lfqSignals.flags().add(LFQueue::QUEUE_NONBLOCK);
	
}

void ThreadRoutine::clear(int32_t q)
{
	m_lfqSignals.putq(-q);
}

void ThreadRoutine::sigq(int32_t s)
{
	if(s<0||s>=(int)m_aFunctions.size())
	{
		return;
	}

	m_lfqSignals.putq(s);
}

int ThreadRoutine::bind(Factor<int()> f)
{
	for(int i=2;i<(int)m_aFunctions.size();i++)
	{
		if(m_aFunctions[i]) continue;
		m_aFunctions[i]=f;
		return i;
	}
	return -1;
}
	

void ThreadRoutine::svc()
{
	while(!test_destroy())
	{
		int32_t q=m_lfqSignals.getq();

		if(q<-1) // delete state
		{
			q=-q;
			if(m_aFunctions[q])
			{
				m_aFunctions[q].clear();				
			}
			else
			{
				System::LogTrace("ThreadRoutine: invalid q: %d",-q);
			}
			continue;
		}
		else if(q>0 && q<(int)m_aFunctions.size())
		{
			if(m_aFunctions[q])
			{
				int r=m_aFunctions[q]();
				if(r>0)
				{
					m_lfqSignals.putq(r);
				}
				else if(r==-1)
				{
					m_aFunctions[q].clear();
				}
			}
			else
			{
				System::LogTrace("ThreadRoutine: invalid slot: %d",q);
			}
		}
		else
		{
			Thread::sleep_for(300);
		}
	}
}


VHWD_LEAVE
