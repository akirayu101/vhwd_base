#include "vhwd/xml/xml_node.h"
#include "vhwd/logging/logger.h"
#include "vhwd/memory/mempool.h"
#include "xml_parser.h"

VHWD_ENTER


XmlNode::XmlNode()
{

}

XmlNode::XmlNode(const XmlNode& o)
	:m_sName(o.m_sName)
	,m_sContent(o.m_sContent)
	,m_aAttribute(o.m_aAttribute)
{

}

XmlNode::XmlNode(const String& tag,const String& val)
	:m_sName(tag)
	,m_sContent(val)
{

}

void XmlNode::InsertChild(XmlNode* p)
{
	if(!p) return;
	p->m_pNextSibling.reset(m_pFirstChild.get());
	m_pFirstChild.reset(p);
}

void XmlNode::AppendChild(XmlNode* p)
{
	if(!m_pFirstChild)
	{
		m_pFirstChild.reset(p);
	}
	else
	{
		XmlNode* pLast=m_pFirstChild.get();
		while(pLast->m_pNextSibling)
		{
			pLast=pLast->m_pNextSibling.get();
		}
		pLast->m_pNextSibling.reset(p);
	}
}

bool XmlNode::RemoveChild(XmlNode* p)
{
	if(!p) return false;
	if(p==m_pFirstChild.get())
	{
		m_pFirstChild.reset(m_pFirstChild->GetNext());
		delete p;
		return true;
	}

	XmlNode* prev=m_pFirstChild.get();
	while(prev && prev->GetNext()!=p)
	{
		prev=prev->GetNext();
	}

	if(prev)
	{
		prev->m_pNextSibling.reset(p->GetNext());
		delete p;
		return true;
	}

	return false;

}

void XmlNode::DeleteChildren()
{
	while(m_pFirstChild)
	{
		XmlNode* g=m_pFirstChild.get();
		m_pFirstChild.reset(m_pFirstChild->GetNext());
		delete g;
	}
}

XmlAttribute& XmlNode::GetAttribute(const String& n)
{
	for(size_t i=0;i<m_aAttribute.size();i++)
	{
		if(m_aAttribute[i].GetName()==n)
		{
			return m_aAttribute[i];
		}
	}

	return AddAttribute(n,"");

}

bool XmlNode::GetAttribute(const String& n,String* v)
{
	if(!v) return false;
	for(size_t i=0;i<m_aAttribute.size();i++)
	{
		if(m_aAttribute[i].GetName()==n)
		{
			*v=m_aAttribute[i].GetValue();
			return true;
		}
	}
	return false;
}

XmlNode::~XmlNode()
{		
	DeleteChildren();
}

XmlAttribute& XmlNode::AddAttribute(const String& v1,const String& v2)
{
	m_aAttribute.push_back(XmlAttribute(v1,v2));
	return m_aAttribute.back();
}

void XmlNode::DeleteAttribute(const String& n)
{
	size_t j=m_aAttribute.size();

	for(size_t i=0;i<j;)
	{
		if(m_aAttribute[i].GetName()==n)
		{
			std::swap(m_aAttribute[i],m_aAttribute[--j]);
		}
		else
		{
			i++;
		}
	}

	m_aAttribute.resize(j);
}


VHWD_LEAVE