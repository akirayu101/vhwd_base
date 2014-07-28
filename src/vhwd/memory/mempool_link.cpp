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
	static class TmpMemPool : public StaticObjectWithoutDeletorT<MemLinking>
	{
	public:

		~TmpMemPool()
		{
			MemLinking& pool(*this);
			if(!pool.report_when_exit()) return;

#ifdef VHWD_MEMDEBUG
			pool.report();
#endif
		}

	} mp;
	return mp;
}


MemLinking::~MemLinking()
{

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

void MemLinking::report()
{
	typedef bst_map<IMemAllocInfo,IMemAllocCount,MemAllocInfoCompare,AllocatorM<int> > map_type;
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
