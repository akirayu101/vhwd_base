// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_THREAD_RWLOCK__
#define __H_VHWD_THREAD_RWLOCK__

#include "vhwd/config.h"

#ifndef _WIN32
#include <pthread.h>
#else
#include <windows.h>
#endif

VHWD_ENTER

class VHWD_DLLIMPEXP RWLock
{
public:
	RWLock();
	RWLock(const RWLock&);
	const RWLock& operator=(const RWLock&){return *this;}

	~RWLock();

	void lock_r();
	void lock_w();

	//bool trylock_r();
	//bool trylock_w();

	void unlock_r();
	void unlock_w();

protected:
#ifdef _WIN32
	SRWLOCK impl;
#else
	pthread_rwlock_t impl;
#endif
};



class LockPolicyRead
{
public:

	static inline void lock(RWLock& mtx)
	{
		mtx.lock_r();
	}

	static inline void unlock(RWLock& mtx)
	{
		mtx.unlock_r();
	}
};

class LockPolicyWrite
{
public:

	static inline void lock(RWLock& mtx)
	{
		mtx.lock_w();
	}

	static inline void unlock(RWLock& mtx)
	{
		mtx.unlock_w();
	}
};

VHWD_LEAVE
#endif
