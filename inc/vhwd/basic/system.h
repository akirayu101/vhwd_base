#ifndef __H_VHWD_BASIC_SYSTEM__
#define __H_VHWD_BASIC_SYSTEM__

#include "vhwd/config.h"
#include "vhwd/basic/string.h"
#include "vhwd/basic/string_detail.h"

VHWD_ENTER


class VHWD_DLLIMPEXP System
{
public:

	static int GetCacheLineSize();
	static int GetCpuCount();
	static int GetPageSize();

	static int GetPid();

	static double GetCpuTime();

	static bool LogFile(const String& file);

	#define SI_TRACE_IMPL(X,Y) LogTraceImpl(X)
	#define SI_FETAL_IMPL(X,Y) LogFetalImpl(X)
	STRING_FORMAT_FUNCTIONS(static void LogTrace,SI_TRACE_IMPL, )
	STRING_FORMAT_FUNCTIONS(static void LogFetal,SI_FETAL_IMPL, )
	#undef SI_TRACE_IMPL
	#undef SI_FETAL_IMPL

	// Debug break.
	static void DebugBreak();

	// Exit process with return code v.
	static void Exit(int v);

	static void CheckError(const String& s);

protected:
	static void LogTraceImpl(const char* s,...);
	static void LogFetalImpl(const char* s,...);
};

VHWD_LEAVE
#endif
