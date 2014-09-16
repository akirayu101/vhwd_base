// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_MEMORY_MEMPOOL__
#define __H_VHWD_MEMORY_MEMPOOL__

#include "vhwd/basic/exception.h"
#include "vhwd/memory/allocator.h"

VHWD_ENTER

VHWD_DLLIMPEXP void* page_alloc(size_t n);
VHWD_DLLIMPEXP void page_free(void* p,size_t n);
VHWD_DLLIMPEXP int page_protect(void* p,size_t n,int f);

VHWD_DLLIMPEXP void* mp_alloc(size_t n);
VHWD_DLLIMPEXP void* mp_realloc(void* p,size_t n);
VHWD_DLLIMPEXP void mp_free(void* p);


class VHWD_DLLIMPEXP MemPoolBase
{
public:
	static void set_break_alloc(unsigned n);	
};



// MemPoolPaging is a thread-safe and hight-efficient MemPool, it uses several fixed-size-allocator units internally.
class VHWD_DLLIMPEXP MemPoolPaging : public MemPoolBase
{
public:

	// Allocate memory with given size, if the given size exceed the max fixed-size, mempoll will use malloc to allocate.
	static void* allocate(size_t size);

	// Deallocate memory. if p is NOT allocated by this mempool, mempool will use free to deallocate.
	static void deallocate(void* p);

	// Same as above, drop the extra parameters.
	static void* allocate(size_t size,const char*,int)
	{
		return allocate(size);
	}

	static void deallocate(void* p,size_t)
	{
		deallocate(p);
	}

	static MemPoolPaging& current()
	{
		static MemPoolPaging pool;
		return pool;
	}

};


// MemPoolMalloc use malloc/free to allocate/deallocate memory.
class VHWD_DLLIMPEXP MemPoolMalloc : public MemPoolBase
{
public:

	static void* allocate(size_t size);

	static void* allocate(size_t size,const char*,int)
	{
		return allocate(size);
	}

	static void deallocate(void* p);

	static void deallocate(void* p,size_t)
	{
		deallocate(p);
	}

	static MemPoolMalloc& current()
	{
		static MemPoolMalloc pool;
		return pool;
	}
};


// MemPoolDebug is used for detect memory leaks. compile with VHWD_MEMDEBUG macros to Enable it as global MemPool.
template<typename T>
class VHWD_DLLIMPEXP MemPoolDebug : public MemPoolBase
{
public:

	static void* allocate(size_t size)
	{
		return allocate(size,NULL,-1);
	}

	// Allocate memory and insert to the MemLinking.
	static void* allocate(size_t size,const char* file,int line);

	// Remove from MemLinking and deallocate memory.
	static void deallocate(void* p);

	static void deallocate(void* p,size_t)
	{
		deallocate(p);
	}

	static MemPoolDebug& current()
	{
		static MemPoolDebug pool;
		return pool;
	}
};



#if defined(VHWD_MEMDEBUG)
typedef MemPoolDebug<MemPoolPaging> MemPool;
#else
typedef MemPoolPaging MemPool;
#endif


class def_allocator : public AllocatorP<int,MemPoolPaging>
{
public:
	def_allocator() {}
	def_allocator(const AllocatorP<int,MemPoolPaging>&) {}
};

VHWD_LEAVE

#endif
