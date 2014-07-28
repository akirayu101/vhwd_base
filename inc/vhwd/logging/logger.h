// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_LOG_LOGGING__
#define __H_VHWD_LOG_LOGGING__

#include "vhwd/logging/logrecord.h"
#include "vhwd/basic/object.h"
VHWD_ENTER

class VHWD_DLLIMPEXP LogTarget;
class VHWD_DLLIMPEXP LoggerImpl;

class VHWD_DLLIMPEXP Logger : public Object
{
public:

	enum
	{
		MSG_IF_ANY,
		MSG_IF_WARNING,
		MSG_IF_ERROR,
		MSG_NEVER,
	};

	Logger(int src=0,int id=0);
	Logger(const Logger& o);
	Logger& operator=(const Logger& o);

	~Logger();

	void swap(Logger& logger)
	{
		std::swap(impl,logger.impl);
	}

	// set/get the default logtarget.
	static void def(LogTarget* p);
	static LogTarget* def();

	// set/get logtarget.
	void reset(LogTarget* p);
	LogTarget* get();

	// set/get source
	void Src(int src);
	int Src();

	// set/get id
	int Id();
	void Id(int id);

	// cache mode, LogRecords will not be send to logtarget immediately.
	bool Cache();
	void Cache(bool cache);

	// handle the logrecord, just send to the logtarget.
	void Handle(LogRecord& o);

	// flush cached logrecords to logtarget, return Ok() and Clear();
	bool Test(int t=MSG_IF_ANY);

	//return true if no error logrecords.
	bool Ok();

	//clear error
	void Clear();

	//#define STRING_FORMAT_LEVEL(X,Y) LogRecord rcd(String::Format(X),Src(),Id(),Y);Handle(rcd)
#define STRING_FORMAT_LEVEL(X,Y) DoLogImpl(Y,X)

#ifndef NDEBUG
	STRING_FORMAT_FUNCTIONS(void LogDeubg,STRING_FORMAT_LEVEL,LOGLEVEL_DEBUG)
#else
	inline void LogDebug(...) {}
#endif

	STRING_FORMAT_FUNCTIONS(void LogInfo,STRING_FORMAT_LEVEL,LOGLEVEL_INFO)
	STRING_FORMAT_FUNCTIONS(void LogTrace,STRING_FORMAT_LEVEL,LOGLEVEL_TRACE)
	STRING_FORMAT_FUNCTIONS(void Printf,STRING_FORMAT_LEVEL,LOGLEVEL_PRINT)
	STRING_FORMAT_FUNCTIONS(void LogMessage,STRING_FORMAT_LEVEL,LOGLEVEL_MESSAGE)
	STRING_FORMAT_FUNCTIONS(void LogWarning,STRING_FORMAT_LEVEL,LOGLEVEL_WARNING)
	STRING_FORMAT_FUNCTIONS(void LogError,STRING_FORMAT_LEVEL,LOGLEVEL_ERROR)
	STRING_FORMAT_FUNCTIONS(void LogFetal,STRING_FORMAT_LEVEL,LOGLEVEL_FETAL)
#undef STRING_FORMAT_LEVEL


protected:

	void DoLogImpl(int lv,const char* msg,...);
	void DoLogImplV(int v,const char* msg,va_list vl);

	LoggerImpl* impl;
};

// get thread private logger;
VHWD_DLLIMPEXP Logger& this_logger();



// LoggerSwap, swap logger with this_logger().
class VHWD_DLLIMPEXP LoggerSwap : public Logger, private NonCopyable
{
public:
	LoggerSwap();
	LoggerSwap(Logger& logger);
	~LoggerSwap();
};

VHWD_LEAVE
#endif
