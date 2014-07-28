#include "vhwd/xml/xml_node.h"
#include "vhwd/logging/logger.h"
#include "vhwd/serialization/serializer.h"

#include "xml_parser.h"

VHWD_ENTER



XmlNode::XmlNode(const XmlNode& o)
	:XmlBase(o)
	,m_nNodeType(o.m_nNodeType)
{

}

XmlNode::XmlNode(const String& tag,const String& val)
	:XmlBase(tag,val)
	,m_nNodeType(XMLNODE_ELEMENT)
{

}

XmlNode::~XmlNode()
{
	DeleteChildren();
	DeleteAttributes();
}

void XmlNode::EnsureChildrenParent()
{
	for(XmlNode* pnode=GetFirstChild(); pnode!=NULL; pnode=pnode->GetNext())
	{
		pnode->SetParent(this);
	}
}

void XmlNode::swap(XmlNode& o)
{
	m_sName.swap(o.m_sName);
	m_sValue.swap(o.m_sValue);
	std::swap(m_nNodeType,o.m_nNodeType);
	listNodes.swap(o.listNodes);
	listAttrs.swap(o.listAttrs);
	EnsureChildrenParent();
	o.EnsureChildrenParent();
}

bool XmlNode::Serialize(Serializer& ar)
{
	ar & m_sName & m_sValue & m_nNodeType;

	int32_t nChilds=listNodes.size();
	ar & nChilds;
	if(ar.is_reader())
	{
		DeleteChildren();
		for(int32_t i=0; i<nChilds; i++)
		{
			AutoPtrT<XmlNode> pnode(new XmlNode);
			if(!pnode->Serialize(ar))
			{
				return false;
			}
			AppendChild(pnode.release());
		}

	}
	else
	{
		for(XmlNode* pnode=GetFirstChild(); pnode!=NULL; pnode=pnode->GetNext())
		{
			if(!pnode->Serialize(ar))
			{
				return false;
			}
		}
	}

	int32_t nAttrs=listAttrs.size();
	ar & nAttrs;
	if(ar.is_reader())
	{
		DeleteAttributes();
		for(int32_t i=0; i<nAttrs; i++)
		{
			AutoPtrT<XmlAttribute> pattr(new XmlAttribute);
			if(!pattr->Serialize(ar))
			{
				return false;
			}
			AppendAttribute(pattr.release());
		}
	}
	else
	{
		for(XmlAttribute* pattr=GetFirstAttribute(); pattr!=NULL; pattr=pattr->GetNext())
		{
			if(!pattr->Serialize(ar))
			{
				return false;
			}
		}
	}

	return ar.good();
}


VHWD_LEAVE
