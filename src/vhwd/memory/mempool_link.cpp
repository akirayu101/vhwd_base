#include "mempool_link.h"
#include "mempool_impl.h"
#include "vhwd/basic/lockguard.h"
#include "vhwd/basic/console.h"
#include "vhwd/basic/system.h"
#include "vhwd/collection/indexer_map.h"
#include "vhwd/collection/bst_map.h"
#include "vhwd/memory/allocator.h"
#include "vhwd/basic/string.h"


#include <algorithm>

VHWD_ENTER


class MemAllocInfo : public ILinkNode
{
public:
	const char* sFile;
	unsigned int nCount;
	unsigned int nGuard;
	unsigned short nLine;
	unsigned short nSize;

	static const unsigned int GUARD_VALUE=0xCDEF1234;

};

const int MemLinking::const_fpi_size=(sizeof(MemAllocInfo)+sizeof(void*)-1)&~(sizeof(void*)-1);

void MemLinking::set_break_alloc(unsigned n)
{
	m_nBreakAlloc=n;
}

void* MemLinking::insert_link(void *orignalPointer,const char* sFile,size_t nLine,size_t nSize)
{
	if(m_nCount==m_nBreakAlloc)
	{
		System::DebugBreak();
	}

	MemAllocInfo* _pMemInfo=(MemAllocInfo*)orignalPointer;
	_pMemInfo->sFile=sFile;
	_pMemInfo->nCount=m_nCount++;
	_pMemInfo->nGuard=MemAllocInfo::GUARD_VALUE;
	_pMemInfo->nLine=nLine;
	_pMemInfo->nSize=nSize;

	LockGuard<AtomicSpin> lock1(m_tSpinLink);
	m_pMemLink.insert(_pMemInfo);

	return ((char*)orignalPointer)+const_fpi_size;
}

void* MemLinking::remove_link(void* p)
{
	MemAllocInfo* orignalPointer=(MemAllocInfo*) (((char*)p)-const_fpi_size);
	if(orignalPointer->nGuard!=MemAllocInfo::GUARD_VALUE)
	{
		return p;
	}
	LockGuard<AtomicSpin> lock1(m_tSpinLink);
	orignalPointer->UnLink();
	return orignalPointer;
}


MemLinking::MemLinking()
{
	m_bReportWhenExit=true;
	m_nCount=0;
	m_nBreakAlloc=(unsigned)-1;
}

void MemLinking::report_when_exit(bool f)
{
	m_bReportWhenExit=f;
}

bool MemLinking::report_when_exit()
{
	return m_bReportWhenExit;
}



MemLinking& MemLinking::current()
{
	static MemLinking gInstance;
	return gInstance;
}


MemLinking::~MemLinking()
{

	if(!report_when_exit()) return;

#ifdef VHWD_MEMDEBUG
	report();
#endif
}



void MemPoolBase::set_break_alloc(unsigned n)
{
	MemLinking::current().set_break_alloc(n);
}


class IMemAllocCount : public arr_1t<unsigned,AllocatorM<unsigned> >
{
public:

	void operator()(MemAllocInfo* p)
	{
		push_back(p->nCount);
	}
};

class IMemAllocInfo
{
public:
	IMemAllocInfo() {}
	IMemAllocInfo(MemAllocInfo* p)
	{
		sFile=p->sFile;
		nLine=p->nLine;
	}
	const char* sFile;
	int nLine;

	void operator()(IMemAllocCount& o) const
	{
		String sb;

		std::sort(o.begin(),o.end());

		if(sFile)
		{
			sb<<sFile<<":"<<nLine<<" "<<(int)o.size();
		}
		else
		{
			sb<<"unkown file"<<" "<<(int)o.size();;
		}

		sb<<"{";
		size_t i=0;
		for(;;)
		{
			sb<<(int)o[i];
			if(++i>=o.size()) break;
			sb<<",";
		}
		sb<<"}";
		System::LogTrace(sb);
	}
};


bool operator==(const IMemAllocInfo& lhs,const IMemAllocInfo& rhs)
{
	return memcmp(&lhs,&rhs,sizeof(IMemAllocInfo))==0;
}

bool operator<(const IMemAllocInfo& lhs,const IMemAllocInfo& rhs)
{
	return memcmp(&lhs,&rhs,sizeof(IMemAllocInfo))<0;
}

template<>class hash_t<IMemAllocInfo> : public vhwd::hash_impl<IMemAllocInfo> {};

class MemAllocInfoCompare
{
public:

	bool operator()(const IMemAllocInfo& lhs,const IMemAllocInfo& rhs)
	{
		return memcmp(&lhs,&rhs,sizeof(IMemAllocInfo))<0;
	}
};



template <typename T>
class AllocatorC
{
public:
	typedef T value_type;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;

	typedef T *pointer;
	typedef const T *const_pointer;

	typedef T &reference;
	typedef const T &const_reference;

public:

	AllocatorC() throw() {}

	AllocatorC(const AllocatorC&) {}

	template <typename T2>
	AllocatorC(const AllocatorC<T2> &) throw() {}

	~AllocatorC() throw() {}

	inline pointer address(reference r)
	{
		return &r;
	}

	inline const_pointer address(const_reference r) const
	{
		return &r;
	}

	MpObjectPool<T> cached;

	inline pointer allocate(size_type n)
	{
		wassert(n==1);
		if(n!=1)
		{
			Exception::XBadAlloc();
		}
		return cached.alloc();
	}

	inline void deallocate(pointer p, size_type)
	{		
		cached.dealloc(p);
	}


	inline void construct(pointer p, const value_type &o)
	{
		new(p) value_type(o);
	}

	inline void destroy(pointer p)
	{
		(void)&p;
		p->~value_type();
	}

	inline size_type max_size() const throw()
	{
		return size_type(-1) / sizeof(value_type);
	}

	template <typename T2>
	struct rebind
	{
		typedef AllocatorC<T2> other;
	};

	bool operator!=(const AllocatorC &other) const
	{
		return !(*this == other);
	}

	// Returns true if and only if storage allocated from *this
	// can be deallocated from other, and vice versa.
	// Always returns true for stateless allocators.
	bool operator==(const AllocatorC &other) const
	{
		(void)&other;
		return true;
	}
};


void MemLinking::report()
{
	typedef bst_map<IMemAllocInfo,IMemAllocCount,MemAllocInfoCompare,AllocatorC<int> > map_type;
	map_type alinfo;

	{
		LockGuard<AtomicSpin> lock1(m_tSpinLink);
		if(!m_pMemLink.tHead.pNext)
		{
			System::LogTrace("no memory records");
			return;
		}

		for(ILinkNode* link=m_pMemLink.tHead.pNext; link!=NULL; link=link->pNext)
		{
			MemAllocInfo* fp=(MemAllocInfo*)link;
			alinfo[fp](fp);
		}
	}

	System::LogTrace("---memlink report begin-------");
	for(map_type::iterator it=alinfo.begin(); it!=alinfo.end(); ++it)
	{
		(*it).first((*it).second);
	}
	System::LogTrace("---memlink report finished----");

}

VHWD_LEAVE
