#include "vhwd/basic/system.h"
#include "vhwd/basic/string.h"

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
	int m_nCpuCount;
	int m_nCacheLine;
	int m_nPageSize;

	static SystemInfo& current()
	{
		static SystemInfo si;
		return si;
	}

	SystemInfo();

};


#ifdef _WIN32


SystemInfo::SystemInfo()
{
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

}

#else

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

}

#endif

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

static FILE* fp_logfile=NULL;

bool System::LogFile(const String& fn)
{
	if(fp_logfile)
	{
		::fclose(fp_logfile);
		fp_logfile=NULL;
	}
	if(fn.empty()) return false;
	fp_logfile=::fopen(fn.c_str(),"a");
	return fp_logfile!=NULL;
}

void System::LogTraceImpl(const char* msg,...)
{

	time_t tt=time(NULL);
	char buf1[256];
	strftime (buf1,256,"%Y-%m-%d %H:%M:%S ",localtime(&tt));

	char buf2[1024];
	va_list arg;
	va_start(arg,msg);
	::vsnprintf(buf2,1024,msg,arg);
	va_end(arg);

	if(fp_logfile!=NULL)
	{
        fprintf(fp_logfile,"%s trace:%s\r\n",buf1,buf2);
        fflush(fp_logfile);
	}
	else
	{
        printf("%s trace:%s\r\n",buf1,buf2);
	}
}

void System::LogFetalImpl(const char* msg,...)
{


	time_t tt=time(NULL);
	char buf1[256];
	strftime (buf1,256,"%Y-%m-%d %H:%M:%S ",localtime(&tt));

	char buf2[1024];
	va_list arg;
	va_start(arg,msg);
	::vsnprintf(buf2,1024,msg,arg);
	va_end(arg);

	if(fp_logfile!=NULL)
	{
        fprintf(fp_logfile,"%s fetal:%s\r\n",buf1,buf2);
        fflush(fp_logfile);
	}
	else
	{
        printf("%s fetal:%s\r\n",buf1,buf2);
	}

	Exit(-1);
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
	LogTrace("System::Exit(%d)",v);
	::exit(v);
}

void System::CheckError(const String& s)
{
#ifdef _WIN32

	int ret=::GetLastError();
	if(ret!=0)
	{
		LogTrace("%s failed with ret=%d",s,ret);
	}

#else

	int eno=errno;
	if(eno!=0)
	{
		LogTrace("%s failed: %s",s,strerror(eno));
	}

#endif
}

VHWD_LEAVE
