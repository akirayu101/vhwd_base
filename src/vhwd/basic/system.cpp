#include "vhwd/basic/system.h"
#include "vhwd/basic/string.h"
#include "vhwd/basic/lockguard.h"
#include "vhwd/basic/stringbuffer.h"

#include <ctime>
#include <cstdlib>
#include <cstdio>

#ifdef _WIN32
#include <windows.h>
#else
#include <cerrno>
#endif

VHWD_ENTER

class SystemInfo
{
public:

	int64_t m_nMemTotalPhys;
	int64_t m_nMemAvailPhys;
	int64_t m_nMemTotalVirtual;
	int64_t m_nMemAvailVirtual;
	int64_t m_nMemTotalPageFile;
	int64_t m_nMemAvailPageFile;

	int m_nCpuCount;
	int m_nCacheLine;
	int m_nPageSize;

	void update();

	static SystemInfo& current()
	{
		static SystemInfo si;
		return si;
	}

	SystemInfo();

};


#ifdef _WIN32

void SystemInfo::update()
{
	MEMORYSTATUSEX ms;
	ms.dwLength=sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&ms);

	m_nMemTotalPhys=ms.ullTotalPhys;
	m_nMemAvailPhys=ms.ullAvailPhys;
	m_nMemTotalVirtual=ms.ullTotalVirtual;
	m_nMemAvailVirtual=ms.ullAvailVirtual;
	m_nMemTotalPageFile=ms.ullTotalPageFile;
	m_nMemAvailPageFile=ms.ullAvailPageFile;
}

SystemInfo::SystemInfo()
{
#ifdef _MSC_VER
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	m_nPageSize=info.dwPageSize;
	m_nCpuCount=info.dwNumberOfProcessors;

	size_t line_size = 0;
	DWORD buffer_size = 0;
	DWORD i = 0;
	SYSTEM_LOGICAL_PROCESSOR_INFORMATION * buffer = 0;

	GetLogicalProcessorInformation(0, &buffer_size);
	buffer = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION *)malloc(buffer_size);
	GetLogicalProcessorInformation(&buffer[0], &buffer_size);

	for (i = 0; i != buffer_size / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION); ++i)
	{
		if (buffer[i].Relationship == RelationCache && buffer[i].Cache.Level == 1)
		{
			line_size = buffer[i].Cache.LineSize;
			break;
		}
	}

	free(buffer);
	m_nCacheLine=(int)line_size;
#endif

	update();
}

#else

void SystemInfo::update()
{

}

SystemInfo::SystemInfo()
{

	m_nPageSize=sysconf(_SC_PAGE_SIZE);;
	m_nCpuCount=sysconf(_SC_NPROCESSORS_ONLN);
	m_nCacheLine=-1;

	FILE * p = 0;
	p = fopen("/sys/devices/system/cpu/cpu0/cache/index0/coherency_line_size", "r");

	if (p)
	{
		fscanf(p, "%d", &m_nCacheLine);
		fclose(p);
	}

	update();

}

#endif


int64_t System::GetMemTotalPhys()
{
	return SystemInfo::current().m_nMemTotalPhys;
}

int64_t System::GetMemAvailPhys()
{
	return SystemInfo::current().m_nMemAvailPhys;
}

int64_t System::GetMemTotalVirtual()
{
	return SystemInfo::current().m_nMemTotalVirtual;
}

int64_t System::GetMemAvailVirtual()
{
	return SystemInfo::current().m_nMemAvailVirtual;
}

void System::Update()
{
	SystemInfo::current().update();
}

int System::GetCpuCount()
{
	return SystemInfo::current().m_nCpuCount;
}

int System::GetPageSize()
{
	return SystemInfo::current().m_nPageSize;
}

int System::GetCacheLineSize()
{
	return SystemInfo::current().m_nCacheLine;
}

int System::GetPid()
{
#ifdef _WIN32
	return ::GetCurrentProcessId();
#else
	return getpid();
#endif
}

double System::GetCpuTime()
{
#ifdef _WIN32
	FILETIME k[4];

	LARGE_INTEGER tbeg;
	QueryPerformanceCounter(&tbeg);

	if(GetProcessTimes(GetCurrentProcess(),k+0,k+1,k+2,k+3)!=FALSE)
	{
		double t1=(double)(k[3].dwLowDateTime|((unsigned long long)k[3].dwHighDateTime<<32))*0.0000001;
		double t2=(double)(k[2].dwLowDateTime|((unsigned long long)k[2].dwHighDateTime<<32))*0.0000001;
		//if(p1) *p1=t1;
		//if(p2) *p2=t2;
		return t1+t2;
	}
	else
	{
		return 0;
	}
#else
	double t0=((double)clock())/double(CLOCKS_PER_SEC);
	//if(p1) *p1=t0;
	//if(p2) *p2=t0;
	return t0;
#endif
}


