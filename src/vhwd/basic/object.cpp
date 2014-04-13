#include "vhwd/basic/object.h"

VHWD_ENTER



bool Object::serialize(Serializer&)
{
	return true;
}

IMPLEMENT_OBJECT_INFO(Object,ObjectInfo)

VHWD_LEAVE
