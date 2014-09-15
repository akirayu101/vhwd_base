// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_MEMORY_GC_PTR__
#define __H_VHWD_MEMORY_GC_PTR__

#include "vhwd/config.h"
#include "vhwd/basic/atomic.h"
#include "vhwd/basic/bitflags.h"
#include "vhwd/collection/arr_1t.h"

VHWD_ENTER

class VHWD_DLLIMPEXP gc_obj;

VHWD_DLLIMPEXP void garbage_force_collect();

template<typename T>
class gc_ptr;

// create object T (T must be derived from gc_obj) and register it to the garbage collection system.
// if T is not created by gc_new, T will not be managed by the garbage collection system.
template<typename T>
gc_ptr<T> gc_new();


class VHWD_DLLIMPEXP gc_handler;

class VHWD_DLLIMPEXP gc_state
{
public:

	enum
	{
		TAG_INIT=0,
		TAG_FLAG=1,
		TAG_MARK=2,
	};

	enum
	{
		FLAG_SHOW_GC_INFO	=1<<0,
	};
	
	class addrinfo_type
	{
	public:

		void* addr;
		size_t size;

		inline bool test(void* p)
		{
			return size_t((char*)p-(char*)addr)<size;
		}
	};

	class dlinkptr_type
	{
	public:
		dlinkptr_type(gc_obj* p=NULL):m_ptr(p),_gc_next(NULL),_gc_prev(NULL){}

		gc_obj* m_ptr;
		dlinkptr_type* _gc_next;
		dlinkptr_type* _gc_prev;
	};

	class queue_type
	{
	public:
		typedef arr_1t<gc_obj*> impl_type;
		typedef impl_type::iterator iterator;

		inline iterator begin(){return impl.begin();}
		inline iterator end(){return impl.end();}
		inline void swap(queue_type& o){impl.swap(o.impl);}

		inline bool empty(){return impl.empty();}
		inline void clear(){impl.clear();}
		inline void reset(){impl.resize(0);}

		void append(gc_obj* o);

		template<typename IT>
		void append(IT t1,IT t2);

	protected:
		impl_type impl;
	};



	typedef int32_t tags_type;

	static uint32_t nStepingDelta;
	static addrinfo_type tCurrentObject;
	static uint32_t nNumberOfObject;
	static uint32_t nNumberOfObjectLast;
	static AtomicMutex tRecursiveMutex;
	static gc_obj* pLinkOfAllObject;
	static BitFlags flags;

	static dlinkptr_type tPtrLink;

};


class VHWD_DLLIMPEXP gc_obj
{
public:

	friend class gc_handler;
	friend class gc_state;

	inline gc_obj()
	{
		_gc_tags=gc_state::TAG_INIT;
	}

	inline gc_obj(const gc_obj& o)
	{
		VHWD_UNUSED(o);
		_gc_tags=gc_state::TAG_INIT;
	}

	virtual ~gc_obj(){}


protected:

	// mark object that is connected to this
	virtual void _gc_mark(gc_state::queue_type& q)
	{
		VHWD_UNUSED(q);
	}

	gc_obj* _gc_next;;
	gc_state::tags_type _gc_tags;
};

inline void gc_state::queue_type::append(gc_obj* p)
{
	if(!p||p->_gc_tags==gc_state::TAG_MARK) return;
	p->_gc_tags=gc_state::TAG_MARK;
	impl.push_back(p);
}

template<typename IT>
inline void gc_state::queue_type::append(IT t1,IT t2)
{
	while(t1!=t2) append(*t1++);
}

// do not call function in this class manually
class VHWD_DLLIMPEXP gc_handler
{
public:

	static inline void link(gc_obj* p)
	{
		wassert(p!=NULL);
		p->_gc_next=gc_state::pLinkOfAllObject;
		gc_state::pLinkOfAllObject=p;
	}

	static void mark(gc_state::queue_type& q);

	static void gc_nolock(bool force_marking_);

	static void gc()
	{
		gc_state::tRecursiveMutex.lock();
		gc_nolock(false);
		gc_state::tRecursiveMutex.unlock();
	}

	static inline void insert(gc_state::dlinkptr_type* p)
	{
		if(!gc_state::tCurrentObject.test(p))
		{
			gc_state::tRecursiveMutex.lock();

			p->_gc_next=gc_state::tPtrLink._gc_next;
			p->_gc_prev=&gc_state::tPtrLink;

			if(gc_state::tPtrLink._gc_next)
			{
				gc_state::tPtrLink._gc_next->_gc_prev=p;
			}
			gc_state::tPtrLink._gc_next=p;

			gc_state::tRecursiveMutex.unlock();
		}
	}

