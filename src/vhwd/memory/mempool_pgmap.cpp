#include "mempool_impl.h"
#include "vhwd/basic/system.h"

VHWD_ENTER


MpAllocBucket* RadixPageMapT<32>::find_bucket(void* p)
{
	uintptr_t kp=reinterpret_cast<uintptr_t>(p);
	uintptr_t bp=kp>>MpAllocConfig::sp_bits;

	MpAllocSpan* sp=buckets[bp].get();
	if(sp)
	{
		if(kp>=sp->sp_base)
		{
			wassert(kp-sp->sp_base<sp->sp_size);
			return &buckets[bp];
		}
	}

	if(bp==0) return NULL;	

	// fall back 1 bucket
	sp=buckets[--bp].get();
	if(sp)
	{
		if(kp-sp->sp_base<sp->sp_size)
		{
			return &buckets[bp];
		}
		else
		{
			return NULL;
		}
	}

	// fall back more buckets, depends on
	for(size_t i=0;i<MpAllocConfig::bk_more;i++)
	{
		if(bp==0) return NULL;
	
		sp=buckets[--bp].get();
		if(sp)
		{
			if(kp-sp->sp_base<sp->sp_size)
			{
				return &buckets[bp];
			}
			else
			{
				return NULL;
			}
		}
	}

	return NULL;
}


inline MpAllocBucket& RadixPageMapT<64>::bucket(MpAllocSpan* sp)
{
	uintptr_t bp0=sp->sp_base>>MpAllocConfig::sp_bits;
	uintptr_t bp1=bp0>>bits1;
	uintptr_t bp2=bp0&mask1;

	if(buckets2[bp1]==NULL)
	{
		MpAllocConfig::lock_type lock1(bk_spin);
		if(buckets2[bp1]==NULL)
		{
			buckets2[bp1]=(BucketLevel2*)page_alloc(sizeof(BucketLevel2));
		}
	}
	return (*buckets2[bp1])[bp2];
}

void RadixPageMapT<64>::insert_span_nolock(MpAllocSpan* sp)
{
	MpAllocBucket& bk(bucket(sp));
	wassert(bk.get()==NULL);
	bk.set(sp);
}

void RadixPageMapT<64>::remove_span_nolock(MpAllocSpan* sp)
{
	MpAllocBucket& bk(bucket(sp));
	wassert(bk.get()==sp);
	bk.set(NULL);
}

MpAllocBucket* RadixPageMapT<64>::find_bucket(void* p)
{
	uintptr_t kp=reinterpret_cast<uintptr_t>(p);
	uintptr_t bp0=kp>>MpAllocConfig::sp_bits;
	uintptr_t bp1=bp0>>bits1;
	uintptr_t bp2=bp0&mask1;

	if(!buckets2[bp1])
	{
		return NULL;
	}

	BucketLevel2* pbucket(buckets2[bp1]);

	MpAllocSpan* sp=(*pbucket)[bp2].get();
	if(sp)
	{
		if(kp>=sp->sp_base)
		{
			wassert(kp-sp->sp_base<sp->sp_size);
			return &(*pbucket)[bp2];
		}
	}
		
	// fall back 1 bucket
	if(bp2==0)
	{
		if(bp1==0)
		{
			return NULL;
		}

		pbucket=buckets2[--bp1];
		if(!pbucket)
		{
			return NULL;
		}
		bp2=size1;
	}

	sp=(*pbucket)[--bp2].get();
	if(sp)
	{
		if(uintptr_t(kp-sp->sp_base)<sp->sp_size)
		{
			return &(*pbucket)[bp2];
		}
		else
		{
			return NULL;
		}
	}


	for(size_t i=0;i<MpAllocConfig::bk_more;i++)
	{		
		if(bp2==0)
		{
			if(bp1==0)
			{
				return NULL;
			}

			pbucket=buckets2[--bp1];
			if(!pbucket)
			{
				return NULL;
			}
			bp2=size1;
		}

		sp=(*pbucket)[--bp2].get();
		if(sp)
		{
			if(uintptr_t(kp-sp->sp_base)<sp->sp_size)
			{
				return &(*pbucket)[bp2];
			}
			else
			{
				return NULL;
			}
		}
	}

		
	return NULL;
}


VHWD_LEAVE


