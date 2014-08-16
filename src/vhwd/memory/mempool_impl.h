#include "vhwd/config.h"
#include "vhwd/memory/mempool.h"
#include "vhwd/basic/lockguard.h"
#include "vhwd/basic/system.h"

#include <errno.h>

VHWD_ENTER

#define SPLIT_UNIT_N(X) ((1024*1024/X)&~4095)

static const size_t FixAllocUnits[]=
{
	8,16,24,32,48,64,96,128,192,256,384,512,1024,1024*2,1024*4,1024*8,
	1024*16,1024*24,1024*32,1024*64,
	SPLIT_UNIT_N(8),SPLIT_UNIT_N(6),SPLIT_UNIT_N(4),SPLIT_UNIT_N(3),SPLIT_UNIT_N(2),SPLIT_UNIT_N(1)
};


class MpAllocConfig
{
public:

	static const size_t sp_bits=20;
	static const size_t sp_size=1<<sp_bits;
	static const size_t sp_mask=sp_size-1;

	static const size_t pg_size=1024*4;
	static const size_t pg_mask=pg_size-1;

	static const size_t bk_more=0;	//sp_size/max(FixAllocUnits)-1;

	typedef AtomicSpin spin_type;
	typedef LockGuard<AtomicSpin> lock_type;


};

class MpAllocFlag
{
public:

	MpAllocFlag():val(0){}

	uintptr_t val;

	bool get_fly(){return val!=0;}
	void set_fly(bool f){val=f?1:0;}

};

class MpAllocNode
{
public:
	MpAllocNode* nd_next;

	static void link(void* p1,size_t sz,size_t tp,void* p3=NULL);
};


class MpAllocSlot;

class MpAllocSpan
{
public:

	uintptr_t sp_base;
	uintptr_t sp_size;
	MpAllocFlag sp_flag;

	MpAllocSpan* sl_next;
	MpAllocSpan* sl_prev;
	MpAllocSlot* sl_slot;

	MpAllocNode* nd_free;
	uintptr_t nd_nnum;


};

class MpAllocSlot
{
public:

	MpAllocSpan* sp_head;
	uint32_t nd_size;
	uint32_t nd_nall;
	MpAllocFlag sp_flag;
	MpAllocConfig::spin_type sl_spin;

	void init(size_t nd);

};


template<typename T>
class MpObjectPool
{
public:
	static const size_t nd_size=(sizeof(T)+sizeof(void*)-1)&~(sizeof(void*)-1);

	MpAllocNode* nd_free;
	MpAllocNode* pg_list;

	MpObjectPool()
	{
		nd_free=NULL;
		pg_list=NULL;
	}

	~MpObjectPool()
	{
		while(pg_list)
		{
			MpAllocNode* pg=pg_list;
			pg_list=pg_list->nd_next;
			page_free(pg,MpAllocConfig::sp_size);
		}
	}

	void alloc_batch()
	{
		MpAllocNode* p1=(MpAllocNode*)page_alloc(MpAllocConfig::sp_size);
		if(!p1)
		{
			System::LogError("page_alloc failed in MpObjectPool::alloc_batch");
			return;
		}

		MpAllocNode::link(p1,MpAllocConfig::sp_size,nd_size,nd_free);
		nd_free=p1->nd_next;

		p1->nd_next=pg_list;
		pg_list=p1;
	}

	T* alloc()
	{
		if(!nd_free)
		{
			alloc_batch();
			if(!nd_free)
			{
				return NULL;
			}
		}
		T* p=(T*)nd_free;
		nd_free=nd_free->nd_next;
		return p;
	}

	void dealloc(T* p)
	{
		MpAllocNode* fn=(MpAllocNode*)p;
		fn->nd_next=nd_free;
		nd_free=fn;
	}

};


class MpAllocBucket
{
public:
	AtomicIntT<MpAllocSpan*>  span;
	MpAllocSpan* get()
	{
		return span.get();
	}
	void set(MpAllocSpan* sp)
	{
		span.store(sp);
	}
};


template<size_t BITS>
class RadixPageMapT;

template<>
class RadixPageMapT<32>
{
public:
	static const uintptr_t bits1=32-20;
	static const uintptr_t size1=1<<bits1;
	static const uintptr_t mask1=size1-1;

	MpAllocBucket buckets[size1];

	void insert_span_nolock(MpAllocSpan* sp)
	{
		uintptr_t bp=sp->sp_base>>MpAllocConfig::sp_bits;
		wassert(buckets[bp].get()==NULL);
		buckets[bp].set(sp);
	}

