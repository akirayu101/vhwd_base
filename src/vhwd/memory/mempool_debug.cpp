#include "mempool_impl.h"
#include "vhwd/threading/lockguard.h"
#include "vhwd/threading/thread_spin.h"
#include "vhwd/basic/system.h"


VHWD_ENTER

template<typename T>
MemPoolDebug<T>& MemPoolDebug<T>::current()
{
	static StaticObjectWithoutDeletorT<MemPoolDebug> gInstance;
	return gInstance;
}

// Allocate memory and insert to the MemLinking.
template<typename T>
void* MemPoolDebug<T>::allocate(size_t size,const char* file,long line)
{
	void* _pMem=basetype::allocate(size+MemLinking::const_fpi_size);
	return MemLinking::current().insert_link(_pMem,file,line,size);
}

// Remove from MemLinking and deallocate memory.
template<typename T>
void MemPoolDebug<T>::deallocate(void* p)
{
	if(!p) return;
	p=MemLinking::current().remove_link(p);
	basetype::deallocate(p);
}


template class MemPoolDebug<MemPoolMalloc>;
template class MemPoolDebug<MemPoolPaging>;

VHWD_LEAVE
