// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_THREAD_COND__
#define __H_VHWD_THREAD_COND__

#include "vhwd/config.h"

#ifndef _WIN32
#include <pthread.h>
#else
#include <windows.h>
#endif


VHWD_ENTER

class VHWD_DLLIMPEXP Mutex;
class VHWD_DLLIMPEXP Condition
{
public:
	Condition();
	Condition(const Condition& o);
	const Condition& operator=(const Condition&){return *this;}

	~Condition();

	void notify_one();
	void notify_all();

	void wait(Mutex& Mutex);
	bool wait_for(Mutex& Mutex,int ms);
	bool wait_for(Mutex& Mutex,const TimeSpan& ts);
	bool wait_until(Mutex& Mutex,const TimePoint& tp);

protected:
#ifdef _WIN32
	CONDITION_VARIABLE impl;
#else
	pthread_cond_t impl;
#endif
};


VHWD_LEAVE
#endif
