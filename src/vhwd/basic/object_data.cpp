#include "vhwd/basic/object.h"
#include "vhwd/threading/thread.h"


VHWD_ENTER

Mutex& object_data_mutex()
{
	static StaticObjectWithoutDeletorT<Mutex> spin;
	return spin;
}


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
	return m_refcount.load();
}

ObjectData::ObjectData():m_refcount(0)
{

}

ObjectData::ObjectData(const ObjectData&):m_refcount(0)
{

}

ObjectData::~ObjectData()
{

}

IMPLEMENT_OBJECT_INFO(ObjectData,ObjectInfo)

VHWD_LEAVE
