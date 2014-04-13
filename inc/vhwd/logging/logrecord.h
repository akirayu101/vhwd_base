#ifndef __H_VHWD_LOG_RECORD__
#define __H_VHWD_LOG_RECORD__

#include "vhwd/basic/clock.h"
#include "vhwd/basic/string.h"

VHWD_ENTER

enum
{
	LOGLEVEL_DEBUG,
	LOGLEVEL_PRINT,
	LOGLEVEL_INFO,
	LOGLEVEL_MESSAGE,
	LOGLEVEL_USER,
	LOGLEVEL_WARNING=20,
	LOGLEVEL_ERROR,
	LOGLEVEL_FETAL,
	LOGLEVEL_MAX,
};


class VHWD_DLLIMPEXP LogRecord
{
public:
	LogRecord();
	LogRecord(const String& s,int src,int id,int lv);

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
