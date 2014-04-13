#ifndef __H_VHWD_MEMORY_ALLOCATOR_MEMPOOL__
#define __H_VHWD_MEMORY_ALLOCATOR_MEMPOOL__

#include "vhwd/config.h"
#include <cstdlib>
#include <malloc.h>

#pragma push_macro("new")
#ifdef new
#undef new
#endif


VHWD_ENTER


template <typename T,typename P>
class AllocatorUsePool
{
public:
	typedef T value_type;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;

	typedef T *pointer;
	typedef const T *const_pointer;

	typedef T &reference;
	typedef const T &const_reference;

public:
	inline AllocatorUsePool() throw() { }

	template <typename T2>
	inline AllocatorUsePool(const AllocatorUsePool<T,P> &) throw() {}

	inline ~AllocatorUsePool() throw() { }

	inline pointer address(reference r)
	{
		return &r;
	}

	inline const_pointer address(const_reference r) const
	{
		return &r;
	}

	inline pointer allocate(size_type n)
	{
		return (T*)P::current().allocate(n*sizeof(T));
	}

	inline void deallocate(pointer p, size_type)
	{
		P::current().deallocate(p);
	}


	inline void construct(pointer p, const value_type &o)
	{
		new(p) value_type(o);
	}

	inline void destroy(pointer p)
	{
		(void)&p;
		p->~value_type();
	}

	inline size_type max_size() const throw()
	{
		return size_type(-1) / sizeof(value_type);
	}

	template <typename T2>
	struct rebind
	{
		typedef AllocatorUsePool<T2,P> other;
	};

	bool operator!=(const AllocatorUsePool<T,P> &other) const
	{
		return !(*this == other);
	}

	// Returns true if and only if storage allocated from *this
	// can be deallocated from other, and vice versa.
	// Always returns true for stateless allocators.
	bool operator==(const AllocatorUsePool &other) const
	{
		(void)&other;
		return true;
	}
};

VHWD_LEAVE

#pragma pop_macro("new")

#endif
