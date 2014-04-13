#include "mempool_impl.h"



VHWD_ENTER

FixedSizeAllocatorUnit::FixedSizeAllocatorUnit(size_t n)
{
	nElemSize=n>0?(n+7)&~7:8;
	pPageList=NULL;
	if(nElemSize==0) return;

	//nPageSpan=1;

	nPageSize=MemPageCache::nPageSize;

	nElemCount=(nPageSize-sizeof(MemPageInfo))/nElemSize;
	nElemShift=nPageSize-nElemSize*nElemCount;
}


VHWD_LEAVE


