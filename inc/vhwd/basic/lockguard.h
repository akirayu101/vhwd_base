// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_THREAD_LOCKGUARD__
#define __H_VHWD_THREAD_LOCKGUARD__

#include "vhwd/config.h"
#include "vhwd/basic/atomic.h"


VHWD_ENTER

template<typename T>
class LockPolicyDefault
{
public:

	static inline void lock(T& mtx)
	{
		mtx.lock();
	}

	static inline void unlock(T& mtx)
	{
		mtx.unlock();
	}
};

template<typename T>
class LockPolicyDefault<AtomicIntT<T> >
{
public:

	static inline void lock(AtomicIntT<T>& mtx)
	{
		while(mtx.exchange(1)!=0)
		{
			// spin;
		}
	}

	static inline void unlock(AtomicIntT<T>& mtx)
	{
		wassert(mtx.get()==1);
		mtx.store(0);
	}
};





template<typename T,typename P=LockPolicyDefault<T> >
class LockGuard : public NonCopyable
{
public:

	LockGuard(T& mtx_):mtx(mtx_)
	{
		P::lock(mtx);
	}

	~LockGuard()
	{
		P::unlock(mtx);
	}

	operator T&(){return mtx;}

private:
	T& mtx;
};


VHWD_LEAVE
#endif
