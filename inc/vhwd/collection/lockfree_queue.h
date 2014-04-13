#ifndef __H_VHWD_COLLECTION_LOCKFREE_QUEUE__
#define __H_VHWD_COLLECTION_LOCKFREE_QUEUE__

#include "vhwd/collection/collection_base.h"
#include "vhwd/basic/atomic.h"
#include "vhwd/basic/bitflags.h"
#include "vhwd/basic/exception.h"
#include "vhwd/basic/system.h"

#include <cstdlib>


VHWD_ENTER

template<typename T>
class LockFreeQueuePolicy
{
public:
	typedef unsigned int size_type;

	// if getq/putq spin count > timeout, and queue timeout flag is set, current thread will enter.
	// this is designed to avoid that some thread get the key but failed to give back.
	static inline size_type timeout(){return 1024*1024*16;}

	// while Queue is NonBlock, getq return invalid_value() if Queue is empty.
	// alternative implementation: throw an exception
	static inline T invalid_value(){return T();}

	// move value from b to a.
	// alternative implementation: swap(a,b);
	static inline void move_value(T& a,T& b){a=b;}

	// if T is not POD, fill/destroy method must be implemented.
	static inline void fill(T*,size_type){}
	static inline void destroy(T*,size_type){}

	// called when key is busy or condition not met, such as queue is full while putq.
	// alternative implementation: Thread::yield()
	static inline void noop(){}

};

#pragma push_macro("new")
#undef new

template<typename T,typename P=LockFreeQueuePolicy<T> >
class LockFreeQueueImpl
{
public:
	typedef typename P::size_type size_type;

	// queue head position
	size_type head;

	// queue rear position
	size_type rear;

	// queue mask, must be 2^n-1
	size_type mask;

	BitFlags flags;

	// the key for getq
	AtomicInt32 key_get;

	// the key for putq
	AtomicInt32 key_put;
	T q[1];
};


// T must be pod_data
template<typename T,typename P=LockFreeQueuePolicy<T> >
class LockFreeQueue : private NonCopyable
{
public:

	typedef typename P::size_type size_type;

	enum
	{
		// queue is intialized
		QUEUE_INITED	=1<<0,

		// queue is nonblock
		QUEUE_NONBLOCK	=1<<1,

		// get key and enter if timeout
		QUEUE_TIMEOUT	=1<<2,
	};

	// calculate memsize needed for queue with a capacity of v.
	static size_t memsize(size_type v)
	{
		size_type k=16;while(k<v) k=k<<1;
		return sizeof(LockFreeQueueImpl<T,P>)+(k-1)*sizeof(T);
	}

	// initialized?
	bool ok()
	{
		return impl!=NULL;
	}

	// return queue size.
	size_type size()
	{
		if(!impl) return 0;
		return (impl->rear-impl->head)&impl->mask;
	}

	// return queue capacity, the queue actually can hold capacity-1 elements.
	size_type capacity()
	{
		if(!impl) return 0;
		return impl->mask+1;
	}

	// queue flags
	BitFlags& flags()
	{
		if(!impl){static BitFlags f;return f;}
		return impl->flags;
	}

	// get an element from queue head.
	T getq()
	{
		wassert(impl!=NULL);

		T q;
		int32_t tag;

		// test and check the key
		// if original value is zero then enter and get an element, and then store zero to the key and leave
		while((tag=impl->key_get.fetch_add(1))!=0)
		{
			if(tag!=(int32_t)P::timeout())
			{
				P::noop();
				continue;
			}

			if(impl->key_get.exchange(1)==0)
			{
				break;
			}

			if(impl->flags.get(QUEUE_TIMEOUT)) // timeout
			{
				System::LogTrace("getq key timeout!");
				break;
			}
		}


		while(impl->head==impl->rear) // the queue is empty
		{
			if(impl->flags.get(QUEUE_NONBLOCK)) // return invalid_value if nonblock
			{
				impl->key_get.store(0);
				return P::invalid_value();
			}
			else // wait until queue is not empty
			{
				// store(1) to tell others that I'm alive.
				impl->key_get.store(1);
				P::noop();
			}
		}

		P::move_value(q,impl->q[impl->head]); // get an element from head
		impl->head=(impl->head+1)&impl->mask; // advance head position
		impl->key_get.store(0); // give back the key.

		return q;

	}

	// push an element to the queue rear
	bool putq(T v)
	{
		wassert(impl!=NULL);

		int32_t tag;

		// test and check the key
		// if original value is zero then enter and get an element, and then store zero to the key and leave
		while((tag=impl->key_put.fetch_add(1))!=0)
		{
			if(tag!=(int32_t)P::timeout())
			{
				P::noop();
				continue;
			}

			if(impl->key_put.exchange(1)==0)
			{
				break;
			}

			if(impl->flags.get(QUEUE_TIMEOUT))
			{
				System::LogTrace("getq key timeout!");
				break;
			}
		}

		while(((impl->rear+1)&impl->mask)==impl->head) // queue is full
		{
			if(impl->flags.get(QUEUE_NONBLOCK)) // give back the key and return false if nonblock
			{
				impl->key_put.store(0);
				return false;
			}
			else
			{
				impl->key_put.store(1); // store(1) to tell others that I'm alive.
				P::noop();
			}
		}

		P::move_value(impl->q[impl->rear],v); // enqueue an element
		impl->rear=(impl->rear+1)&impl->mask; // advance rear position
		impl->key_put.store(0); // give back the key.
		return true;
	}

	void clear()
	{
		if(impl && m_bNeedFree)
		{
			P::destroy(impl->q,impl->mask+1);
			::free(impl);
		}
		m_bNeedFree=false;
		impl=NULL;
	}

	// create a queue with capacity of v
	// v will be adjust to 2^n

	void reset(size_type v)
	{
		size_t h=memsize(v);
		void* p=::malloc(h);
		if(p==NULL)
		{
			Exception::XBadAlloc();
		}

		init(p,h,true);
		m_bNeedFree=true;
	}

	// initialize the queue with give addr and size.
	bool init(void* addr_,size_type memsize_,bool forceinit_)
	{
		if(memsize_<sizeof(LockFreeQueueImpl<T,P>)+sizeof(T))
		{
			System::LogTrace("LockFreeQueue init failed!");
			return 0;
		}

		size_type _nElem=(memsize_+sizeof(T)-sizeof(LockFreeQueueImpl<T,P>))/sizeof(T);
		size_type v=1;
		while((v<<1)<=_nElem) v<<=1;

		clear();

		impl=(LockFreeQueueImpl<T,P>*)addr_;

		if(!forceinit_)
		{
			return true;
		}

		impl->flags.clr(QUEUE_INITED);
		impl->head=impl->rear=0;
		impl->mask=v-1;
		impl->key_get.store(0);
		impl->key_put.store(0);

		P::fill(impl->q,impl->mask+1);

		return true;

	}

	LockFreeQueue()
	{
		impl=NULL;
		m_bNeedFree=false;
	}

	~LockFreeQueue()
	{
		clear();
	}

private:
	LockFreeQueueImpl<T,P>* impl;
	bool m_bNeedFree;
};

#pragma pop_macro("new")

VHWD_LEAVE


#endif

