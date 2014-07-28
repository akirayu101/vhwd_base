// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_THREAD_SEM__
#define __H_VHWD_THREAD_SEM__


#include "vhwd/config.h"

#ifndef _MSC_VER
#include <pthread.h>
#include "semaphore.h"
#else
#include <windows.h>
#endif


VHWD_ENTER

class VHWD_DLLIMPEXP Semaphore
{
public:
	Semaphore();
	Semaphore(const Semaphore&);
	Semaphore& operator=(const Semaphore&)
	{
		return *this;
	}

	~Semaphore();

	void post();
	void post(int n);
	void wait();
	bool try_wait();

	bool wait_for(int ms);
	bool wait_for(const TimeSpan& ts);
	bool wait_until(const TimePoint& tp);

protected:
#ifdef _MSC_VER
	void* impl;
#else
	sem_t impl;
#endif

};



VHWD_LEAVE
#endif
