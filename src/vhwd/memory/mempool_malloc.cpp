#include "vhwd/memory/mempool.h"
#include "vhwd/basic/system.h"

VHWD_ENTER

void* MemPoolMalloc::allocate(size_t size)
{
	void* pMem=::malloc(size);
	if(pMem==NULL)
	{
		System::LogTrace("malloc failed: size=%d",(int)size);
		Exception::XBadAlloc();
	}
	return pMem;
}

VHWD_LEAVE
