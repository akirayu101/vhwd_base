// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_SMART_PTR__
#define __H_VHWD_SMART_PTR__

#include "vhwd/basic/object.h"
#include "vhwd/basic/pointer_detail.h"

VHWD_ENTER

// LitePtrT almost same as T*, but will initalize m_ptr to null pointer in default constructor.
template<typename T>
class LitePtrT : public detail::BasePtrT<T,detail::po_lite<T> >
{
public:

    typedef detail::BasePtrT<T,detail::po_lite<T> > basetype;
    typedef T* pointer;
    using basetype::m_ptr;

	LitePtrT(pointer p=NULL):basetype(p){}

	inline operator pointer()
	{
		return m_ptr;
	}

	inline operator const T*() const
	{
		return m_ptr;
	}

	inline pointer operator=(pointer p)
	{
		m_ptr=p;
		return m_ptr;
	}
};

// Reference counted, T must derive from ObjectData.
template<typename T>
class DataPtrT : public detail::BasePtrT<T,detail::po_data<T> >
{
public:
	template<typename X>
	DataPtrT(const DataPtrT<X>& o)
	{
		reset(o.get());
	}

	DataPtrT(T *p=NULL):detail::BasePtrT<T,detail::po_data<T> >(p){}

};

// AutoPtrT almost same as std::auto_ptr, but no copy constructor and operator=
template<typename T>
class AutoPtrT : public detail::BasePtrT<T,detail::po_auto<T> > , private NonCopyable
{
public:
	using detail::BasePtrT<T,detail::po_auto<T> >::m_ptr;

	explicit AutoPtrT(T *p=NULL) : detail::BasePtrT<T,detail::po_auto<T> >(p){}

	inline T *release()
	{
		T *tmp=m_ptr;
		m_ptr=NULL;
		return tmp;
	}

};

template<typename T>
class TempPtrT : public AutoPtrT<T>
{
public:

	TempPtrT(){}
	TempPtrT(T* p):AutoPtrT<T>(p){}
	TempPtrT(const TempPtrT& o)
	{
		swap(const_cast<TempPtrT&>(o));
	}
	const TempPtrT& operator=(const TempPtrT& o)
	{
		swap(const_cast<TempPtrT&>(o));
		return *this;
	}
};


// if T is not derived from ObjectData, it's same as LitePtrT, else DataPtrT
template<typename T>
class FakePtrT : public tl::meta_if<tl::is_convertible<T,ObjectData>::value,DataPtrT<T>,LitePtrT<T> >::type
{
public:
	typedef typename tl::meta_if<tl::is_convertible<T,ObjectData>::value,DataPtrT<T>,LitePtrT<T> >::type basetype;
	FakePtrT(T* p=NULL):basetype(p){}

};

template<typename T,typename B=Object>
class ObjectT : public B
{
public:
	typedef T obj_type;

	inline void SetData(T* d)
	{
		m_refData.reset(d);
	}

	inline T* GetData()
	{
		return m_refData.get();
	}

	inline const T* GetData() const
	{
		return m_refData.get();
	}

protected:
	DataPtrT<T> m_refData;
};


#define VHWD_BASEPTRT_REL_OP(X) \
template<typename T,typename P1,typename P2>\
inline bool operator X(const detail::BasePtrT<T,P1>& lhs,const detail::BasePtrT<T,P2>& rhs)\
{\
	return lhs.get() X rhs.get();\
}\
template<typename T,typename P>\
inline bool operator X(const detail::BasePtrT<T,P>& lhs,T* rhs)\
{\
	return lhs.get() X rhs;\
}\
template<typename T,typename P>\
inline bool operator X(T* lhs,const detail::BasePtrT<T,P>& rhs)\
{\
	return lhs X rhs.get();\
}\

VHWD_BASEPTRT_REL_OP(==)
VHWD_BASEPTRT_REL_OP(!=)
VHWD_BASEPTRT_REL_OP(>=)
VHWD_BASEPTRT_REL_OP(>)
VHWD_BASEPTRT_REL_OP(<)
VHWD_BASEPTRT_REL_OP(<=)

#undef VHWD_BASEPTRT_REL_OP

VHWD_LEAVE


#endif

