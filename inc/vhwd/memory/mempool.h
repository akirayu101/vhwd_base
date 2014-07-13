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

class VHWD_DLLIMPEXP MemPoolBase
{
public:
	static void set_break_alloc(unsigned n);
};


// Forward declaration
class VHWD_DLLIMPEXP FixedSizeAllocatorUnit;


// MemPoolPaging is a thread-safe and hight-efficient MemPool, it uses several fixed-size-allocator units internally.
// Memory allocated by MemPoolPaging can be deallocated by another MemPoolPaging.
// Memory allocated by malloc can be deallocated by MemPoolPaging.
class VHWD_DLLIMPEXP MemPoolPaging : public MemPoolBase, private NonCopyable
{
public:

	MemPoolPaging();
	~MemPoolPaging();

	// Allocate memory with given size, if the given size exceed the max fixed-size, mempoll will use malloc to allocate.
	void* allocate(size_t size);

	// Deallocate memory. if p is NOT allocated by this mempool, mempool will use free to deallocate.
	static void deallocate(void* p);

	// Same as above, drop the extra parameters.
	void* allocate(size_t size,const char*,int){return allocate(size);}
	static void deallocate(void* p,size_t){deallocate(p);}

	// Default MemPoolPaging object, usually we don't need to define MemPool objects.
	static MemPoolPaging& current();


protected:
	void _init();

	FixedSizeAllocatorUnit* m_aSlots;
	FixedSizeAllocatorUnit** m_pSlots;
	size_t m_nFixedSizeMax;
	size_t m_nFixedSizeCount;
	bool m_bCustom;
};


// MemPoolMalloc use malloc/free to allocate/deallocate memory.
class VHWD_DLLIMPEXP MemPoolMalloc : public MemPoolBase
{
public:

	static void* allocate(size_t size);

	static void* allocate(size_t size,const char*,int){return allocate(size);}
	static void deallocate(void* p){if(p) ::free(p);}
	static void deallocate(void* p,size_t){deallocate(p);}

	static MemPoolMalloc current(){ return MemPoolMalloc();}
};


// MemPoolDebug is used for detect memory leaks. compile with VHWD_MEMDEBUG macros to Enable it as global MemPool.
template<typename T>
class VHWD_DLLIMPEXP MemPoolDebug : public MemPoolBase
{
public:


	MemPoolDebug(){}

	static MemPoolDebug current(){return MemPoolDebug();}

	static void* allocate(size_t size)
	{
		return allocate(size,NULL,-1);
	}
	// Allocate memory and insert to the MemLinking.
	static void* allocate(size_t size,const char* file,int line);

	// Remove from MemLinking and deallocate memory.
	static void deallocate(void* p);

	static void deallocate(void* p,size_t){deallocate(p);}

};



#if defined(VHWD_MEMDEBUG)
typedef MemPoolDebug<MemPoolPaging> MemPool;
#else
typedef MemPoolPaging MemPool;
#endif


class def_allocator : public AllocatorP<int,MemPoolPaging>
{
public:
	def_allocator(){}
	def_allocator(const AllocatorP<int,MemPoolPaging>&){}
};

VHWD_LEAVE

#endif
