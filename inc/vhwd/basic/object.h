// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_BASIC_OBJECT__
#define __H_VHWD_BASIC_OBJECT__


#include "vhwd/basic/atomic.h"
#include "vhwd/basic/string.h"

#define DECLARE_OBJECT_INFO(TYPE,INFO)	\
	public:\
	typedef INFO infobase;\
	typedef ObjectInfoT<TYPE,INFO> infotype;\
	static  infotype sm_info;\
	virtual infobase* GetObjectInfo() const{return &sm_info;}\
	virtual const String& GetObjectName() const {return sm_info.GetObjectName();}


#define DECLARE_OBJECT_INFO_ABSTRACT(TYPE,INFO)


#define DECLARE_OBJECT_INFO_BASE(TYPE,INFO,BASE)	\
	DECLARE_OBJECT_INFO(TYPE,INFO)\
	typedef BASE basetype;\

#define DECLARE_OBJECT_INFO_BASE_ABSTRACT(TYPE,INFO,BASE)	\
	DECLARE_OBJECT_INFO_ABSTRACT(TYPE,INFO)\
	typedef BASE basetype;\

#define IMPLEMENT_OBJECT_INFO_NAME(TYPE,INFO,NAME)	\
	TYPE::infotype TYPE::sm_info(NAME);\

#define IMPLEMENT_OBJECT_INFO(TYPE,INFO)	IMPLEMENT_OBJECT_INFO_NAME(TYPE,INFO,#TYPE)


#define IMPLEMENT_OBJECT_INFO_NAME_T1(TYPE,INFO,NAME)	\
	template<typename T1> typename TYPE<T1>::infotype  TYPE<T1>::sm_info(vhwd::ObjectNameT<T1>::MakeName(NAME));


#define IMPLEMENT_OBJECT_INFO_NAME_T2(TYPE,INFO,NAME)	\
	template<typename T1,typename T2> typename TYPE<T1,T2>::infotype TYPE<T1,T2>::sm_info(vhwd::ObjectNameT<T2>::MakeName(vhwd::ObjectNameT<T1>::MakeName(NAME)));


#define IMPLEMENT_OBJECT_INFO_T1(TYPE,INFO)	IMPLEMENT_OBJECT_INFO_NAME_T1(TYPE,INFO,#TYPE)
#define IMPLEMENT_OBJECT_INFO_T2(TYPE,INFO)	IMPLEMENT_OBJECT_INFO_NAME_T2(TYPE,INFO,#TYPE)

VHWD_ENTER

class VHWD_DLLIMPEXP Object;
class VHWD_DLLIMPEXP Serializer;

class VHWD_DLLIMPEXP ObjectInfo : private NonCopyable
{
public:
	ObjectInfo(const String& s);
	virtual ~ObjectInfo();

	const String& GetObjectName() const;

	virtual Object* CreateObject()=0;

protected:
	String m_sClassName;
};


template<typename T,typename INFO=ObjectInfo>
class ObjectInfoT : public INFO
{
public:
	typedef INFO basetype;
	ObjectInfoT(const String &name):basetype(name) {}

	Object *CreateObject()
	{
		return new T();
	}
};


class VHWD_DLLIMPEXP Object
{
public:

	virtual ~Object() {}
	virtual bool Serialize(Serializer& ar);

	DECLARE_OBJECT_INFO(Object,ObjectInfo)
};

class VHWD_DLLIMPEXP ObjectData : public Object
{
public:

	ObjectData();
	ObjectData(const ObjectData& o);

	ObjectData& operator=(const ObjectData&){return *this;}
	~ObjectData();

	// Increase reference counter,
	inline void IncRef(){m_refcount++;}

	// Decrease reference counter,
	inline void DecRef()
	{
		wassert(m_refcount.get()>0);
		if(--m_refcount==0)
		{
			delete this;
		}
	}


	// Get reference count.
	int GetRef() const;

	// Increase reference count of p2, decrease reference count of p1
	static void locked_reset(ObjectData*&p1,ObjectData* p2)
	{
		if(p1==p2) return;
		if(p2) p2->IncRef();
		if(p1) p1->DecRef();
		p1=p2;
	}

	static void locked_equal(ObjectData*&p1,ObjectData*&p2)
	{
		if(p1==p2) return;
		if(p2) p2->IncRef();
		if(p1) p1->DecRef();
		p1=p2;
	}

	template<typename T>
	static typename tl::enable_if<tl::is_convertible<T,ObjectData>,void>::type locked_reset(T*& p1,T* p2)
	{
		locked_reset(reinterpret_cast<ObjectData*&>(p1),p2);
	}

	template<typename T>
	static typename tl::enable_if<tl::is_convertible<T,ObjectData>,void>::type locked_equal(T*& p1,T*& p2)
	{
		locked_reset(reinterpret_cast<ObjectData*&>(p1),reinterpret_cast<ObjectData*&>(p2));
	}


	DECLARE_OBJECT_INFO(ObjectData,ObjectInfo)

protected:

	//virtual void on_destroy() {}
	//virtual void on_created() {}

	AtomicInt32 m_refcount;
};


// ObjectName is used to generate template objects' names.

DEFINE_OBJECT_NAME(String,"str");
DEFINE_OBJECT_NAME(bool,"i01");
DEFINE_OBJECT_NAME(char,"i08");
DEFINE_OBJECT_NAME(wchar_t,"wch");
DEFINE_OBJECT_NAME(int32_t,"i32");
DEFINE_OBJECT_NAME(int64_t,"i64");
DEFINE_OBJECT_NAME(uint32_t,"u32");
DEFINE_OBJECT_NAME(uint64_t,"u64");
DEFINE_OBJECT_NAME(float32_t,"f32");
DEFINE_OBJECT_NAME(float64_t,"f64");

VHWD_LEAVE

#endif
