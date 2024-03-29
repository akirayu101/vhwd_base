// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_THREAD_MUTEX__
#define __H_VHWD_THREAD_MUTEX__

#include "vhwd/config.h"
#include "vhwd/basic/clock.h"

#ifndef _MSC_VER
#include <pthread.h>
#else
#include <windows.h>
#endif


VHWD_ENTER

class VHWD_DLLIMPEXP Mutex
{
public:

	Mutex();
	Mutex(const Mutex&);
	Mutex& operator=(const Mutex&)
	{
		return *this;
	}

	~Mutex();

	void lock();
	void unlock();
	bool try_lock();

protected:
#ifdef VHWD_WINDOWS
	CRITICAL_SECTION impl;
#else
	pthread_mutex_t impl;
#endif
};

VHWD_LEAVE
#endif
