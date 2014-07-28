#include "vhwd/config.h"
#include "vhwd/memory/mempool.h"
#include "vhwd/basic/lockguard.h"


VHWD_ENTER

void* heap_alloc(size_t nSize);
void heap_free(void* pMem,size_t);

class MemPageNode
{
public:
	MemPageNode* pNext;
};

class MemPageInfo
{
public:

	static MemPageInfo* CreatePage(FixedSizeAllocatorUnit& sl);
	static void DestroyPage(MemPageInfo* p);

	MemPageInfo* pNext;
	MemPageInfo* pPrev;
	MemPageInfo* pSibling; // next bucket

	FixedSizeAllocatorUnit* m_pSlot;
	MemPageNode* pFreeList;

	unsigned nUsedCount;


};

class MemPageCache
{
public:

	friend class MemPoolPaging;

	static size_t nPageBits;
	static size_t nPageSize;

	static MemPageCache& current();


	void append(MemPageInfo* mi);
	void remove(MemPageInfo* mi);
	MemPageInfo* search(void* p);

	// Default fixed size allocator units
	FixedSizeAllocatorUnit* m_aSlots;
	FixedSizeAllocatorUnit** m_pSlots;
	size_t m_nFixedSizeCount;
	size_t m_nFixedSizeMax;

	void _init();

protected:
	MemPageCache();

	class Bucket
	{
	public:
		AtomicIntT<MemPageInfo*> mi;
	};

	~MemPageCache() {}

	Bucket* aBuckets;
	size_t nBucketCount;
	size_t nBucketBits;
	size_t nBucketMask;

};



// FixedSizeAllocatorUnit is a fix size memory allocator unit.
class VHWD_DLLIMPEXP FixedSizeAllocatorUnit
{
public:
	FixedSizeAllocatorUnit(size_t n);

	MemPageInfo* pPageList;

	size_t nElemShift;
	size_t nElemCount;
	size_t nElemSize;
	size_t nPageSize;

	AtomicSpin tSpin;

	typedef LockGuard<AtomicSpin> LockType;

};


// Double LinkList node, assume pPrev is NOT NULL
class VHWD_DLLIMPEXP ILinkNode
{
public:
	ILinkNode* pNext;
	ILinkNode* pPrev;

	// UnLink current node
	void UnLink()
	{
		pPrev->pNext=pNext;
		if(pNext) pNext->pPrev=pPrev;
	}
};

// Double LinkList
class VHWD_DLLIMPEXP ILinkList
{
public:
	ILinkNode tHead;

	ILinkList()
	{
		tHead.pPrev=NULL;
		tHead.pNext=NULL;
	}

	void insert(ILinkNode* p)
	{
		p->pNext=tHead.pNext;
		p->pPrev=&tHead;
		if(tHead.pNext)
		{
			tHead.pNext->pPrev=p;
		}
		tHead.pNext=p;
	}
};


// MemLinking is a double linked list, it is used for linking all memory blocks allocated by MemPoolDebug
// The memory must be allocated with extra size of MemLinking::const_fpi_size.
class VHWD_DLLIMPEXP MemLinking : private NonCopyable
{
public:

	static const int const_fpi_size;
	static MemLinking& current();

	// Insert_link and return p+const_fpi_size
	void* insert_link(void *p,const char* file,size_t line,size_t size);

	// Remove_link and return p-const_fpi_size
	void* remove_link(void* p);

	// Report memeory blocks in MemLinking, using System::LogTrace.
	void report();

	// Set and get report when exit flag, only used in VHWD_MEMDEBUG mode.
	void report_when_exit(bool f);
	bool report_when_exit();

	MemLinking();
	~MemLinking();

	void set_break_alloc(unsigned n);


protected:
	unsigned m_nCount;
	unsigned m_nBreakAlloc;

	AtomicSpin m_tSpinLink;
	ILinkList m_pMemLink;

	bool m_bReportWhenExit;

};

VHWD_LEAVE

