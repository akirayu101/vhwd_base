// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_LOG_RECORD__
#define __H_VHWD_LOG_RECORD__

#include "vhwd/basic/clock.h"
#include "vhwd/basic/string.h"

VHWD_ENTER


class VHWD_DLLIMPEXP LogRecord
{
public:
	LogRecord();
	LogRecord(const String& s,int src,int id,int lv);
	LogRecord(int src,int id,int lv);

	// timepoint when record generated.
	TimePoint m_tStamp;

	// message description
	String m_sMessage;

	// source of this logrecord.
	int32_t m_nSrc;

	// thread id
	int32_t m_nId;

	// level of the logrecord
	int32_t m_nLevel;
};

VHWD_LEAVE
#endif
