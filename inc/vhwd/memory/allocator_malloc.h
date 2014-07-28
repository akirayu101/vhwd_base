// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_MEMORY_ALLOCATOR_MALLOC__
#define __H_VHWD_MEMORY_ALLOCATOR_MALLOC__

#include "vhwd/config.h"
#include "vhwd/basic/exception.h"
#include <cstdlib>
#include <malloc.h>

#pragma push_macro("new")
#ifdef new
#undef new
#endif


VHWD_ENTER


//align must be 8 16 32 64 128 256
VHWD_DLLIMPEXP void* AlignedAlloc(size_t size,size_t align=16);
VHWD_DLLIMPEXP void* AlignedAlloc(size_t size,size_t align,size_t shift);
VHWD_DLLIMPEXP void AllignedFree(void* p);

template<int N>
class allocator_detail
{
public:

	static void* allocate(size_t n,size_t shift)
	{
		return AlignedAlloc(n,N,shift);
	}

	static void deallocate(void* p)
	{
		AllignedFree(p);
	}
};

template<>
class allocator_detail<0>
{
public:

	static void* allocate(size_t n,size_t=0)
	{
		return ::malloc(n);
	}

	static void deallocate(void* p)
	{
		::free(p);
	}
};


// shift D element align N
template <typename T, size_t N=0,size_t D=0>
class AllocatorM
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
	inline AllocatorM() throw() { }

	template <typename T2>
	inline AllocatorM(const AllocatorM<T2,N,D> &) throw() { }

	inline ~AllocatorM() throw() { }

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
		pointer p=(pointer)allocator_detail<N>::allocate(n*sizeof(T),D*sizeof(T));
		if(!p) Exception::XBadAlloc();
		return p;
	}

	inline void deallocate(pointer p, size_type)
	{
		allocator_detail<N>::deallocate(p);
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
		return (size_type(-1)-N) / sizeof(value_type);
	}

	template <typename T2>
	struct rebind
	{
		typedef AllocatorM<T2,N,D> other;
	};

	bool operator!=(const AllocatorM<T,N,D> &other) const
	{
		return !(*this == other);
	}

	// Returns true if and only if storage allocated from *this
	// can be deallocated from other, and vice versa.
	// Always returns true for stateless allocators.
	bool operator==(const AllocatorM &other) const
	{
		(void)&other;
		return true;
	}
};


VHWD_LEAVE

#pragma pop_macro("new")

#endif
