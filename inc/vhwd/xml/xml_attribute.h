#ifndef __H_VHWD_XML_ATTRIBUTE__
#define __H_VHWD_XML_ATTRIBUTE__


#include "vhwd/basic/object.h"
#include "vhwd/basic/pointer.h"
#include "vhwd/collection/indexer_set.h"
#include <fstream>

VHWD_ENTER


class VHWD_DLLIMPEXP XmlAttribute
{
public:
	friend class XmlParser;

	XmlAttribute();
	XmlAttribute(const String& n,const String& v=String());

	const String& GetName() const {return m_sName;}
	const String& GetValue() const {return m_sValue;}
	void SetName(const String& v){m_sName=v;}
	void SetValue(const String& v){m_sValue=v;}

protected:
	String m_sName;
	String m_sValue;

};


VHWD_LEAVE
#endif
