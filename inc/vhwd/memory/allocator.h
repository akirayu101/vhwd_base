// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_MEMORY_ALLOCATOR__
#define __H_VHWD_MEMORY_ALLOCATOR__

#include "vhwd/memory/allocator_malloc.h"
#include "vhwd/memory/allocator_mempool.h"

VHWD_ENTER

#pragma push_macro("new")
#ifdef new
#undef new
#endif

// allocate Extra storage of E before returned address
template<typename A,typename E>
class AllocatorE : public A
{
public:

	typedef A basetype;
	typedef typename A::pointer pointer;
	typedef typename A::size_type size_type;
	typedef typename A::value_type value_type;

	AllocatorE() {}

	template<typename A2>
	AllocatorE(const A2& o):A(o) {}

	static const int shiftElem=(sizeof(E)+sizeof(value_type)-1)/(sizeof(value_type));

	inline pointer allocate(size_type n)
	{
		pointer p=basetype::allocate(n+shiftElem);
		new(p) E();
		return p+shiftElem;
	}

	static E& extra(pointer p)
	{
		if(!p)
		{
			static E g;
			return g;
		}
		return *(E*)(p-shiftElem);
	}

	inline void deallocate(pointer p, size_type n)
	{
		if(!p) return;
		basetype::deallocate(p-shiftElem,n);
	}

	inline size_type max_size() const throw()
	{
		return basetype::max_size()-shiftElem;
	}

	template <typename T2>
	struct rebind
	{
		typedef AllocatorE<typename A::template rebind<T2>::other,E> other;
	};

};


template<typename T,typename E,size_t N,size_t D>
class AllocatorE<AllocatorM<T,N,D>,E > : public AllocatorM<T,N,D+(sizeof(E)+sizeof(T)-1)/sizeof(T)>
{
public:

	typedef AllocatorM<T,N,D+(sizeof(E)+sizeof(T)-1)/sizeof(T)> basetype;
	typedef typename basetype::pointer pointer;
	typedef typename basetype::size_type size_type;
	typedef typename basetype::value_type value_type;

	static const int shiftElem=(sizeof(E)+sizeof(T)-1)/sizeof(T);


	inline pointer allocate(size_type n)
	{
		pointer p=(pointer)basetype::allocate(n+shiftElem);
		new(p) E();
		return p+shiftElem;
	}

	static E& extra(pointer p)
	{
		if(!p)
		{
			static E g;
			return g;
		}
		return *(E*)(p-shiftElem);
	}

	inline void deallocate(pointer p, size_type n)
	{
		if(!p) return;
		basetype::deallocate(p-shiftElem,n);
	}

	inline size_type max_size() const throw()
	{
		return basetype::max_size()-shiftElem;
	}

	template <typename T2>
	struct rebind
	{
		typedef AllocatorE<AllocatorM<T2,N,D>,E> other;
	};

};

template<typename A,size_t N>
class AllocatorN : public A
{
public:

	typedef A basetype;
	typedef typename A::pointer pointer;
	typedef typename A::size_type size_type;
	typedef typename A::value_type value_type;

	AllocatorN() {}

	template<typename A2>
	AllocatorN(const A2& o):A(o) {}

	inline pointer allocate(size_type n)
	{
		return basetype::allocate(n+N);
	}

	inline void deallocate(pointer p, size_type n)
	{
		if(!p) return;
		basetype::deallocate(p,n+N);
	}

	inline size_type max_size() const throw()
	{
		return basetype::max_size()-1;
	}

	template <typename T2>
	struct rebind
	{
		typedef AllocatorN<typename A::template rebind<T2>::other,N> other;
	};

};


#pragma pop_macro("new")

VHWD_LEAVE

#endif
