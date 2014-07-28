// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_XML_ATTRIBUTE__
#define __H_VHWD_XML_ATTRIBUTE__

#include "vhwd/xml/xml_base.h"

VHWD_ENTER


class VHWD_DLLIMPEXP XmlAttribute : public XmlBase
{
public:
	friend class XmlParser;
	friend class XmlNode;

	XmlAttribute() {}
	XmlAttribute(const String& n,const String& v=String());

	XmlAttribute* GetNext()
	{
		return m_pNextSibling.get();
	}
	const XmlAttribute* GetNext() const
	{
		return m_pNextSibling.get();
	}

	bool Serialize(Serializer& ar);

protected:
	LitePtrT<XmlAttribute> m_pNextSibling;

};


VHWD_LEAVE
#endif
