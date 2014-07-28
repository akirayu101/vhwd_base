#include "vhwd/basic/object.h"
#include "vhwd/collection/indexer_map.h"


VHWD_ENTER


ObjectInfo::ObjectInfo(const String& s)
	:m_sClassName(s)
{
	if(!m_sClassName.empty())
	{
		ObjectCreator::current().Register(this);
	}
}


class ObjectCreatorImpl : public ObjectData
{
public:

	ObjectCreatorImpl(int n)
	{
		kmap.rehash(n);
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


	indexer_map<String,ObjectInfo*> kmap;
};

void ObjectCreator::reset(ObjectData *p)
{
	ObjectData::locked_reset(impl,p);
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
		System::LogTrace("info=NULL in ObjectCreator::Register");
		return;
	}

	String name=info->GetObjectName();
	if(name.empty())
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
