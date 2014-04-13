#include "mempool_impl.h"



VHWD_ENTER
	
MemPageInfo* MemPageInfo::CreatePage(FixedSizeAllocatorUnit& sl)
{
	MemPageInfo* mi=(MemPageInfo*)heap_alloc(sl.nPageSize);

	if(mi==0)
	{
		Exception::XBadAlloc();
	}

	MemPageNode* hPrev=(MemPageNode*)(((char*)mi)+sl.nElemShift);

	mi->m_pSlot=&sl;
	mi->nUsedCount=0;
	mi->pNext=NULL;
	mi->pPrev=NULL;
	mi->pFreeList=hPrev;

	mi->pSibling=NULL;

	for(size_t i=1;i<sl.nElemCount;i++)
	{
		MemPageNode* pTemp=(MemPageNode*)(((char*)hPrev)+sl.nElemSize);
		hPrev->pNext=pTemp;
		hPrev=pTemp;
	}
	hPrev->pNext=NULL;
	return mi;

}

void MemPageInfo::DestroyPage(MemPageInfo* p)
{
	heap_free(p,p->m_pSlot->nPageSize);
}

VHWD_LEAVE


