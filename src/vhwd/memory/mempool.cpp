#include "mempool_impl.h"
#include "vhwd/basic/system.h"


#ifdef _WIN32
#include <windows.h>
#else
#include <sys/mman.h>
#include <cstring>
#endif

#ifdef new
#undef new
#endif


VHWD_ENTER


#ifdef _WIN32

void* page_alloc(size_t n)
{
	void* p = ::VirtualAlloc(NULL, n, MEM_COMMIT, PAGE_READWRITE );
	if(p)
	{
		return p;
	}
	
	if(g_myalloc_impl)
	{
		System::LogTrace("page_alloc failed, call gc() and retry");
		g_myalloc_impl->gc();

		p = ::VirtualAlloc(NULL, n, MEM_COMMIT, PAGE_READWRITE );
		if(p)
		{
			return p;
		}
	}


	int ret=::GetLastError();
	System::LogTrace("VirtualAlloc failed: ret=%d",ret);
	return NULL;
}

void page_free(void* p,size_t)
{
	if(::VirtualFree(p, 0, MEM_RELEASE)==0)
	{
		System::LogTrace("VirtualFree failed: ptr=%p",p);
	}
}

void heap_protect(void* p,size_t n,bool f)
{
	DWORD old;
	VirtualProtect(p,n,f?PAGE_READONLY:PAGE_READWRITE,&old);
}

#else

void* page_alloc(size_t nSize)
{

	void* p=mmap(NULL,nSize,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);

	if(p!=MAP_FAILED)
	{
		return p;
	}

	if(g_myalloc_impl)
	{
		System::LogTrace("page_alloc failed, call gc() and retry");
		g_myalloc_impl->gc();

		p=mmap(NULL,nSize,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);
		if(p!=MAP_FAILED)
		{
			return p;
		}
	}


	int ret=errno;
	System::LogTrace("mmap failed: %s",strerror(ret));
	return NULL;
}

void heap_protect(void* p,size_t n,bool f)
{
	mprotect (p, n, f?PROT_READ:(PROT_READ|PROT_WRITE));
}

void page_free(void* p,size_t s)
{
	munmap(p,s);
}
#endif



void* SmallObject::operator new[](size_t nSize)
{
	return vhwd::MemPoolPaging::allocate(nSize,NULL,-1);
}

void* SmallObject::operator new(size_t nSize,const char* sFile,int nLine)
{
	return vhwd::MemPoolPaging::allocate(nSize,sFile,nLine);
}

void* SmallObject::operator new[](size_t nSize,const char* sFile,int nLine)
{
	return vhwd::MemPoolPaging::allocate(nSize,sFile,nLine);
}

void SmallObject::operator delete(void* p,const char*,int)
{
	vhwd::MemPoolPaging::deallocate(p);
}
void SmallObject::operator delete[](void* p,const char*,int)
{
	vhwd::MemPoolPaging::deallocate(p);
}
void* SmallObject::operator new(size_t nSize)
{
	return vhwd::MemPoolPaging::allocate(nSize,NULL,-1);
}

void SmallObject::operator delete(void* p)
{
	vhwd::MemPoolPaging::deallocate(p);
}

void SmallObject::operator delete[](void* p)
{
	vhwd::MemPoolPaging::deallocate(p);
}

void* SmallObject::operator new(size_t s,void* p)
{
	(void)&s;
	return p;
}

void SmallObject::operator delete(void*,void*) {}

VHWD_LEAVE

#if defined(VHWD_MEMDEBUG) || defined(VHWD_MEMUSEPOOL)

void* operator new(size_t nSize)
{
	return vhwd::MemPool::allocate(nSize);
}

void operator delete(void* p)
{
	vhwd::MemPool::deallocate(p);
}

void* operator new(size_t nSize,const char* sFile,int nLine)
{
	return vhwd::MemPool::allocate(nSize,sFile,nLine);
}

void operator delete(void* p,const char*,int)
{
	vhwd::MemPool::deallocate(p);
}

void* operator new(size_t nSize,int,const char* sFile,int nLine)
{
	return vhwd::MemPool::allocate(nSize,sFile,nLine);
}

void operator delete(void* p,int,const char*,int)
{
	vhwd::MemPool::deallocate(p);
}

void* operator new[](size_t nSize)
{
	return vhwd::MemPool::allocate(nSize);
}

void operator delete[](void* p)
{
	vhwd::MemPool::deallocate(p);
}

void* operator new[](size_t nSize,const char* sFile,int nLine)
{
	return vhwd::MemPool::allocate(nSize,sFile,nLine);
}

void operator delete[](void* p,const char*,int)
{
	vhwd::MemPool::deallocate(p);
}

void* operator new[](size_t nSize,int,const char* sFile,int nLine)
{
	return vhwd::MemPool::allocate(nSize,sFile,nLine);
}

void operator delete[](void* p,int,const char*,int)
{
	vhwd::MemPool::deallocate(p);
}

#endif
