#include "vhwd/xml/xml_document.h"
#include "vhwd/logging/logger.h"
#include "vhwd/serialization/serializer_buffer.h"

#include "xml_parser.h"

VHWD_ENTER


XmlDocument::XmlDocument():XmlNode(XmlNode::XMLNODE_ROOT)
{

}

XmlDocument::~XmlDocument()
{

}

XmlDocument::XmlDocument(const XmlDocument& o):XmlNode(o)
{
	XmlNode* node=xml_copy_recursive(&o);
	listNodes.swap(node->listNodes);
	listAttrs.swap(node->listAttrs);
	xml_ensure_children_parent();
	delete node;
}

const XmlDocument& XmlDocument::operator=(const XmlDocument& src)
{
	XmlDocument(src).swap(*this);
	return *this;
}

bool XmlDocument::LoadXml(const String& s)
{
	XmlParser parser(*this);
	return parser.LoadXml(s,FILE_TEXT);
}

bool XmlDocument::LoadStr(const char* pstr_,size_t size_)
{
	XmlParser parser(*this);
	return parser.LoadStr(pstr_,size_);
}

bool XmlDocument::LoadStr(const char* pstr_)
{
	return LoadStr(pstr_,::strlen(pstr_));
}


bool XmlDocument::SaveXml(const String& s)
{
	XmlParser parser(*this);
	return parser.SaveXml(s);
}

VHWD_LEAVE
