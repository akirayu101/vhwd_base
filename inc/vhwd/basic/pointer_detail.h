// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_POINTER_DETAIL__
#define __H_VHWD_POINTER_DETAIL__

#include "vhwd/basic/object.h"

#ifdef _MSC_VER
#pragma warning(disable:4180)
#endif

VHWD_ENTER


namespace detail
{

	template<typename T>
	class po_lite
	{
	public:

		static inline void reset(T* &p1,T* p2)
		{
			p1=p2;
		}

		static inline void equal(T* &p1,T* p2)
		{
			p1=p2;
		}
	};

	template<typename T>
	class po_auto
	{
	public:
		static inline void reset(T* &p1,T* p2)
		{
			if(p1==p2) return;
			delete p1;
			p1=p2;
		}

	private:
		static void equal(T* &p1,T* p2);
	};

	template<typename T>
	class po_data
	{
	public:

		static inline void reset(T* &p1,T* &p2)
		{
			if(p1==p2) return;
			if(p2) p2->IncRef();
			if(p1) p1->DecRef();
			p1=p2;
		}

		static inline void equal(T* &p1,T*& p2)
		{
			if(p1==p2) return;
			if(p2) p2->IncRef();
			if(p1) p1->DecRef();
			p1=p2;
		}

	};

	template<typename T>
	class po_sel
	{
	public:
		static const bool value=tl::is_convertible<T,ObjectData>::value;
		typedef typename tl::meta_if<value,po_data<T>,po_auto<T> >::type type;

	};

	template<typename T,typename P>
	class BasePtrT
	{
	public:
		typedef T *pointer;

		inline BasePtrT(T *p=NULL)
		{
			m_ptr=NULL;
			reset(p);
		}

		inline const BasePtrT &operator=(const BasePtrT& p)
		{
			P::equal(m_ptr,const_cast<T*&>(p.m_ptr));
			return *this;
		}

		inline BasePtrT(const BasePtrT& p) :m_ptr(NULL)
		{
			P::equal(m_ptr,const_cast<T*&>(p.m_ptr));
		}

		inline ~BasePtrT()
		{
			reset(NULL);
		}

		inline void swap(BasePtrT& p)
		{
			T* tmp(m_ptr);
			m_ptr=p.m_ptr;
			p.m_ptr=tmp;
		}

		inline void reset(pointer p)
		{
			P::reset(m_ptr,p);
		}

		inline pointer get()
		{
			return m_ptr;
		}

		inline const T* get() const
		{
			return m_ptr;
		}
		/*
				inline operator pointer()
				{
					return m_ptr;
				}

				inline operator const pointer() const
				{
					return m_ptr;
				}
		*/
		inline T &operator*()
		{
			return *m_ptr;
		}

		inline const T &operator*() const
		{
			return *m_ptr;
		}

		inline pointer operator->()
		{
			return m_ptr;
		}
		inline const T* operator->() const
		{
			return m_ptr;
		}

		inline operator bool() const
		{
			return m_ptr!=NULL;
		}


	protected:
		T *m_ptr;
	};
};

VHWD_LEAVE
#endif
