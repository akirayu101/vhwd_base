
#include "vhwd/ipc/shm_queue.h"
#include "vhwd/logging/logger.h"
#include "vhwd/threading/thread.h"
#include "vhwd/basic/console.h"
#include "vhwd/basic/system.h"

#include <cstdlib>

VHWD_ENTER


SharedMemQueue::SharedMemQueue()
{
	m_bOk=false;
}

bool SharedMemQueue::Create(const String& name,size_t qc,size_t bc,size_t bs)
{
	if(m_bOk)
	{
		return false;
	}

	size_t sb=IShmQueue::memsize(bc);
	size_t sq=IShmQueue::memsize(qc);
	size_t sa=8+sb+sq*2+bs*bc;

	if(!shm.Create(name,sa))
	{
		return false;
	}

	AtomicInt32* pGuard=(AtomicInt32*)shm.addr();
	bool cr=pGuard->exchange(0x123456)!=0x123456;

	char* p=shm.addr()+8;
	m_pBufferStart=p+sb+sq*2;

	m_nQueueCapacity=qc;
	m_nBufferSize=bs;
	m_nBufferCount=bc;

	m_qb.clear();
	m_qb.init(p,sb,cr);

	m_q1.clear();
	m_q1.init(p+sb,sq,cr);

	m_q2.clear();
	m_q2.init(p+sb+sq,sq,cr);

	if(cr)
	{
		m_qb.flags().add(IShmQueue::QUEUE_TIMEOUT);
		m_q1.flags().add(IShmQueue::QUEUE_TIMEOUT);
		m_q2.flags().add(IShmQueue::QUEUE_TIMEOUT);

		for(int i=0;i<(int)m_nBufferCount-1;i++)
		{
			m_qb.putq(i);
		}
	}

	if(m_qb.capacity()!=bc)
	{
		System::LogTrace("invalid qb capacity");
		return false;
	}

	if(m_q1.capacity()!=qc)
	{
		System::LogTrace("invalid q1 capacity");
		return false;
	}

	if(m_q2.capacity()!=qc)
	{
		System::LogTrace("invalid q2 capacity");
		return false;
	}


	m_bOk=true;
	return true;
}

void SharedMemQueue::Close()
{
	m_q1.flags().add(IShmQueue::QUEUE_NONBLOCK);
	m_q2.flags().add(IShmQueue::QUEUE_NONBLOCK);
	m_qb.flags().add(IShmQueue::QUEUE_NONBLOCK);
}


char* SharedMemQueue::Buffer_address(int32_t v)
{
	wassert(v>=0&&v<(int32_t)m_nBufferCount);
	return m_pBufferStart+m_nBufferSize*v;
}

VHWD_LEAVE
