// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_BASIC_OBJECT_EX__
#define __H_VHWD_BASIC_OBJECT_EX__

#include "vhwd/basic/object.h"
#include "vhwd/basic/pointer.h"
#include "vhwd/collection/array.h"

VHWD_ENTER


class VHWD_DLLIMPEXP ObjectGroup
{
	typedef arr_1t<ObjectData*> impl_type;
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
	impl_type impl;
};

template<typename T,typename B=Object>
class ObjectGroupT : public B
{
	typedef arr_1t<DataPtrT<T> > impl_proxy;
	typedef DataPtrT<T> value_proxy;

public:

	value_proxy& operator[](size_t n)
	{
		return __proxy()[n];
	}

	const value_proxy& operator[](size_t n) const
	{
		return __proxy()[n];		
	}

	size_t size() const
	{
		return impl.size();
	}

	void append(T* d)
	{
		impl.append(d);
	}

	template<typename T2>
	void append(DataPtrT<T2> p)
	{
		append(p.get());
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

	typedef typename impl_proxy::iterator iterator;
	typedef typename impl_proxy::reverse_iterator reverse_iterator;
	typedef typename impl_proxy::const_iterator const_iterator;
	typedef typename impl_proxy::const_reverse_iterator const_reverse_iterator;

	iterator begin(){return __proxy().begin();}
	iterator end(){return __proxy().end();}
	reverse_iterator rbegin(){return __proxy().rbegin();}
	reverse_iterator rend(){return __proxy().rend();}
	const_iterator begin() const {return __proxy().begin();}
	const_iterator end() const {return __proxy().end();}
	const_reverse_iterator rbegin() const {return __proxy().rbegin();}
	const_reverse_iterator rend() const {return __proxy().rend();}

protected:

	impl_proxy& __proxy(){return *(impl_proxy*)&impl;}
	const impl_proxy& __proxy() const {return *(impl_proxy*)&impl;}

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



VHWD_LEAVE
#endif

