#ifndef __H_VHWD_XML_NODE__
#define __H_VHWD_XML_NODE__

#include "vhwd/xml/xml_attribute.h"
#include "vhwd/basic/object.h"
#include "vhwd/basic/pointer.h"
#include "vhwd/collection/indexer_set.h"
#include <fstream>

VHWD_ENTER


class XmlParser;


class VHWD_DLLIMPEXP XmlNode
{
public:
	friend class XmlParser;

	XmlNode();
	XmlNode(const XmlNode& o);
	XmlNode(const String& tag,const String& val="");
	~XmlNode();

	XmlNode* GetChildren(){return m_pFirstChild.get();}
	XmlNode* GetNext(){return m_pNextSibling.get();}
	const String& GetName() const {return m_sName;}
	const String& GetContent() const {return m_sContent;}
	void SetName(const String& s){m_sName=s;}
	void SetContent(const String& s){m_sContent=s;}

	void InsertChild(XmlNode* p);
	void AppendChild(XmlNode* p);
	bool RemoveChild(XmlNode* p);
	void DeleteChildren();

	size_t GetAttributeCount() const {return m_aAttribute.size();}
	XmlAttribute& AddAttribute(const String& name,const String& value);
	void DeleteAttribute(const String& n);

	XmlAttribute& GetAttribute(size_t n){return m_aAttribute[n];}
	XmlAttribute& GetAttribute(const String& n);
	bool GetAttribute(const String& n,String* v);

	const XmlAttribute& GetAttribute(size_t n) const {return m_aAttribute[n];}

protected:

	LitePtrT<XmlNode> m_pFirstChild;
	LitePtrT<XmlNode> m_pNextSibling;
	String m_sName;
	String m_sContent;
	arr_1t<XmlAttribute> m_aAttribute;
};


VHWD_LEAVE
#endif
