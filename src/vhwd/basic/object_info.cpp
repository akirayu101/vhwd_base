#include "vhwd/basic/object.h"
#include "vhwd/collection/indexer_map.h"

VHWD_ENTER


ObjectInfo::ObjectInfo(const String& s)
	:m_sClassName(s)
{
	if(m_sClassName!="")
	{
		ObjectCreator::current().Register(this);
	}
}


class ObjectCreatorImpl : public ObjectData
{
public:
	ObjectCreatorImpl(int capacity)
	{
		kmap.set_capacity(capacity);
	}

	Object *Create(const String &name)
	{
		ObjectInfo *info=kmap[name];
		if(!info)
		{
			return NULL;
		}
		return info->CreateObject();
	}


	indexer_map<String,ObjectInfo *> kmap;
};

void ObjectCreator::reset(ObjectData *p)
{
	if(p==impl)
	{
		return;
	}
	if(impl)
	{
		impl->DecRef();
	}
	impl=(ObjectData*)p;
	if(impl)
	{
		impl->IncRef();
	}
}

Object *ObjectCreator::Create(const String &name)
{
	return ((ObjectCreatorImpl *)impl)->Create(name);
}

ObjectCreator::ObjectCreator()
{
	impl=NULL;
	reset(new ObjectCreatorImpl(1024));
}

ObjectCreator::ObjectCreator(const ObjectCreator &o)
{
	impl=NULL;
	reset(o.impl);
}

ObjectCreator &ObjectCreator::operator=(const ObjectCreator &o)
{
	reset(o.impl);
	return *this;
}

void ObjectCreator::Register(ObjectInfo *info)
{
	if(!info)
	{
		return;
	}
	String name=info->GetObjectName();
	if(name=="")
	{
		return;
	}
	((ObjectCreatorImpl *)impl)->kmap[name]=info;
}

ObjectCreator::~ObjectCreator()
{
	reset(NULL);
}


ObjectInfo *ObjectCreator::GetInfo(const String &name)
{
	return ((ObjectCreatorImpl *)impl)->kmap[name];
}


ObjectCreator& ObjectCreator::current()
{
	static ObjectCreator g_object_creator;
	return g_object_creator;
}


VHWD_LEAVE
