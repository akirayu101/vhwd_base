// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_THREAD_SPIN__
#define __H_VHWD_THREAD_SPIN__

#include "vhwd/config.h"

#ifndef _MSC_VER
#include <pthread.h>
#else
#include <windows.h>
#endif

VHWD_ENTER

class VHWD_DLLIMPEXP SpinLock
{
public:

	SpinLock();
	SpinLock(const SpinLock&);
	SpinLock& operator=(const SpinLock&)
	{
		return *this;
	}

	~SpinLock();

	void lock();
	void unlock();
	bool try_lock();

protected:
#ifdef _MSC_VER
	CRITICAL_SECTION impl;
#else
	pthread_spinlock_t impl;
#endif
};


VHWD_LEAVE
#endif
