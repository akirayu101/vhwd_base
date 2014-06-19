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
	virtual infobase* GetObjectInfo() const;\
	static  infotype sm_info;\
	virtual const String& GetObjectName() const {return TYPE::sm_info.GetObjectName();}\


#define DECLARE_OBJECT_INFO_ABSTRACT(TYPE,INFO)	\
public:\
	typedef INFO infobase;\
	typedef INFO infotype;\
	virtual infobase* GetObjectInfo() const;\
	static  infotype sm_info;\
	virtual const String& GetObjectName() const {return TYPE::sm_info.GetObjectName();}\


#define DECLARE_OBJECT_INFO_BASE(TYPE,INFO,BASE)	\
	DECLARE_OBJECT_INFO(TYPE,INFO)\
	typedef BASE basetype;\

#define DECLARE_OBJECT_INFO_BASE_ABSTRACT(TYPE,INFO,BASE)	\
	DECLARE_OBJECT_INFO_ABSTRACT(TYPE,INFO)\
	typedef BASE basetype;\

#define IMPLEMENT_OBJECT_INFO_NAME(TYPE,INFO,NAME)	\
	TYPE::infotype TYPE::sm_info(NAME);\
	TYPE::infobase* TYPE::GetObjectInfo() const {return &TYPE::sm_info;}


#define IMPLEMENT_OBJECT_INFO(TYPE,INFO)	IMPLEMENT_OBJECT_INFO_NAME(TYPE,INFO,#TYPE)

#define DECLARE_OBJECT_INFO_T(TYPE,INFO)	\
public:\
	typedef INFO infobase;\
	typedef vhwd::ObjectInfoT<TYPE,INFO> infotype;\
	virtual infobase* GetObjectInfo() const;\
	static  infotype sm_info;\
	virtual const String& GetObjectName() const {return TYPE::sm_info.GetObjectName();}\


#define IMPLEMENT_OBJECT_INFO_NAME_T(TYPE,INFO,NAME)	\
	template<typename T> typename TYPE::infotype TYPE::sm_info(vhwd::ObjectNameT<T>::MakeName(NAME));\
	template<typename T> typename TYPE::infobase* TYPE::GetObjectInfo() const {return &TYPE::sm_info;}

#define IMPLEMENT_OBJECT_INFO_T(TYPE,INFO)	IMPLEMENT_OBJECT_INFO_NAME_T(TYPE,INFO,#TYPE)

VHWD_ENTER

class VHWD_DLLIMPEXP Object;
class VHWD_DLLIMPEXP Serializer;

class VHWD_DLLIMPEXP ObjectInfo : private NonCopyable
{
public:
	ObjectInfo(const String& s);
	virtual ~ObjectInfo(){}

	const String& GetObjectName() const {return m_sClassName;}
	virtual Object* CreateObject()=0;

protected:
	String m_sClassName;
};


template<typename T,typename INFO=ObjectInfo>
class ObjectInfoT : public INFO
{
public:
	typedef INFO basetype;
	ObjectInfoT(const String &name):basetype(name){}

	Object *CreateObject()
	{
		return new T();
	}
};


class VHWD_DLLIMPEXP Object
{
public:

	virtual ~Object(){}
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
	// Call on_created if origin reference count is 0.
	void IncRef();

	// Decrease reference counter,
	// Call on_destroy and delete this if new reference is 0.
	void DecRef();

	// Get reference count.
	int GetRef() const;

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

	virtual void on_destroy(){}
	virtual void on_created(){}

	AtomicInt32 m_refcount;
};

class VHWD_DLLIMPEXP ObjectGroup
{
public:
	ObjectGroup();
	ObjectGroup(const ObjectGroup&);
	ObjectGroup& operator=(const ObjectGroup&);
	~ObjectGroup();

	ObjectData* operator[](size_t n);
	size_t size() const;

	void append(ObjectData*);
	void remove(ObjectData*);

	void clear();

	void swap(ObjectGroup& o);

private:

	void* impl;
};

template<typename T,typename B>
class ObjectGroupT : public B
{
public:

	T* operator[](size_t n)
	{
		return static_cast<T*>(impl[n]);
	}

	size_t size() const
	{
		return impl.size();
	}

	void append(T* d)
	{
		impl.append(d);
	}

	void remove(T* d)
	{
		impl.remove(d);
	}

	void clear()
	{
		impl.clear();
	}

	void swap(ObjectGroupT& o)
	{
		impl.swap(o.impl);
	}

private:
	ObjectGroup impl;
};

class VHWD_DLLIMPEXP ObjectCreator
{
public:

	ObjectCreator();
	ObjectCreator(const ObjectCreator& o);
	ObjectCreator& operator=(const ObjectCreator& o);

	~ObjectCreator();


	// Create an Object using Object name.
	Object* Create(const String& name);

	// Register ObjectInfo.
	void Register(ObjectInfo* info);

	// GetObjectInfo by name.
	ObjectInfo* GetInfo(const String& name);

	// Default ObjectCreator
	static ObjectCreator& current();

private:
	void reset(ObjectData* p);
	ObjectData* impl;
};


// ObjectName is used to generate template objects' names.

DEFINE_OBJECT_NAME(String,"str");
DEFINE_OBJECT_NAME(bool,"i01");
DEFINE_OBJECT_NAME(char,"i08");
DEFINE_OBJECT_NAME(int32_t,"i32");
DEFINE_OBJECT_NAME(int64_t,"i64");
DEFINE_OBJECT_NAME(uint32_t,"u32");
DEFINE_OBJECT_NAME(uint64_t,"u64");
DEFINE_OBJECT_NAME(float32_t,"f32");
DEFINE_OBJECT_NAME(float64_t,"f64");

VHWD_LEAVE

#endif
