#ifndef __H_VHWD_THREAD_SEM__
#define __H_VHWD_THREAD_SEM__


#include "vhwd/config.h"

#ifndef _WIN32
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
	const Semaphore& operator=(const Semaphore&){return *this;}

	~Semaphore();

	void post();
	void post(int n);
	void wait();
	bool try_wait();

	bool wait_for(int ms);
	bool wait_for(const TimeSpan& ts);
	bool wait_until(const TimePoint& tp);

protected:
#ifdef _WIN32
	void* impl;
#else
	sem_t impl;
#endif

};



VHWD_LEAVE
#endif
