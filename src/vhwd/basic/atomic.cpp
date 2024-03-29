
#include "vhwd/basic/atomic.h"
#include "vhwd/basic/lockguard.h"
#include <cstdlib>

#ifdef VHWD_WINDOWS
#include <windows.h>
#else
#include <sched.h>
#include <pthread.h>
#endif

#ifdef _MSC_VER
#pragma warning(disable:4146)
#include "intrin.h"
#endif

VHWD_ENTER


#ifdef _MSC_VER

template<typename T,int N>
class AtomicImpl;

template<typename T>
class AtomicImpl<T,4>
{
public:
	typedef T type;
	typedef volatile long * pint;
	static inline type fetch_add(volatile type* p,type v)
	{
		return InterlockedExchangeAdd((pint)p,v);
	}

	static inline type fetch_sub(volatile type* p,type v)
	{
		return InterlockedExchangeAdd((pint)p,-v);
	}

	static inline type fetch_and(volatile type* p,type v)
	{
		return _InterlockedAnd((pint)p,v);
	}

	static inline type fetch_or(volatile type* p,type v)
	{
		return _InterlockedOr((pint)p,v);
	}

	static inline type fetch_xor(volatile type* p,type v)
	{
		return _InterlockedXor((pint)p,v);
	}

	static inline type exchange(volatile type* p,type v)
	{
		return InterlockedExchange((pint)p,v);
	}

	static type store(volatile type* p,type v)
	{
		return InterlockedExchange((pint)p,v);
	}

	static inline bool compare_exchange(volatile type* p,type& c,type v)
	{
		type t=InterlockedCompareExchange((pint)p,v,c);
		if(t!=c)
		{
			c=t;
			return false;
		}
		else
		{
			return true;
		}
	}

	static inline type load(volatile const type* p)
	{
		return *p;
	}

};

#ifndef VHWD_NO64BIT_ATOMIC

template<typename T>
class AtomicImpl<T,8>
{
public:
	typedef T type;
	typedef volatile int64_t* pint;

	static inline type fetch_add(volatile type* p,type v)
	{
		return InterlockedExchangeAdd64((pint)p,v);
	}

	static inline type fetch_sub(volatile type* p,type v)
	{
		return InterlockedExchangeAdd64((pint)p,-v);
	}

	static inline type fetch_and(volatile type* p,type v)
	{
		return InterlockedAnd64((pint)p,v);
	}

	static inline type fetch_or(volatile type* p,type v)
	{
		return InterlockedOr64((pint)p,v);
	}

	static inline type fetch_xor(volatile type* p,type v)
	{
		return InterlockedXor64((pint)p,v);
	}

	static inline type exchange(volatile type* p,type v)
	{
		return InterlockedExchange64((pint)p,v);
	}

	static inline type store(volatile type* p,type v)
	{
		return InterlockedExchange64((pint)p,v);
	}

	static inline bool compare_exchange(volatile type* p,type& c,type v)
	{
		type t=InterlockedCompareExchange64((pint)p,v,c);
		if(t!=c)
		{
			c=t;
			return false;
		}
		else
		{
			return true;
		}
	}

	static inline type load(volatile const type* p)
	{
		return InterlockedOr64((pint)p,0);
	}
};

#endif

template<typename T>
T AtomicIntT<T>::fetch_add(T v)
{
	return AtomicImpl<T,sizeof(T)>::fetch_add(&val,v);
}

template<typename T>
T AtomicIntT<T>::fetch_sub(T v)
{
	return AtomicImpl<type,sizeof(T)>::fetch_sub(&val,v);
}



template<typename T>
T AtomicIntT<T>::fetch_and(type v)
{
	return AtomicImpl<type,sizeof(T)>::fetch_and(&val,v);
}

template<typename T>
T AtomicIntT<T>::fetch_or(type v)
{
	return AtomicImpl<type,sizeof(T)>::fetch_or(&val,v);
}

template<typename T>
T AtomicIntT<T>::fetch_xor(type v)
{
	return AtomicImpl<type,sizeof(T)>::fetch_xor(&val,v);
}

template<typename T>
T AtomicIntT<T>::exchange(T v)
{
	return AtomicImpl<T,sizeof(T)>::exchange(&val,v);
}

template<typename T>
T AtomicIntT<T>::operator++()
{
	return AtomicImpl<T,sizeof(T)>::fetch_add(&val,1)+1;
}

template<typename T>
T AtomicIntT<T>::operator++ (int)
{
	return AtomicImpl<T,sizeof(T)>::fetch_add(&val,1);
}

template<typename T>
T AtomicIntT<T>::operator--()
{
	return AtomicImpl<T,sizeof(T)>::fetch_sub(&val,1)-1;
}

template<typename T>
T AtomicIntT<T>::operator-- (int)
{
	return AtomicImpl<T,sizeof(T)>::fetch_sub(&val,1);
}

template<typename T>
void  AtomicIntT<T>::store(T v)
{
	exchange(v);
}

template<typename T>
T AtomicIntT<T>::load() const
{
	return AtomicImpl<T,sizeof(T)>::load(&val);
}


template<typename T>
bool AtomicIntT<T>::compare_exchange(T& expected,T v)
{
	return AtomicImpl<T,sizeof(T)>::compare_exchange(&val,expected,v);
}

#else

template<typename T>
bool AtomicIntT<T>::compare_exchange(T& expected,T v)
{

	T old=__sync_val_compare_and_swap(&val,expected,v);
	if(old==expected) return true;
	expected=old;
	return false;
}

template<typename T>
T AtomicIntT<T>::fetch_add(type v)
{
	return __sync_fetch_and_add(&val,v);
}

template<typename T>
T AtomicIntT<T>::fetch_sub(type v)
{
	return __sync_fetch_and_sub(&val,v);
}

template<typename T>
T AtomicIntT<T>::exchange(type v)
{
	return __sync_lock_test_and_set(&val,v);
}

template<typename T>
T AtomicIntT<T>::operator++()
{
	return __sync_add_and_fetch (&val,1);
}

template<typename T>
T AtomicIntT<T>::operator++ (int)
{
	return __sync_fetch_and_add(&val,1);
}

template<typename T>
T AtomicIntT<T>::operator--()
{
	return __sync_sub_and_fetch(&val,1);
}

template<typename T>
T AtomicIntT<T>::operator-- (int)
{
	return __sync_fetch_and_sub(&val,1);
}

template<typename T>
void  AtomicIntT<T>::store(T v)
{
	exchange(v);
}

template<typename T>
T AtomicIntT<T>::fetch_and(type v)
{
	return __sync_fetch_and_and(&val,v);
}

template<typename T>
T AtomicIntT<T>::fetch_or(type v)
{
	return __sync_fetch_and_or(&val,v);
}

template<typename T>
T AtomicIntT<T>::fetch_xor(type v)
{
	return __sync_fetch_and_xor(&val,v);
}

template<typename T>
T AtomicIntT<T>::load() const
{
	return val;
}

#endif

void AtomicSpin::noop()
{
#ifdef VHWD_WINDOWS
	_sleep(0);
#else
	sched_yield();
#endif
}


uintptr_t AtomicMutex::thread_id()
{
#ifdef VHWD_WINDOWS
	return ::GetCurrentThreadId();
#else
	return (uintptr_t)pthread_self();
#endif
}

template class AtomicIntT<int32_t>;
template class AtomicIntT<uint32_t>;

#ifndef VHWD_NO64BIT_ATOMIC
template class AtomicIntT<int64_t>;
template class AtomicIntT<uint64_t>;
#endif

VHWD_LEAVE
