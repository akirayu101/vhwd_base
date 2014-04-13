#include "vhwd/threading/thread_mutex.h"
#include "vhwd/threading/lockguard.h"

#include "vhwd/basic/clock.h"

#include <ctime>


#ifdef _WIN32
#include <windows.h>
#else
#include "sys/time.h"
#endif

VHWD_ENTER


#ifdef _WIN32

static LARGE_INTEGER freq;
static LARGE_INTEGER tbeg;

static int64_t get_h0()
{
	QueryPerformanceFrequency(&freq);
	SYSTEMTIME t0,t1;
	time_t tt;
	GetLocalTime(&t0);
	for(;;)
	{
		time(&tt);
		GetLocalTime(&t1);
		QueryPerformanceCounter(&tbeg);
		if(t1.wMilliseconds!=t0.wMilliseconds) break;
	}

	return tt*1000000;
}

TimePoint Clock::now()
{
	static int64_t h0=get_h0();
	LARGE_INTEGER tnow;
	QueryPerformanceCounter(&tnow);

	return TimePoint(h0+(tnow.QuadPart-tbeg.QuadPart)*1000000/freq.QuadPart);
}
#else

TimePoint Clock::now()
{
	timeval tv;
	gettimeofday(&tv,NULL);
	return TimePoint(tv.tv_sec*1000000+tv.tv_usec);
}
#endif

const TimePoint Clock::tpStarted(now());

String TimePoint::Format(const String& f) const
{
	TimeDetail td(*this);
	return td.Format(f);
}

const TimePoint& TimePoint::operator+=(const TimeSpan& span)
{
	val+=span.val;
	return *this;
}

static Mutex& get_time_mutex()
{
	static StaticObjectWithoutDeletorT<Mutex> m;
	return m;
}

TimeDetail::TimeDetail(const TimePoint& tp_,int t)
{
	Parse(tp_,t);
}

TimeDetail::TimeDetail(int t)
{
	Parse(Clock::now(),t);
}

int TimeDetail::GetYear() const
{
	return tk.tm_year+1900;
}

int TimeDetail::GetMonth() const
{
	return tk.tm_mon+1;
}

int TimeDetail::GetDay() const
{
	return tk.tm_mday;
}

int TimeDetail::GetHour() const
{
	return tk.tm_hour;
}

int TimeDetail::GetMinute() const
{
	return tk.tm_min;
}

int TimeDetail::GetSecond() const
{
	return tk.tm_sec;
}

int TimeDetail::GetMilliSecond() const
{
	return (tp.val/1000)%1000;
}

String TimeDetail::Format(const String& f) const
{
	char buffer[256];
	if(f=="")
	{
		strftime (buffer,256,"%Y-%m-%d %H:%M:%S ",&tk);
	}
	else
	{
		strftime (buffer,256,f.c_str(),&tk);
	}
	return buffer;
}

static bool read_number(const char*& sp,int& val)
{
	const char* p1=sp;
	val=0;
	while(::isdigit(sp[0]))
	{
		val=val*10+sp[0]-'0';
		sp++;
	}
	if(p1==sp) return false;
	return true;
}


bool TimeDetail::Parse(const String& ts)
{
	struct tm tmp;
	int yy,mm,dd,hh,mu,sc;
	char k1,k2;
	const char* sp=ts.c_str();
	if(!read_number(sp,yy)) return false;
	k1=*sp++;
	if(!read_number(sp,mm)) return false;
	if(k1!=*sp++) return false;
	if(!read_number(sp,dd)) return false;
	sp++;
	if(read_number(sp,hh))
	{
		k2=*sp++;
		if(!read_number(sp,mu)) return false;
		if(k2!=*sp++) return false;
		if(!read_number(sp,sc)) return false;
	}
	else
	{
		hh=mu=sc=0;
	}

	if(yy>1900)
	{
		tmp.tm_year=yy-1900;
	}
	else if(yy<99&&yy>49)
	{
		tmp.tm_year=yy;
	}
	else if(yy<30)
	{
		tmp.tm_year=yy+100;
	}
	else
	{
		return false;
	}

	if(mm<1||mm>12)
	{
		return false;
	}
	tmp.tm_mon=mm-1;

	if(dd<1||dd>31)
	{
		return false;
	}

	tmp.tm_mday=dd;
	tmp.tm_hour=hh;
	tmp.tm_min=mu;
	tmp.tm_sec=sc;

	time_t tt=mktime(&tmp);
	TimePoint tp(tt*1000000);
	Parse(tp);

	return true;
}

bool TimeDetail::Parse(const TimePoint& tp_,int t)
{
	tp=tp_;
	time_t tt=tp.val/1000000;

	LockGuard<Mutex> lck(get_time_mutex());
	struct tm* pk;
	if(t==UTC)
	{
		pk=gmtime(&tt);
	}
	else
	{
		pk=localtime(&tt);
	}
	memcpy(&tk,pk,sizeof(struct tm));
	return true;
}

TimePoint TimeDetail::GetTimePoint() const
{
	return tp;
}

VHWD_LEAVE
