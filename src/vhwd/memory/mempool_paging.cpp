#include "mempool_impl.h"
#include "vhwd/basic/lockguard.h"
#include "vhwd/threading/thread_spin.h"
#include "vhwd/basic/system.h"


#ifdef _WIN32
#include <windows.h>
#else
#include <sys/mman.h>
#include <cstring>
#include <cerrno>
#endif

#ifdef new
#undef new
#endif


VHWD_ENTER

#ifdef _WIN32


void* heap_alloc(size_t nSize_)
{
	void* pMem_ = ::VirtualAlloc(NULL, nSize_, MEM_COMMIT, PAGE_READWRITE );
	if(pMem_)
	{
		return pMem_;
	}

	int ret=::GetLastError();
	System::LogTrace("VirtualAlloc failed: ret=%d",ret);
	return NULL;
}

void heap_free(void* pMem_,size_t)
{
	if(::VirtualFree(pMem_, 0, MEM_RELEASE)==0)
	{
		System::LogTrace("VirtualFree failed: ptr=%p",pMem_);
	}
}

void heap_protect(void* pMem_,size_t nSize_,bool f)
{
	DWORD old;
	VirtualProtect(pMem_,nSize_,f?PAGE_READONLY:PAGE_READWRITE,&old);
}

#else

void* heap_alloc(size_t nSize)
{

	void* p=mmap(NULL,nSize,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);

	if(p!=MAP_FAILED)
	{
		return p;
	}

	int ret=errno;
	System::LogTrace("mmap failed: %s",strerror(ret));
	return NULL;
}

void heap_protect(void* pMem_,size_t nSize_,bool f)
{
	mprotect (pMem_, nSize_, f?PROT_READ:(PROT_READ|PROT_WRITE));
}

void heap_free(void* p,size_t s)
{
	munmap(p,s);
}
#endif



void MemPoolPaging::_init()
{
	if(m_nFixedSizeMax!=0) return;

	MemPageCache& mpc(MemPageCache::current());
	mpc._init();

	m_bCustom=false;
	m_nFixedSizeCount=mpc.m_nFixedSizeCount;
	m_nFixedSizeMax=mpc.m_nFixedSizeMax;
	m_pSlots=mpc.m_pSlots;
	m_aSlots=mpc.m_aSlots;
}

MemPoolPaging::MemPoolPaging()
{
	m_nFixedSizeMax=0;
	_init();
}

MemPoolPaging::~MemPoolPaging()
{
	if(!m_bCustom) return;

	for(size_t i=0;i<m_nFixedSizeCount;i++)
	{
		if(m_aSlots[i].pPageList!=NULL)
		{
			System::LogTrace("MemPool not empty in destructor");
		}
		else
		{
			m_aSlots[i].~FixedSizeAllocatorUnit();
		}
	}
	::free(m_pSlots);
	::free(m_aSlots);
}



void* MemPoolPaging::allocate(size_t nSize)
{

	if(nSize>m_nFixedSizeMax)
	{
		if(m_nFixedSizeMax==0)
		{
			_init();
			if(nSize>m_nFixedSizeMax)
			{
				return ::malloc(nSize);
			}
		}
		else
		{
			return ::malloc(nSize);
		}
	}

	FixedSizeAllocatorUnit& sl(*m_pSlots[nSize]);
	FixedSizeAllocatorUnit::LockType lock1(sl.tSpin);

	if(!sl.pPageList)
	{
		MemPageInfo* mi=MemPageInfo::CreatePage(sl);
		MemPageCache::current().append(mi);
		sl.pPageList=mi;
	}

	MemPageInfo* mi=sl.pPageList;
	void* p=mi->pFreeList;
	mi->pFreeList=mi->pFreeList->pNext;
	mi->nUsedCount++;

	if(!mi->pFreeList)
	{
		sl.pPageList=sl.pPageList->pNext;
		if(sl.pPageList)
		{
			sl.pPageList->pPrev=NULL;
		}
	}
	return p;
}