void System::DebugBreak()
{
#ifdef _WIN32
	::DebugBreak();
#else
	__asm__ volatile("int $0x03");
#endif
}

void System::Exit(int v)
{
	System::LogTrace("System::Exit(%d)",v);
	::exit(v);
}


#ifdef _WIN32

String GetModulePathImpl()
{
	char buf[MAX_PATH];buf[0]=0;
	::GetModuleFileNameA(NULL,buf,MAX_PATH);
	return buf;
}

const String& System::GetModulePath()
{
	static String sModulePath(GetModulePathImpl());
	return sModulePath;
}

bool System::Execute(const String& s)
{
	STARTUPINFOA si={sizeof(STARTUPINFO)};
    PROCESS_INFORMATION pi;
	StringBuffer<char> sb(s);
	sb.push_back(0);

	if(!::CreateProcessA(NULL,sb.data(),NULL,NULL,FALSE,0,NULL,NULL,&si,&pi))
	{
		System::LogTrace("System::Exectue:%s FAILED",s);
		return false;
	}

	System::LogTrace("System::Exectue:%s",s);

	::CloseHandle(pi.hProcess);
	::CloseHandle(pi.hThread);

	return true;


}

#else

String GetModulePathImpl()
{
	return "";
}

const String& System::GetModulePath()
{
	static String sModulePath(GetModulePathImpl());
	return sModulePath;
}

bool System::Execute(const String& s)
{
	int pid;
	if((pid=fork())==-1)
	{
		System::LogTrace("System::Exectue:%s FAILED",s);
		return false;
	}
	if(pid==0)
	{
		execlp(s.c_str(),NULL,NULL);
		exit(-1);
	}
	else
	{
		System::LogTrace("System::Exectue:%s",s);
		return true;
	}
}

#endif


#ifdef _WIN32

char* win_strerror(int ret)
{
	char* lpTStr(NULL);
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		ret,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpTStr,
		0x100,
		NULL);
	return lpTStr;
}

#endif

void System::CheckError(const String& msg)
{
#ifdef _WIN32

	int ret=::GetLastError();
	if(ret!=0)
	{
		System::LogTrace("%s failed, code(%d): %s",msg,ret,win_strerror(ret));
	}

#else

	int ret=errno;
	if(ret!=0)
	{
		System::LogTrace("%s failed, code(%d): %s",msg,ret,strerror(ret));
	}

#endif
}


extern AtomicSpin g_tSpinConsole;

class SystemLoggerData
{
public:

	static const char* GetMsgLevel(int lv)
	{
		switch(lv)
		{
		case LOGLEVEL_DEBUG: return "debug";
		case LOGLEVEL_ERROR: return "error";
		case LOGLEVEL_TRACE: return "trace";
		case LOGLEVEL_FETAL: return "fetal";
		default: return "other";
		}
	}

	void LogImplV(int lv,const char* msg,va_list arg)
	{

		time_t tt=time(NULL);
		char buf2[1024];
		char buf1[256];
		strftime (buf1,256,"%Y-%m-%d %H:%M:%S ",localtime(&tt));

		::vsnprintf(buf2,1024,msg,arg);

		if(fp_logfile!=NULL)
		{
			LockGuard<AtomicSpin> lock1(spin);
			fprintf(fp_logfile,"%s %s:%s\r\n",buf1,GetMsgLevel(lv),buf2);
			fflush(fp_logfile);
		}
		else
		{
			LockGuard<AtomicSpin> lock1(g_tSpinConsole);
			printf("%s %s:%s\r\n",buf1,GetMsgLevel(lv),buf2);
		}

		if(lv==LOGLEVEL_FETAL)
		{
			System::Exit(-1);
		}
	}

	bool SetLogFile(const String& fn,bool app)
	{
		if(fp_logfile)
		{
			::fclose(fp_logfile);
			fp_logfile=NULL;
		}
		fp_logfile=::fopen(fn.c_str(),app?"a":"w");
		return fp_logfile!=NULL;
	}

	FILE* fp_logfile;
	AtomicSpin spin;
}gSystelLoggerData;


bool System::SetLogFile(const String& fn,bool app)
{
	return gSystelLoggerData.SetLogFile(fn,app);
}


void System::DoLogImpl(int lv,const char* msg,...)
{
	va_list arglist;
	va_start(arglist,msg);
	gSystelLoggerData.LogImplV(lv,msg,arglist);
	va_end(arglist);
}



VHWD_LEAVE
