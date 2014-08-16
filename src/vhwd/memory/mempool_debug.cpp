#include "mempool_link.h"
#include "vhwd/basic/lockguard.h"
#include "vhwd/basic/system.h"


VHWD_ENTER

// Allocate memory and insert to the MemLinking.
template<typename T>
void* MemPoolDebug<T>::allocate(size_t size,const char* file,int line)
{
	void* _pMem=T::current().allocate(size+MemLinking::const_fpi_size);
	return MemLinking::current().insert_link(_pMem,file,line,size);
}

// Remove from MemLinking and deallocate memory.
template<typename T>
void MemPoolDebug<T>::deallocate(void* p)
{
	if(!p) return;
	p=MemLinking::current().remove_link(p);
	T::current().deallocate(p);
}


template class MemPoolDebug<MemPoolMalloc>;
template class MemPoolDebug<MemPoolPaging>;
template class MemPoolDebug<MemPoolCached>;

VHWD_LEAVE
