#include "vhwd/xml/xml_document.h"
#include "vhwd/logging/logger.h"
#include "vhwd/memory/mempool.h"
#include "xml_parser.h"

VHWD_ENTER

bool XmlDocument::LoadXml(const String& s)
{
	XmlParser parser;
	if(!parser.LoadXml(s))
	{
		return false;
	}
	//this_logger().LogMessage("load file done");
	bool flag=parser.parse(*this);
	return flag;
}



bool XmlDocument::SaveXml(const String& s)
{
	XmlParser parser;
	return parser.SaveXml(*this,s);
}

XmlDocument::XmlDocument()
{

}

XmlDocument::~XmlDocument()
{

}

IMPLEMENT_OBJECT_INFO(XmlDocument,ObjectInfo)

VHWD_LEAVE
