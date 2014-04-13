#ifndef __H_VHWD_THREAD_SPIN__
#define __H_VHWD_THREAD_SPIN__

#include "vhwd/config.h"

#ifndef _WIN32
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
	const SpinLock& operator=(const SpinLock&){return *this;}

	~SpinLock();

	void lock();
	void unlock();
	bool try_lock();

protected:
#ifdef _WIN32
	CRITICAL_SECTION impl;
#else
	pthread_spinlock_t impl;
#endif
};


VHWD_LEAVE
#endif