void MemPoolPaging::deallocate(void* p)
{


	if(!p) return;


	static MemPageCache& mpc(MemPageCache::current());

	MemPageInfo* mi=mpc.search(p);
	if(!mi)
	{
		::free(p);
		return;
	}

	{
		FixedSizeAllocatorUnit& sl(*mi->m_pSlot);
		MemPageNode* pn=(MemPageNode*)p;
		FixedSizeAllocatorUnit::LockType lock1(sl.tSpin);

		if(!mi->pFreeList)
		{
			if(sl.pPageList)
			{
				sl.pPageList->pPrev=mi;
			}
			wassert(mi->pPrev==NULL);
			mi->pNext=sl.pPageList;
			sl.pPageList=mi;
		}

		pn->pNext=mi->pFreeList;
		mi->pFreeList=pn;

		if(--mi->nUsedCount!=0||!sl.pPageList->pNext)
		{
			return;
		}

		if(mi->pPrev)
		{
			mi->pPrev->pNext=mi->pNext;
		}
		else
		{
			sl.pPageList=mi->pNext;
		}

		if(mi->pNext)
		{
			mi->pNext->pPrev=mi->pPrev;
		}

	}

	mpc.remove(mi);
	MemPageInfo::DestroyPage(mi);

}


char gInstance_mempoolpaging[sizeof(MemPoolPaging)];
MemPoolPaging& MemPoolPaging::current()
{
	return *(MemPoolPaging*)gInstance_mempoolpaging;
}


void* SmallObject::operator new[](size_t nSize)
{
	return vhwd::MemPoolPaging::current().allocate(nSize,NULL,-1);
}

void* SmallObject::operator new(size_t nSize,const char* sFile,int nLine)
{
	return vhwd::MemPoolPaging::current().allocate(nSize,sFile,nLine);
}

void* SmallObject::operator new[](size_t nSize,const char* sFile,int nLine)
{
	return vhwd::MemPoolPaging::current().allocate(nSize,sFile,nLine);
}

void SmallObject::operator delete(void* p,const char*,int)
{
	vhwd::MemPoolPaging::current().deallocate(p);
}
void SmallObject::operator delete[](void* p,const char*,int)
{
	vhwd::MemPoolPaging::current().deallocate(p);
}
void* SmallObject::operator new(size_t nSize)
{
	return vhwd::MemPoolPaging::current().allocate(nSize,NULL,-1);
}

void SmallObject::operator delete(void* p)
{
	vhwd::MemPoolPaging::current().deallocate(p);
}

void SmallObject::operator delete[](void* p)
{
	vhwd::MemPoolPaging::current().deallocate(p);
}

VHWD_LEAVE

#if defined(VHWD_MEMDEBUG) || defined(VHWD_MEMUSEPOOL)



void* operator new(size_t nSize)
{
	return vhwd::MemPool::current().allocate(nSize);
}

void operator delete(void* p)
{
	vhwd::MemPool::current().deallocate(p);
}

void* operator new(size_t nSize,const char* sFile,int nLine)
{
	return vhwd::MemPool::current().allocate(nSize,sFile,nLine);
}

void operator delete(void* p,const char*,int)
{
	vhwd::MemPool::current().deallocate(p);
}

void* operator new(size_t nSize,int,const char* sFile,int nLine)
{
	return vhwd::MemPool::current().allocate(nSize,sFile,nLine);
}

void operator delete(void* p,int,const char*,int)
{
	vhwd::MemPool::current().deallocate(p);
}

void* operator new[](size_t nSize)
{
	return vhwd::MemPool::current().allocate(nSize);
}

void operator delete[](void* p)
{
	vhwd::MemPool::current().deallocate(p);
}

void* operator new[](size_t nSize,const char* sFile,int nLine)
{
	return vhwd::MemPool::current().allocate(nSize,sFile,nLine);
}

void operator delete[](void* p,const char*,int)
{
	vhwd::MemPool::current().deallocate(p);
}

void* operator new[](size_t nSize,int,const char* sFile,int nLine)
{
	return vhwd::MemPool::current().allocate(nSize,sFile,nLine);
}

void operator delete[](void* p,int,const char*,int)
{
	vhwd::MemPool::current().deallocate(p);
}

#endif


