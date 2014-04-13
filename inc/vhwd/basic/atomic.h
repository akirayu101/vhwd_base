#ifndef __H_VHWD_BASIC_ATOMIC__
#define __H_VHWD_BASIC_ATOMIC__

#include "vhwd/config.h"


VHWD_ENTER

// AtomicIntT, T must be 32bit or 64bit signed/unsigned integer or pointer
template<typename T>
class VHWD_DLLIMPEXP AtomicIntT
{
public:

	typedef T type;

	AtomicIntT(){val=0;}
	AtomicIntT(T v):val(v){}


	// fetch_add performs value=value+v and return original value
	type fetch_add(type v);

	// fetch_sub performs value=value-v and return original value
	type fetch_sub(type v);

	// fetch_and performs value=value&v and return original value
	type fetch_and(type v);

	// fetch_or performs value=value|v and return original value
	type fetch_or(type v);

	// fetch_xor performs value=value^v and return original value
	type fetch_xor(type v);

	// exchange value and v and return original value
	type exchange(type v);

	// if value==expected then value=v return true else expected=value and return false.
	bool compare_exchange(type& expected,type v);

	T operator++();
	T operator++(int);
	T operator--();
	T operator--(int);

	T get() const {return val;}

	// load return value
	T load() const;

	// store performs value=v;
	void store(type v);

private:
	volatile T val;
};

typedef AtomicIntT<int32_t> AtomicInt32;
typedef AtomicIntT<int64_t> AtomicInt64;
typedef AtomicIntT<uint32_t> AtomicUint32;
typedef AtomicIntT<uint64_t> AtomicUint64;

template<typename T>
class VHWD_DLLIMPEXP AtomicIntT<T*>
{
public:
	typedef typename tl::meta_if<sizeof(T*)==4,AtomicInt32,AtomicInt64>::type impl_type;
	typedef typename impl_type::type diff_type;

	typedef T* type;

	type exchange(type v)
	{
		return (type)impl.exchange((diff_type)v);
	}

	bool compare_exchange(type& expected,type v)
	{
		return impl.compare_exchange(*(diff_type*)&expected,(diff_type)v);
	}

	type get(){return (type)impl.get();}
	type load(){return (type)impl.load();}
	void store(type v){impl.store((diff_type)v);}

protected:
	impl_type impl;
};


class VHWD_DLLIMPEXP AtomicSpin
{
public:

	static const int32_t nSpinCount=1024*1024*32;

	void lock()
	{
		while(val.exchange(1)!=0)
		{
			noop();
		}
	}

	static void noop();

	bool try_lock()
	{
		return val.exchange(1)==0;
	}

	void unlock()
	{
		val.store(0);
	}

	AtomicInt32 val;
};


class VHWD_DLLIMPEXP RefCounter
{
public:

	AtomicInt32 m_nUseCount;
	AtomicInt32 m_nWeakCount;

	RefCounter():m_nUseCount(0),m_nWeakCount(0){}

	void IncUseCount()
	{
		++m_nUseCount;
	}

	bool DecUseCount()
	{
		if(--m_nUseCount!=0)
		{
			return false;
		}
		if(m_nWeakCount.load()==0)
		{
			delete this;
		}
		return true;
	}

	void IncWeakCount()
	{
		++m_nWeakCount;
	}

	void DecWeakCount()
	{
		if(--m_nWeakCount==0 && m_nUseCount.load()==0)
		{
			delete this;
		}
	}
};

VHWD_LEAVE
#endif
