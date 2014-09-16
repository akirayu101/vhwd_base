#include "mempool_impl.h"


VHWD_ENTER

void MpAllocCachedNoLock::init()
{
	for(size_t i=0;i<MpAllocGlobal::sl_size;i++)
	{
		aLinks[i].nd_head=NULL;
		aLinks[i].nd_nnum=0;
		aLinks[i].nd_nmax=g_myalloc_impl->aSlots[i].nd_nall*2;
		if(aLinks[i].nd_nmax<4)
		{
			aLinks[i].nd_nmax=4;
		}
	}
}

void MpAllocCachedNoLock::cleanup()
{
	for(size_t i=0;i<MpAllocGlobal::sl_size;i++)
	{
		MpAllocSlot& sl(g_myalloc_impl->aSlots[i]);
		sl.sl_spin.lock();
		aLinks[i].nd_head=g_myalloc_impl->dealloc_batch_nolock(sl,aLinks[i].nd_head,aLinks[i].nd_nnum);

		wassert(!aLinks[i].nd_head);
		aLinks[i].nd_nnum=0;

		sl.sl_spin.unlock();
	}
}

void MpAllocCachedNoLock::gc()
{
	for(size_t i=0;i<MpAllocGlobal::sl_size;i++)
	{
		MpAllocSlot& sl(g_myalloc_impl->aSlots[i]);
		if(!sl.sl_spin.try_lock()) continue;
		aLinks[i].nd_head=g_myalloc_impl->dealloc_batch_nolock(sl,aLinks[i].nd_head,aLinks[i].nd_nnum);

		wassert(!aLinks[i].nd_head);
		aLinks[i].nd_nnum=0;

		sl.sl_spin.unlock();
	}
}

void* MpAllocCachedNoLock::alloc(size_t n)
{
	MpAllocSlot* psl=g_myalloc_impl->get_slot(n);
	if(!psl)
	{
		return g_myalloc_impl->alloc_large(n);
	}

	size_t k=psl-g_myalloc_impl->aSlots;
	MpAllocNode* p=aLinks[k].nd_head;
	if(!p)
	{
		wassert(aLinks[k].nd_nnum==0);
		p=g_myalloc_impl->alloc_small_batch(g_myalloc_impl->aSlots[k],aLinks[k].nd_nnum);
		if(!p) return NULL;
	}
	aLinks[k].nd_nnum--;
	aLinks[k].nd_head=p->nd_next;
	return p;
}

void MpAllocCachedNoLock::dealloc(void* p)
{
	MpAllocBucket* pbk=g_myalloc_impl->pgmap.find_bucket(p);
	if(!pbk)
	{
		::free(p);
		return;
	}

	MpAllocSlot* psl=pbk->get()->sl_slot;
	if(!psl)
	{
		g_myalloc_impl->dealloc_real(p,*pbk);
		return;
	}

	MpAllocNode* n=(MpAllocNode*)p;
	intptr_t k=psl-g_myalloc_impl->aSlots;
	n->nd_next=aLinks[k].nd_head;
	aLinks[k].nd_head=n;
	if(aLinks[k].nd_nnum++>=aLinks[k].nd_nmax)
	{
		size_t sz=aLinks[k].nd_nnum/2;
		aLinks[k].nd_head=g_myalloc_impl->dealloc_batch_nolock(*psl,aLinks[k].nd_head,sz);
		wassert(aLinks[k].nd_head!=NULL);
		aLinks[k].nd_nnum-=sz;
	}
}


void tc_init()
{
	MpAllocCachedNoLock* tc_data=tc_get();
	if(tc_data)
	{
		System::LogTrace("ThreadCachedData already inited");
		return;
	}

	tc_data=(MpAllocCachedNoLock*)mp_alloc_real(sizeof(MpAllocCachedNoLock));
	if(tc_data)
	{
		tc_data->init();
		tc_set(tc_data);
	}

}

void tc_gc()
{
	MpAllocCachedNoLock* tc_data=tc_get();
	if(tc_data)
	{
		tc_data->gc();
	}
}

void tc_cleanup()
{
	MpAllocCachedNoLock* tc_data=tc_get();
	if(tc_data)
	{
		tc_data->cleanup();
		tc_set(NULL);
		mp_free_real(tc_data);
	}

}

VHWD_LEAVE
