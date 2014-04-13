#include "vhwd/xml/xml_attribute.h"


VHWD_ENTER

XmlAttribute::XmlAttribute()
{

}

XmlAttribute::XmlAttribute(const String& n,const String& v)
	:m_sName(n),m_sValue(v)
{

}

VHWD_LEAVE
