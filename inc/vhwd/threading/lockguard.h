#ifndef __H_VHWD_THREAD_LOCKGUARD__
#define __H_VHWD_THREAD_LOCKGUARD__

#include "vhwd/config.h"
#include "vhwd/threading/thread_rwlock.h"
#include "vhwd/threading/thread.h"
#include "vhwd/basic/atomic.h"


VHWD_ENTER

template<typename T>
class LockPolicyDefault
{
public:

	static inline void Lock(T& mtx)
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

	static inline void Lock(AtomicIntT<T>& mtx)
	{
		while(mtx.exchange(1)!=0)
		{
			// spin;
		}
	}

	static inline void unlock(AtomicIntT<T>& mtx)
	{
		mtx.store(0);
	}
};


template<typename T>
class LockPolicyYield
{
public:

	static inline void lock(T& mtx)
	{
		while(mtx.exchange(1)!=0)
		{
			Thread::yield();
		}
	}

	static inline void unlock(T& mtx)
	{
		mtx.store(0);
	}
};


class LockPolicyRead
{
public:

	static inline void lock(RWLock& mtx)
	{
		mtx.lock_r();
	}

	static inline void unlock(RWLock& mtx)
	{
		mtx.unlock_r();
	}
};

class LockPolicyWrite
{
public:

	static inline void lock(RWLock& mtx)
	{
		mtx.lock_w();
	}

	static inline void unlock(RWLock& mtx)
	{
		mtx.unlock_w();
	}
};

template<typename T,typename P=LockPolicyDefault<T> >
class LockGuard : public NonCopyable
{
public:

	LockGuard(T& mtx_):mtx(mtx_)
	{
		P::Lock(mtx);
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
