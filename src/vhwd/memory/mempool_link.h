#include "vhwd/config.h"
#include "vhwd/memory/mempool.h"
#include "vhwd/basic/lockguard.h"
#include "vhwd/basic/atomic.h"
#include "vhwd/basic/system.h"


VHWD_ENTER


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
