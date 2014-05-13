#include "mempool_impl.h"
#include "vhwd/basic/system.h"


#ifdef new
#undef new
#endif

VHWD_ENTER

size_t MemPageCache::nPageBits=0;
size_t MemPageCache::nPageSize=0;

void MemPageCache::append(MemPageInfo* mi)
{
	size_t kp=(((size_t)mi)>>nPageBits)&nBucketMask;
	MemPageInfo* mo=aBuckets[kp].mi.exchange(mi);
	if(mo!=NULL)
	{
		aBuckets[kp].mi.exchange(mo);
		System::LogFetal("MemPageCache::append failed! addr1:%p, addr2:%p",mo,mi);
		MemPageInfo::DestroyPage(mi);
		Exception::XBadAlloc();
	}

}

void MemPageCache::remove(MemPageInfo* mi)
{
	size_t kp=(((size_t)mi)>>nPageBits)&nBucketMask;
	wassert(aBuckets[kp].mi.get()==mi);
	aBuckets[kp].mi.store(NULL);
}

MemPageInfo* MemPageCache::search(void* p_)
{
	size_t kp0=(((size_t)p_)-nPageSize)>>nPageBits;
	{
		size_t kp=(kp0+0)&nBucketMask;
		Bucket& bucket(aBuckets[kp]);
		MemPageInfo* mi=bucket.mi.get();
		if((size_t)((char*)p_-(char*)mi)<nPageSize)
		{
			return bucket.mi.get();
		}
	}
	{
		size_t kp=(kp0+1)&nBucketMask;
		Bucket& bucket(aBuckets[kp]);
		MemPageInfo* mi=bucket.mi.get();
		if((size_t)((char*)p_-(char*)mi)<nPageSize)
		{
			return bucket.mi.get();
		}
	}
	return NULL;
}


MemPageCache::MemPageCache()
{
	nPageBits=20;
	nPageSize=(size_t)1<<nPageBits;

	nBucketBits=20;
	nBucketCount=(size_t)1<<nBucketBits;
	nBucketMask=nBucketCount-1;

	aBuckets=(Bucket*)heap_alloc(nBucketCount*sizeof(Bucket));
	if(!aBuckets)
	{
		System::LogFetal("MemPageCache buckets allocate failed");
		Exception::XBadAlloc();
	}

	for(size_t i=0;i<nBucketCount;i++)
	{
		new(aBuckets+i) Bucket();
	}


	// Define Fix size allocate units.
	size_t sbit[]={8,16,24,32,48,64,80,96,112,128,256,512,768,1024,1536,2048,3072,4096};

	// Determine real slot count
	m_nFixedSizeCount=sizeof(sbit)/sizeof(int);

	for(size_t i=3;i<m_nFixedSizeCount;i++)
	{
		if(MemPageCache::nPageSize/sbit[i]<3)
		{
			m_nFixedSizeCount=i-1;
			break;
		}
	}

	m_nFixedSizeMax=sbit[m_nFixedSizeCount-1];

	// Allocate memory for aSlots and pSlots using malloc
	m_aSlots= (FixedSizeAllocatorUnit*)::malloc(sizeof(FixedSizeAllocatorUnit)*(m_nFixedSizeCount));
	m_pSlots=(FixedSizeAllocatorUnit**)::malloc(sizeof(void*)*(m_nFixedSizeMax+1));

	if(!m_aSlots||!m_pSlots)
	{
		if(m_aSlots) ::free(m_aSlots);
		if(m_pSlots) ::free(m_pSlots);
		System::LogTrace("MemPoolPaging slots allocation failed!");
		Exception::XBadAlloc();
	}

	// Initializing MemSlots
	for(size_t i=0;i<m_nFixedSizeCount;i++)
	{
		new (m_aSlots+i) FixedSizeAllocatorUnit(sbit[i]);
	}

	// Setting up size mapping, map size to MemSlot
	size_t j=0;
	for(size_t i=0;i<=m_nFixedSizeMax;i++)
	{
		if(i>sbit[j]) j++;
		m_pSlots[i]=m_aSlots+j;
	}
}

VHWD_LEAVE


