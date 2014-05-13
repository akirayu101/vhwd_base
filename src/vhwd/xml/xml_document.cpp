#include "vhwd/xml/xml_document.h"
#include "vhwd/logging/logger.h"
#include "vhwd/serialization/serializer_buffer.h"

#include "xml_parser.h"

VHWD_ENTER

XmlDocument::XmlDocument(const XmlDocument& o)
{
	(*this)=o;
}

const XmlDocument& XmlDocument::operator=(const XmlDocument& src)
{
	XmlDocument dst;

	SerializerBuffer sbuf;
	sbuf.writer() & src;
	sbuf.reader() & dst;
	if(!sbuf.skip())
	{
		System::LogError("XmlDocument::operator= failed!");
	}
	swap(dst);
	return *this;
}

bool XmlDocument::LoadXml(const String& s)
{
	XmlParser parser(*this);
	return parser.load(s);
}

bool XmlDocument::LoadStr(const char* pstr_,size_t size_)
{
	XmlParser parser(*this);
	return parser.load(pstr_,size_);
}

bool XmlDocument::LoadStr(const char* pstr_)
{
	return LoadStr(pstr_,::strlen(pstr_));
}


bool XmlDocument::SaveXml(const String& s)
{
	XmlParser parser(*this);
	return parser.save(s);
}

XmlDocument::XmlDocument():XmlNode(XmlNode::XMLNODE_ROOT)
{

}

XmlDocument::~XmlDocument()
{

}

VHWD_LEAVE
