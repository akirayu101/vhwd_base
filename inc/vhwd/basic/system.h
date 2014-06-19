// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_BASIC_SYSTEM__
#define __H_VHWD_BASIC_SYSTEM__

#include "vhwd/config.h"
#include "vhwd/basic/string_detail.h"

VHWD_ENTER

class VHWD_DLLIMPEXP System
{
public:

	static bool Execute(const String& s);

	static const String& GetModulePath();

	static int GetCacheLineSize();
	static int GetCpuCount();
	static int GetPageSize();

	static int GetPid();

	static double GetCpuTime();

	// Debug break.
	static void DebugBreak();

	// Exit process with return code v.
	static void Exit(int v);

	static void CheckError(const String& s);


	#define STRING_FORMAT_LEVEL(X,Y) DoLogImpl(Y,X)

#ifndef NDEBUG
	STRING_FORMAT_FUNCTIONS(static void LogDeubg,STRING_FORMAT_LEVEL,LOGLEVEL_DEBUG)
#else
	static void LogDebug(...){}
#endif

	STRING_FORMAT_FUNCTIONS(static void LogInfo,STRING_FORMAT_LEVEL,LOGLEVEL_INFO)
	STRING_FORMAT_FUNCTIONS(static void LogTrace,STRING_FORMAT_LEVEL,LOGLEVEL_TRACE)
	STRING_FORMAT_FUNCTIONS(static void Printf,STRING_FORMAT_LEVEL,LOGLEVEL_PRINT)
	STRING_FORMAT_FUNCTIONS(static void LogMessage,STRING_FORMAT_LEVEL,LOGLEVEL_MESSAGE)
	STRING_FORMAT_FUNCTIONS(static void LogWarning,STRING_FORMAT_LEVEL,LOGLEVEL_WARNING)
	STRING_FORMAT_FUNCTIONS(static void LogError,STRING_FORMAT_LEVEL,LOGLEVEL_ERROR)
	STRING_FORMAT_FUNCTIONS(static void LogFetal,STRING_FORMAT_LEVEL,LOGLEVEL_FETAL)
	#undef STRING_FORMAT_LEVEL

	static bool SetLogFile(const String& fn,bool app=true);

private:
	static void DoLogImpl(int v,const char* msg,...);	
};

VHWD_LEAVE
#endif
