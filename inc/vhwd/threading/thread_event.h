// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_THREAD_EVENT__
#define __H_VHWD_THREAD_EVENT__

#include "vhwd/basic/atomic.h"
#include "vhwd/threading/thread_cond.h"
#include "vhwd/threading/thread_mutex.h"

#ifndef _MSC_VER
#include <pthread.h>
#else
#include <windows.h>
#endif

VHWD_ENTER

// Manual reset event, intitial state: not set
class VHWD_DLLIMPEXP Event
{
public:

	Event();
	Event(const Event&);
	Event& operator=(const Event&)
	{
		return *this;
	}

	~Event();

	void reset();
	void set();
	void wait();
	bool wait_for(int ms);
	bool wait_for(const TimeSpan& ts);
	bool wait_until(const TimePoint& tp);

protected:

#ifdef _WIN32
	void* hEvent;
#else
	Mutex m_tMutex;
	Condition m_tCond;
	AtomicInt32 m_nValue;
#endif

};


VHWD_LEAVE
#endif
