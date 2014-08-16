// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_THREAD_MANAGER__
#define __H_VHWD_THREAD_MANAGER__

#include "vhwd/threading/thread_cond.h"
#include "vhwd/threading/thread_mutex.h"
#include "vhwd/basic/pointer.h"
#include "vhwd/basic/bitflags.h"


VHWD_ENTER

class VHWD_DLLIMPEXP ThreadImpl;

class VHWD_DLLIMPEXP ThreadManager : private NonCopyable
{
public:
	friend class ThreadImpl;
	friend class StaticObjectWithoutDeletorT<ThreadManager>;

	void wait();
	void close();
	int count();

	bool ok();

	static ThreadManager& current();

protected:


	ThreadManager();
	~ThreadManager();

	int m_nThreadMax;
	BitFlags m_nFlags;
	AtomicInt32 m_nThreadNum;
	AtomicInt32 m_nThreadJob;

	Mutex m_thrd_mutex;
	LitePtrT<ThreadImpl> m_pThreads_free;
	Condition m_cond_thrd_empty;
	Condition m_thrd_attached;

};

VHWD_LEAVE
#endif
