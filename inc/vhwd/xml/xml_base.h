// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.


#ifndef __H_VHWD_XML_BASE__
#define __H_VHWD_XML_BASE__


#include "vhwd/basic/object.h"
#include "vhwd/basic/pointer.h"
#include "vhwd/memory/mempool.h"

VHWD_ENTER


class VHWD_DLLIMPEXP XmlAttribute;
class VHWD_DLLIMPEXP XmlNode;
class VHWD_DLLIMPEXP XmlDocument;
class VHWD_DLLIMPEXP XmlParser;

class VHWD_DLLIMPEXP XmlBase : public SmallObject
{
public:

	XmlBase() {}
	XmlBase(const String& name_,const String& value_=""):m_sName(name_),m_sValue(value_) {}

	const String& GetName() const
	{
		return m_sName;
	}
	const String& GetValue() const
	{
		return m_sValue;
	}

	void SetName(const String& s)
	{
		m_sName=s;
	}
	void SetValue(const String& s)
	{
		m_sValue=s;
	}

protected:
	String m_sName;
	String m_sValue;
};

VHWD_LEAVE
#endif