	static inline void erase(gc_state::dlinkptr_type* p)
	{
		if(p->_gc_prev)
		{
			gc_state::tRecursiveMutex.lock();

			gc_state::dlinkptr_type* p1=p->_gc_prev;
			gc_state::dlinkptr_type* p2=p->_gc_next;

			p1->_gc_next=p2;
			if(p2)
			{
				p2->_gc_prev=p1;
			}

			gc_state::tRecursiveMutex.unlock();
		}
	}
	
	static inline void* alloc(size_t n)
	{
		return tc_alloc(n);
	}

	static inline void dealloc(void* p)
	{
		tc_free(p);
	}

	static inline void destroy(gc_obj* p)
	{
		--gc_state::nNumberOfObject;
		p->~gc_obj();
		dealloc(p);
	}

};

template<typename T>
class gc_ptr : private gc_state::dlinkptr_type
{
public:

	typedef gc_state::dlinkptr_type basetype;
	typedef T* pointer;
	typedef const T* const_pointer;

	gc_ptr(T* p=NULL):basetype(p)
	{
		gc_handler::insert(this);
	}

	gc_ptr(const gc_ptr& o):basetype(o.m_ptr)
	{
		gc_handler::insert(this);
	}

	gc_ptr& operator=(const gc_ptr& o)
	{
		reset((pointer)o.get());
		return *this;
	}

	template<typename G>
	gc_ptr(const gc_ptr<G>& o)
	{
		reset((G*)o.get());
		gc_handler::insert(this);
	}


	template<typename G>
	gc_ptr& operator=(const gc_ptr<G>& o)
	{
		reset((G*)o.get());
		return *this;
	}

	~gc_ptr()
	{
		gc_handler::erase(this);
	}


	inline operator pointer()
	{
		return (pointer)m_ptr;
	}

	inline operator const_pointer() const
	{
		return (const_pointer)m_ptr;
	}


	inline void reset(pointer p)
	{
		m_ptr=p;
	}

	inline pointer get()
	{
		return (pointer)m_ptr;
	}

	inline const_pointer get() const
	{
		return (const_pointer)m_ptr;
	}
		
		
	inline T &operator*()
	{
		return *(pointer)m_ptr;
	}

	inline const T &operator*() const
	{
		return *(const_pointer)m_ptr;
	}

	inline pointer operator->()
	{
		return (pointer)m_ptr;
	}

	inline const T* operator->() const
	{
		return (const_pointer)m_ptr;
	}

	inline operator bool() const
	{
		return m_ptr!=NULL;
	}

	inline void swap(gc_ptr& o)
	{
		std::swap(m_ptr,o.m_ptr);
	}

};

template<typename T>
gc_ptr<T> gc_new()
{

	T* p=(T*)gc_handler::alloc(sizeof(T));
	if(!p)
	{
		System::LogTrace("gc_new: alloc failed, force collect and retry");
		garbage_force_collect();

		p=(T*)gc_handler::alloc(sizeof(T));
		if(!p)
		{
			System::LogTrace("gc_new failed: not enough memory");
			Exception::XBadAlloc();
		}
	}

	gc_state::tRecursiveMutex.lock();

	wassert(gc_state::nNumberOfObject>=gc_state::nNumberOfObjectLast);

	if(gc_state::nNumberOfObject>gc_state::nNumberOfObjectLast+gc_state::nStepingDelta)
	{
		gc_handler::gc_nolock(false);
	}

	gc_state::addrinfo_type _tOldObjectInfo(gc_state::tCurrentObject);

	gc_state::tCurrentObject.addr=p;
	gc_state::tCurrentObject.size=sizeof(T);

	try
	{
		// constuctor may throw exception

#pragma push_macro("new")
#undef new

		new(p) T();

#pragma pop_macro("new")

	}
	catch(...)
	{
		gc_state::tCurrentObject=_tOldObjectInfo;
		gc_state::tRecursiveMutex.unlock();

		gc_handler::dealloc(p);
		throw;
	}


	gc_ptr<T> gp(p);

	++gc_state::nNumberOfObject;
	gc_handler::link(p);

	gc_state::tCurrentObject=_tOldObjectInfo;
	gc_state::tRecursiveMutex.unlock();

	return gp;
}


VHWD_LEAVE
#endif
