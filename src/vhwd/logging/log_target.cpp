#include "vhwd/logging/logtarget.h"
#include "vhwd/logging/logrecord.h"
#include "vhwd/basic/lockguard.h"

#include <cstdlib>
#include <cstdio>

VHWD_ENTER

LogTarget::LogTarget()
{
	flags.clr(FLAG_SHOWTIME);
}

void LogTarget::Flush()
{

}

String LogTarget::Format(const LogRecord& o) const
{
	if(o.m_nLevel==LOGLEVEL_PRINT)
	{
		return o.m_sMessage;
	}
	else
	{
		String msg;

		if(flags.get(FLAG_SHOWDATE))
		{
			if(flags.get(FLAG_SHOWTIME))
			{
				msg << o.m_tStamp.Format("%Y-%m-%d %H:%M:%S ");
			}
			else
			{
				msg << o.m_tStamp.Format("%Y-%m-%d ");
			}
		}
		else if(flags.get(FLAG_SHOWTIME))
		{
			msg << o.m_tStamp.Format("%H:%M:%S ");
		}


		char _nLevel=' ';
		switch(o.m_nLevel)
		{
		case LOGLEVEL_MESSAGE:
			_nLevel='M';
			break;
		case LOGLEVEL_WARNING:
			_nLevel='W';
			break;
		case LOGLEVEL_ERROR:
			_nLevel='E';
			break;
		default:
			_nLevel='O';
			break;
		};

		msg<<String::Format("[%c]",_nLevel);
		msg<<":";
		msg<<o.m_sMessage;

		return msg;
	}
}


LogFile::LogFile(const String& filename,bool app)
{
	int flag=FileAccess::FLAG_WR|FileAccess::FLAG_CR;
	if(app) flag|=FileAccess::FLAG_APPEND;
	fp.Open(filename,flag);
	if(!app)
	{
		fp.Truncate(0);
	}
}

LogFile::~LogFile()
{

}

void LogFile::Handle(const LogRecord& o)
{
	if(!fp.Good()) return;

	LockGuard<AtomicSpin> lock1(spin);

	String s;
	try
	{
		s=Format(o);
	}
	catch(...)
	{
		System::LogError("Unknown exception in LogFile::Handle");
		return;
	}

	fp.Write(s.c_str(),s.size());
	fp.Write("\r\n",2);
	fp.Flush();


}

void LogPtr::Handle(const LogRecord& o)
{
	if(m_refData) m_refData->Handle(o);
}



VHWD_LEAVE
