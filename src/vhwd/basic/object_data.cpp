#include "vhwd/basic/object.h"
#include "vhwd/basic/object_ex.h"
#include "vhwd/collection/arr_1t.h"

VHWD_ENTER


int ObjectData::GetRef() const
{
	return m_refcount.get();
}

ObjectData::ObjectData()
{

}

ObjectData::ObjectData(const ObjectData&)
{

}

ObjectData::~ObjectData()
{
	if(m_refcount.get()!=0)
	{
		System::LogTrace("m_refcount==%d while ObjectData destruct",m_refcount.get());
	}
}


ObjectGroup::ObjectGroup()
{
	
}

ObjectGroup::ObjectGroup(const ObjectGroup& o):impl(o.impl)
{
	for(size_t i=0;i<impl.size();i++)
	{
		impl[i]->IncRef();
	}
}

ObjectGroup& ObjectGroup::operator=(const ObjectGroup& o)
{
	if(this==&o) return *this;
	ObjectGroup(o).swap(*this);
	return *this;
}

ObjectGroup::~ObjectGroup()
{
	clear();
}

ObjectData* ObjectGroup::operator[](size_t n)
{
	if(n>size()) return NULL;
	return impl[n];
}

size_t ObjectGroup::size() const
{
	return impl.size();
}

void ObjectGroup::append(ObjectData* d)
{
	if(!d) return;
	impl.push_back(d);
	d->IncRef();
}

void ObjectGroup::remove(ObjectData* d)
{
	for(size_t i=0;i<impl.size();i++)
	{
		if(impl[i]==d)
		{
			std::swap(impl[i],impl.back());
			impl.pop_back();
			d->DecRef();
			return;
		}
	}
}

void ObjectGroup::clear()
{
	for(size_t i=0;i<impl.size();i++)
	{
		impl[i]->DecRef();
	}
	impl.clear();
}

void ObjectGroup::swap(ObjectGroup& o)
{
	impl.swap(o.impl);
}

IMPLEMENT_OBJECT_INFO(ObjectData,ObjectInfo)

VHWD_LEAVE
