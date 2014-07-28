#include "vhwd/basic/object.h"
#include "vhwd/collection/array.h"

VHWD_ENTER

void ObjectData::IncRef()
{
	if(m_refcount++==0)
	{
		on_created();
	}
}

void ObjectData::DecRef()
{
	wassert(m_refcount.get()>0);

	if(--m_refcount==0)
	{
		on_destroy();
		delete this;
	}
}

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


typedef arr_1t<ObjectData*> ObjectGroupImpl;

ObjectGroup::ObjectGroup()
{
	impl=NULL;
}

ObjectGroup::ObjectGroup(const ObjectGroup& o)
{
	ObjectGroupImpl* p1=(ObjectGroupImpl*)o.impl;
	ObjectGroupImpl* p2=NULL;
	if(p1)
	{
		p2=new ObjectGroupImpl(*p1);
		for(size_t i=0; i<p2->size(); i++)
		{
			if((*p2)[i]) (*p2)[i]->IncRef();
		}
	}
	impl=p2;
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
	return (*(ObjectGroupImpl*)impl)[n];
}

size_t ObjectGroup::size() const
{
	if(!impl) return 0;
	return (*(ObjectGroupImpl*)impl).size();
}

void ObjectGroup::append(ObjectData* d)
{
	if(!d) return;

	if(!impl) impl=new ObjectGroupImpl;
	(*(ObjectGroupImpl*)impl).push_back(d);
	d->IncRef();
}

void ObjectGroup::remove(ObjectData* d)
{
	if(!impl)
	{
		return;
	}
	ObjectGroupImpl& grp(*(ObjectGroupImpl*)impl);
	size_t i=0;
	size_t n=grp.size();

	for(; i<n; i++)
	{
		if(grp[i]==d) break;
	}

	if(i==n)
	{
		return;
	}

	for(; i<n-1; i++)
	{
		grp[i]=grp[i+1];
	}

	d->DecRef();
	grp.pop_back();


}

void ObjectGroup::clear()
{
	if(!impl) return;
	ObjectGroupImpl& grp(*(ObjectGroupImpl*)impl);
	for(size_t i=0; i<grp.size(); i++)
	{
		grp[i]->DecRef();
	}
	grp.clear();
	delete (ObjectGroupImpl*)impl;
}

void ObjectGroup::swap(ObjectGroup& o)
{
	std::swap<void*>(impl,o.impl);
}

IMPLEMENT_OBJECT_INFO(ObjectData,ObjectInfo)

VHWD_LEAVE
