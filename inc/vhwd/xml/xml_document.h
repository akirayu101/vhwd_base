// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_XML_DOCUMENT__
#define __H_VHWD_XML_DOCUMENT__

#include "vhwd/xml/xml_node.h"
#include "vhwd/collection/array.h"

VHWD_ENTER


class VHWD_DLLIMPEXP XmlDocument : public XmlNode
{
public:
	friend class XmlParser;

	XmlDocument();

	// deep copy
	XmlDocument(const XmlDocument& o);
	const XmlDocument& operator=(const XmlDocument& o);

	~XmlDocument();

	bool LoadXml(const String& s);
	bool SaveXml(const String& s);
	bool LoadStr(const char* pstr_,size_t size_);
	bool LoadStr(const char* pstr_);

};


VHWD_LEAVE
#endif
