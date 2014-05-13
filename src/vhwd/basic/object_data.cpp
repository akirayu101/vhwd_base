#include "vhwd/basic/object.h"
#include "vhwd/threading/thread.h"


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

IMPLEMENT_OBJECT_INFO(ObjectData,ObjectInfo)

VHWD_LEAVE
