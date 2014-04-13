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


VHWD_LEAVE
#endif
