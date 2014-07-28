#include "vhwd/basic/object.h"

VHWD_ENTER

bool Object::Serialize(Serializer&)
{
	return true;
}

IMPLEMENT_OBJECT_INFO(Object,ObjectInfo)

VHWD_LEAVE
