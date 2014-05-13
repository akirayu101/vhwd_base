// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

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

template<typename T>
class LockFreeQueuePolicyInt : public LockFreeQueuePolicy<T>
{
public:
	static inline T invalid_value(){return T(-1);}
};

template<typename T>
class LockFreeQueuePolicyObj : public LockFreeQueuePolicy<T>
{
public:
    typedef typename LockFreeQueuePolicy<T>::size_type size_type;
	static inline void fill(T* p,size_type n)
	{
		xmem<T>::uninitialized_fill_n(p,n,T());
	}
	static inline void destroy(T* p,size_type n)
	{
		xmem<T>::destroy(p,n);
	}
};

#pragma push_macro("new")
#undef new

class lf_queue_header
{
public:
	AtomicInt32 head;
	AtomicInt32 rear;
	AtomicInt32 kget;
	AtomicInt32 kput;

	int32_t mask;
	BitFlags flags;

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

		QUEUE_FREE_HEADER=1<<3,
		QUEUE_FREE_BUFFER=1<<4,
	};

	static T invalid_value(){return P::invalid_value();}


	// initialized?
	bool ok()
	{
		return pHeader!=NULL;
	}

	// return queue size.
	size_type size()
	{
		if(!pHeader) return 0;
		return (pHeader->rear.get()-pHeader->head.get())&pHeader->mask;
	}

	// return queue capacity, the queue actually can hold capacity-1 elements.
	size_type capacity()
	{
		if(!pHeader) return 0;
		return pHeader->mask+1;
	}

	bool empty()
	{
		if(!pHeader) return false;
		return pHeader->rear.get()==pHeader->head.get();
	}

	bool full()
	{
		if(!pHeader) return true;
		return ((pHeader->rear.get()+1)&pHeader->mask)==pHeader->head.get();
	}

	// queue flags
	BitFlags& flags()
	{
		if(!pHeader){static BitFlags f;return f;}
		return pHeader->flags;
	}

	// get an element from queue head.
	T getq()
	{
		wassert(pHeader!=NULL);

		T q;
		int32_t tag;

		// test and check the key
		// if original value is zero then enter and get an element, and then store zero to the key and leave
		while((tag=pHeader->kget.fetch_add(1))!=0)
		{
			if(tag!=(int32_t)P::timeout())
			{
				P::noop();
				continue;
			}

			if(pHeader->kget.exchange(1)==0)
			{
				break;
			}

			if(pHeader->flags.get(QUEUE_TIMEOUT)) // timeout
			{
				System::LogTrace("getq key timeout!");
				break;
			}
		}


		while(pHeader->head.get()==pHeader->rear.get()) // the queue is empty
		{
			if(pHeader->flags.get(QUEUE_NONBLOCK)) // return invalid_value if nonblock
			{
				pHeader->kget.store(0);
				return P::invalid_value();
			}
			else // wait until queue is not empty
			{
				// store(1) to tell others that I'm alive.
				pHeader->kget.store(1);
				P::noop();
			}
		}

		P::move_value(q,pBuffer[pHeader->head.get()]); // get an element from head
		pHeader->head.store((pHeader->head.get()+1)&pHeader->mask); // advance head position
		pHeader->kget.store(0); // give back the key.

		return q;

	}

	// push an element to the queue rear
	bool putq(T v)
	{
		wassert(pHeader!=NULL);

		int32_t tag;

		// test and check the key
		// if original value is zero then enter and get an element, and then store zero to the key and leave
		while((tag=pHeader->kput.fetch_add(1))!=0)
		{
			if(tag!=(int32_t)P::timeout())
			{
				P::noop();
				continue;
			}

			if(pHeader->kput.exchange(1)==0)
			{
				break;
			}

			if(pHeader->flags.get(QUEUE_TIMEOUT))
			{
				System::LogTrace("getq key timeout!");
				break;
			}
		}

		while(((pHeader->rear.get()+1)&pHeader->mask)==pHeader->head.get()) // queue is full
		{
			if(pHeader->flags.get(QUEUE_NONBLOCK)) // give back the key and return false if nonblock
			{
				pHeader->kput.store(0);
				return false;
			}
			else
			{
				pHeader->kput.store(1); // store(1) to tell others that I'm alive.
				P::noop();
			}
		}

		P::move_value(pBuffer[pHeader->rear.get()],v); // enqueue an element
		pHeader->rear.store((pHeader->rear.get()+1)&pHeader->mask); // advance rear position
		pHeader->kput.store(0); // give back the key.
		return true;
	}

	void clear()
	{
		if(!pHeader)
		{
			return;
		}

		if(pHeader->flags.get(QUEUE_FREE_BUFFER))
		{
			P::destroy(pBuffer,pHeader->mask+1);
			::free(pBuffer);
		}

		if(pHeader->flags.get(QUEUE_FREE_HEADER))
		{
			::free(pHeader);
		}

		pBuffer=NULL;
		pHeader=NULL;
	}

	// create a queue with capacity of v
	// v will be adjust to 2^n

	void reset(size_type v)
	{
		v=sz_helper::n2p(v);

		lf_queue_header* p1=(lf_queue_header*)::malloc(sizeof(lf_queue_header));
		if(!p1)
		{
			Exception::XBadAlloc();
		}
		T* p2=(T*)::malloc(sizeof(T)*v);

		if(p2==NULL)
		{
			::free(p2);
			Exception::XBadAlloc();
		}

		p1->flags.clr(QUEUE_FREE_BUFFER|QUEUE_FREE_HEADER);
		p1->mask=v-1;
		init(p1,p2,true);
	}

	// initialize the queue with give addr and size.
	bool init(lf_queue_header* addr_,T* buff_,bool forceinit_)
	{
		//size_type _nElem=addr_->mask+1;
		clear();

		pHeader=addr_;
		pBuffer=buff_;

		if(!forceinit_)
		{
			return true;
		}

		pHeader->flags.add(QUEUE_INITED);
		pHeader->head.store(0);
		pHeader->rear.store(0);
		pHeader->kget.store(0);
		pHeader->kput.store(0);

		P::fill(pBuffer,pHeader->mask+1);

		return true;

	}

	LockFreeQueue()
	{
		pHeader=NULL;
		pBuffer=NULL;
	}

	LockFreeQueue(size_type n,int f)
	{
		pHeader=NULL;
		pBuffer=NULL;
		reset(n);
		flags().add(f);
	}

	~LockFreeQueue()
	{
		clear();
	}

private:
	lf_queue_header* pHeader;
	T* pBuffer;
};

#pragma pop_macro("new")

VHWD_LEAVE


#endif

