// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_BASIC_CLOCK__
#define __H_VHWD_BASIC_CLOCK__

#include "vhwd/config.h"
#include "vhwd/basic/string.h"

VHWD_ENTER

class VHWD_DLLIMPEXP String;
class VHWD_DLLIMPEXP TimeSpan;
class VHWD_DLLIMPEXP ElapsedTimer;

// TimePoint
class VHWD_DLLIMPEXP TimePoint
{
public:

	TimePoint():val(0){}

	explicit TimePoint(int64_t v):val(v){}

	// cast to time_t
	time_t GetTime() const {return val/1000000;}

	// set from time_t and microseconds
	void SetTime(time_t v,int microseconds=0){val=(int64_t)v*1000000+(int64_t)microseconds;}

	// cast to string
	String Format(const String& f="") const;

	// add a timespan
	TimePoint& operator+=(const TimeSpan& span);

	int64_t val;
};

// TimeDetail
class VHWD_DLLIMPEXP TimeDetail
{
public:

	enum
	{
		LOCAL,
		UTC,
	};

	TimeDetail(int t=LOCAL);
	TimeDetail(const TimePoint& tp_,int t=LOCAL);

	int GetYear() const;
	int GetMonth() const;
	int GetDay() const;
	int GetHour() const;
	int GetMinute() const;
	int GetSecond() const;
	int GetMilliSecond() const;

	String Format(const String& f="") const;

	// Parse from string yy-mm-dd[ hh:mm:dd]
	bool Parse(const String& ts_);


	bool Parse(const TimePoint& tp_,int t=LOCAL);

	TimePoint GetTimePoint() const;
	struct tm& GetTm(){return tk;}

private:
	struct tm tk;
	TimePoint tp;
};

// TimeSpan
class VHWD_DLLIMPEXP TimeSpan
{
public:
	TimeSpan():val(0){}
	explicit TimeSpan(int64_t v):val(v){}

	int64_t GetSeconds() const {return val/1000000;}
	int64_t GetMilliSeconds() const {return val/1000;}
	int64_t GetMicroSeconds() const {return val;}

	static TimeSpan Day(int64_t v){return Seconds(60*60*24*v);}
	static TimeSpan Hours(int64_t v){return Seconds(60*60*v);}
	static TimeSpan Minutes(int64_t v){return Seconds(60*v);}
	static TimeSpan Seconds(int64_t v){return TimeSpan(1000*1000*v);}
	static TimeSpan MilliSeconds(int64_t v){return TimeSpan(1000*v);}
	static TimeSpan MicroSeconds(int64_t v){return TimeSpan(v);}

	int64_t val;
};


#define BOOL_OPERATOR(TYPE,VALUE)\
inline bool operator <(const TYPE& p1,const TYPE& p2){return p1.VALUE  < p2.VALUE;}\
inline bool operator<=(const TYPE& p1,const TYPE& p2){return p1.VALUE <= p2.VALUE;}\
inline bool operator>=(const TYPE& p1,const TYPE& p2){return p1.VALUE >= p2.VALUE;}\
inline bool operator >(const TYPE& p1,const TYPE& p2){return p1.VALUE  > p2.VALUE;}\
inline bool operator==(const TYPE& p1,const TYPE& p2){return p1.VALUE == p2.VALUE;}\
inline bool operator!=(const TYPE& p1,const TYPE& p2){return p1.VALUE != p2.VALUE;}

BOOL_OPERATOR(TimePoint,val)
BOOL_OPERATOR(TimeSpan,val)

// TimeSpan and TimePoint operators

inline double operator/(const TimeSpan& s1,const TimeSpan& s2)
{
	return double(s1.val)/double(s2.val);
}

inline const TimeSpan operator*(const TimeSpan& s1,double v)
{
	return TimeSpan((int64_t)(double(s1.val)*v));
}

inline const TimeSpan operator*(double v,const TimeSpan& s1)
{
	return TimeSpan((int64_t)(double(s1.val)*v));
}

inline TimeSpan operator-(const TimePoint& p1,const TimePoint& p2)
{
	return TimeSpan(p1.val-p2.val);
}

inline TimePoint operator+(const TimePoint& p1,const TimeSpan& p2)
{
	return TimePoint(p1.val+p2.val);
}

inline TimePoint operator-(const TimePoint& p1,const TimeSpan& p2)
{
	return TimePoint(p1.val-p2.val);
}

inline TimePoint operator+(const TimeSpan& p1,const TimePoint& p2)
{
	return TimePoint(p1.val+p2.val);
}

inline TimeSpan operator+(const TimeSpan& p1,const TimeSpan& p2)
{
	return TimeSpan(p1.val+p2.val);
}


class VHWD_DLLIMPEXP Clock
{
public:

	// now() returns current TimePoint
	static TimePoint now();

	// process started time
	static const TimePoint tpStarted;
};


class VHWD_DLLIMPEXP ElapsedTimer
{
public:

	ElapsedTimer()
	{
		tick();
	}

	void tick()
	{
		p1=p2=Clock::now();
	}

	double tack()
	{
		p2=Clock::now();
		return get();
	}

	double get()
	{
		return (p2-p1)/TimeSpan::Seconds(1);
	}

private:
	TimePoint p1;
	TimePoint p2;
};

VHWD_LEAVE
#endif
