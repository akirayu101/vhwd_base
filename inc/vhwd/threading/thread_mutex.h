#ifndef __H_VHWD_THREAD_MUTEX__
#define __H_VHWD_THREAD_MUTEX__

#include "vhwd/config.h"
#include "vhwd/basic/clock.h"

#ifndef _WIN32
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
	const Mutex& operator=(const Mutex&){return *this;}

	~Mutex();

	void lock();
	void unlock();
	bool try_lock();

protected:
#ifdef _WIN32
	CRITICAL_SECTION impl;
#else
	pthread_mutex_t impl;
#endif
};

VHWD_LEAVE
#endif
