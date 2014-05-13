#include "vhwd/xml/xml_attribute.h"
#include "vhwd/serialization/serializer.h"

VHWD_ENTER

XmlAttribute::XmlAttribute(const String& n,const String& v)
	:XmlBase(n,v)
{

}

bool XmlAttribute::Serialize(Serializer& ar)
{
	ar & m_sName & m_sValue;
	return ar.good();
}


VHWD_LEAVE
