#ifndef __H_VHWD_THREAD_POOL__
#define __H_VHWD_THREAD_POOL__

#include "vhwd/threading/thread_cond.h"
#include "vhwd/threading/thread_mutex.h"
#include "vhwd/threading/thread.h"
#include "vhwd/basic/bitflags.h"


VHWD_ENTER

class ThreadImpl;

class VHWD_DLLIMPEXP ThreadPool : private NonCopyable
{
public:
	friend class ThreadImpl;

	ThreadPool();
	~ThreadPool();

	static ThreadPool& current();

	void wait();
	void close();
	int count();

	bool ok();

	enum
	{
		POOL_DISABLED	=1<<0,
		POOL_NOCACHE	=1<<1,
	};

	Mutex m_thrd_mutex;

protected:

	BitFlags m_nFlags;
	int m_nThreadNum;
	int m_nThreadJob;
	int m_nThreadMax;

	LitePtrT<ThreadImpl> m_pThreads_free;
	Condition m_cond_thrd_empty;
	Condition m_thrd_attached;

};

VHWD_LEAVE
#endif