	void remove_span_nolock(MpAllocSpan* sp)
	{
		uintptr_t bp=sp->sp_base>>MpAllocConfig::sp_bits;
		wassert(buckets[bp].get()==sp);
		buckets[bp].set(NULL);
	}

	MpAllocBucket* find_bucket(void* p);

	MpAllocSpan* find_span(void* p)
	{
		MpAllocBucket* bk=find_bucket(p);
		return bk?bk->get():NULL;
	}
};

template<>
class RadixPageMapT<64>
{
public:
	static const uintptr_t bits0=64-MpAllocConfig::sp_bits;

	static const uintptr_t bits1=bits0/2;
	static const uintptr_t size1=1<<bits1;
	static const uintptr_t mask1=size1-1;

	static const uintptr_t bits2=bits0-bits1;
	static const uintptr_t size2=1<<bits2;
	static const uintptr_t mask2=size2-1;

	
	typedef MpAllocBucket BucketLevel2[size1];
	BucketLevel2* buckets2[size2];

	MpAllocConfig::spin_type bk_spin;;

	MpAllocBucket& bucket(MpAllocSpan* sp);
	void insert_span_nolock(MpAllocSpan* sp);
	void remove_span_nolock(MpAllocSpan* sp);

	MpAllocBucket* find_bucket(void* p);

	MpAllocSpan* find_span(void* p)
	{
		MpAllocBucket* bk=find_bucket(p);
		return bk?bk->get():NULL;
	}
};


class MpAllocGlobal
{
public:

	static const size_t sz_bits1=3;
	static const size_t sz_bits2=12;
	static const size_t sz_bits3=20;

	static const size_t sz_slot1=1<<sz_bits2;
	static const size_t sz_slot2=1<<sz_bits3;

	static const size_t sl_size=sizeof(FixAllocUnits)/sizeof(size_t);

	typedef RadixPageMapT<sizeof(void*)*8> pagemap_type;

	pagemap_type pgmap;
	MpObjectPool<MpAllocSpan> spanalloc;
	MpAllocConfig::spin_type span_spin;

	MpAllocSlot aSlots[sl_size];
	MpAllocSlot* pSlot1[(sz_slot1>>sz_bits1)+1];
	MpAllocSlot* pSlot2[(sz_slot2>>sz_bits2)+1];


	void init();

	void gc();


	MpAllocSlot* get_slot(size_t n)
	{
		if(n<=sz_slot1)
		{
			size_t k=(n+((1<<sz_bits1)-1))>>sz_bits1;
			return pSlot1[k];
		}
		if(n<=sz_slot2)
		{
			size_t k=(n+((1<<sz_bits2)-1))>>sz_bits2;
			return pSlot2[k];
		}
		return NULL;
	}


	MpAllocSpan* create_span_nolock(MpAllocSlot& sl);
	MpAllocNode* dealloc_batch_nolock(MpAllocSlot& sl,MpAllocNode* fp,size_t sz);
	MpAllocNode* alloc_small_batch(MpAllocSlot& sl,size_t& sz);

	void* alloc_small(size_t n,MpAllocSlot& sl);
	void* alloc_large(size_t n);
	void dealloc_real(void* p,MpAllocBucket& bk);
	void* realloc_real(void* p,size_t n,MpAllocBucket& bk);

	void* alloc(size_t n)
	{
		MpAllocSlot* psl=get_slot(n);
		if(!psl)
		{
			return alloc_large(n);
		}
		else
		{
			return alloc_small(n,*psl);
		}
	}

	void* realloc(void* p,size_t n)
	{
		if(p==NULL)
		{
			return alloc(n);
		}
		if(n==0)
		{
			dealloc(p);
			return NULL;
		}

		MpAllocBucket* pbk=pgmap.find_bucket(p);
		if(pbk)
		{
			return realloc_real(p,n,*pbk);
		}
		
		return ::realloc(p,n);
	}

	void dealloc(void* p)
	{
		MpAllocBucket* pbk=pgmap.find_bucket(p);
		if(pbk)
		{
			dealloc_real(p,*pbk);
			return;
		}

		::free(p);
	}
};

class MpAllocNodeLink
{
public:
	MpAllocNodeLink(){nd_head=NULL;nd_nnum=0;}
	MpAllocNode* nd_head;
	size_t nd_nnum;
	size_t nd_nmax;
};

extern MpAllocGlobal *g_myalloc_impl;

class MpAllocCachedNoLock
{
public:
	MpAllocNodeLink aLinks[MpAllocGlobal::sl_size];

	void init();
	void cleanup();
	void gc();
	void* alloc(size_t n);
	void dealloc(void* p);

};

VHWD_LEAVE

