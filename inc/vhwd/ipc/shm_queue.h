#ifndef __H_VHWD_IPC_SHM_QUEUE__
#define __H_VHWD_IPC_SHM_QUEUE__


#include "vhwd/ipc/shm.h"
#include "vhwd/basic/system.h"
#include "vhwd/basic/bitflags.h"
#include "vhwd/collection/lockfree_queue.h"
#include "vhwd/threading/thread.h"

VHWD_ENTER

template<typename T>
class ShmQueuePolicy : public LockFreeQueuePolicy<T>
{
public:

    typedef typename LockFreeQueuePolicy<T>::size_type size_type;

	static inline T invalid_value(){return -1;}

	static inline void fill(T* p,size_type s)
	{
		for(size_type i=0;i<s;i++) p[s]=-1;
	}

	static inline void noop()
	{
		Thread::yield();
	}

};

class VHWD_DLLIMPEXP SharedMemQueue
{
public:

	typedef LockFreeQueue<int32_t,ShmQueuePolicy<int32_t> > IShmQueue;

	SharedMemQueue();

	bool Create(const String& name,size_t qc=1024*16,size_t bc=1024*32,size_t bs=4096);

	void Close();

	bool Ok(){return m_bOk;}

	size_t Buffer_size(){return m_nBufferSize;}
	size_t Buffer_count(){return m_nBufferCount;}

	char* Buffer_address(int32_t v);

	bool Buffer_putq(int32_t v){return m_qb.putq(v);}
	int32_t Buffer_getq(){return m_qb.getq();}

	bool Queue1_putq(int32_t v){return m_q1.putq(v);}
	int32_t Queue1_getq(){return m_q1.getq();}

	bool Queue2_putq(int32_t v){return m_q2.putq(v);}
	int32_t Queue2_getq(){return m_q2.getq();}

private:

	IShmQueue m_q1;
	IShmQueue m_q2;
	IShmQueue m_qb;

	size_t m_nBufferSize;
	size_t m_nQueueCapacity;
	size_t m_nBufferCount;

	char* m_pBufferStart;
	bool m_bOk;

	SharedMem shm;

};



VHWD_LEAVE
#endif
