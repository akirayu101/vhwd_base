// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_THREAD_LOCKGUARD__
#define __H_VHWD_THREAD_LOCKGUARD__

#include "vhwd/config.h"

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


template<typename T,typename P=LockPolicyDefault<T> >
class LockGuard : public NonCopyable
{
public:

	inline LockGuard(T& mtx_):mtx(mtx_)
	{
		P::lock(mtx);
	}

	inline ~LockGuard()
	{
		P::unlock(mtx);
	}

	operator T&()
	{
		return mtx;
	}

private:
	T& mtx;
};


template<typename T>
class LockState : public NonCopyable
{
public:
	T oldvalue;
	T& value;

	LockState(T& v,const T n=T()):value(v)
	{
		oldvalue=value;
		value=n;
	}

	~LockState()
	{
		value=oldvalue;
	}

};

VHWD_LEAVE
#endif
