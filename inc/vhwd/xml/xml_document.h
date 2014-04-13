#ifndef __H_VHWD_XML_DOCUMENT__
#define __H_VHWD_XML_DOCUMENT__

#include "vhwd/xml/xml_node.h"

VHWD_ENTER


class VHWD_DLLIMPEXP XmlDocument : public Object, private NonCopyable
{
public:
	friend class XmlParser;

	DECLARE_OBJECT_INFO(XmlDocument,ObjectInfo)

	XmlDocument();
	~XmlDocument();

	bool LoadXml(const String& s);
	bool SaveXml(const String& s);

	operator bool() const {return m_pRoot;}
	XmlNode* GetRoot(){return m_pRoot.get();}
	XmlNode* Release(){return m_pRoot.release();}

	void SetRoot(XmlNode* p){m_pRoot.reset(p);}
	void Clear(){m_pRoot.reset(NULL);}

protected:

	AutoPtrT<XmlNode> m_pRoot;
	arr_1t<String> m_aDecl;
};



VHWD_LEAVE
#endif
