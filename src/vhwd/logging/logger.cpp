#include "vhwd/logging/logger.h"
#include "vhwd/logging/logtarget.h"
#include "vhwd/basic/pointer.h"
#include <cstdarg>

#ifndef VHWD_WINDOWS
#include "errno.h"
#endif


VHWD_ENTER

LogRecord::LogRecord()
{
	m_tStamp=Clock::now();
	m_nId=0;
	m_nSrc=0;
	m_nLevel=LOGLEVEL_MESSAGE;

}

LogRecord::LogRecord(const String& s,int src,int id,int lv)
	:m_sMessage(s)
	,m_nSrc(src)
	,m_nId(id)
	,m_nLevel(lv)
{
	m_tStamp=Clock::now();
}

LogRecord::LogRecord(int src,int id,int lv)
	:m_nSrc(src)
	,m_nId(id)
	,m_nLevel(lv)
{
	m_tStamp=Clock::now();
}


class LoggerImpl : public ObjectData
{
public:
	enum
	{
		FLAG_CACHED=1<<0,
	};


	static LogPtr* def()
	{
		class DefaultLogPtr : public DataPtrT<LogPtr>
		{
		public:

			DefaultLogPtr()
			{
				LogPtr* h=new LogPtr();
				h->SetData(new LogConsole);
				reset(h);
			}

			~DefaultLogPtr()
			{
				reset(NULL);
			}
		};

		static DefaultLogPtr gInstance;
		return gInstance.get();
	}

	LogTarget* m_refData;
	arr_1t<LogRecord> m_aMsg;
	int m_nId;
	int m_nSrc;

	int m_nErrCount;
	int m_nWrnCount;

	BitFlags flags;

	LoggerImpl(int src=0,int id=0)
		:m_refData(NULL)
		,m_nId(id)
		,m_nSrc(src)
		,m_nErrCount(0)
		,m_nWrnCount(0)
	{
		SetData(def());
	}

	~LoggerImpl()
	{
		SetData(NULL);
	}

	void Handle(LogRecord& o)
	{
		if(o.m_nLevel==LOGLEVEL_WARNING)
		{
			m_nWrnCount++;
		}
		if(o.m_nLevel>LOGLEVEL_WARNING)
		{
			m_nErrCount++;
		}

		if(flags.get(FLAG_CACHED))
		{
			m_aMsg.push_back(o);
		}
		else if(m_refData)
		{
			m_refData->Handle(o);
		}
	}

	void SetData(LogTarget* t)
	{
		ObjectData::locked_reset(m_refData,t);
	}

	LogTarget* GetData()
	{
		return m_refData;
	}

};

void Logger::DoLogImplV(int lv,const char* msg,va_list vl)
{
	if(!impl) return;

	LogRecord rcd(impl->m_nSrc,impl->m_nId,lv);


	try
	{
		rcd.m_sMessage.PrintfV(msg,vl);
	}
	catch(...)
	{
		System::LogTrace("String::PrintfV error");
	}


	Handle(rcd);
}

void Logger::DoLogImpl(int lv,const char* msg,...)
{
	va_list arglist;
	va_start(arglist,msg);
	DoLogImplV(lv,msg,arglist);
	va_end(arglist);
}

void Logger::Handle(LogRecord& o)
{
	if(!impl) return;

	try
	{
		impl->Handle(o);
	}
	catch(...)
	{
		System::LogError("Unhandle exception caught in Logger::Handle");
	}
}

Logger::Logger(int src,int id)
{
	impl=new LoggerImpl(src,id);
	impl->IncRef();
}

Logger::Logger(const Logger& o):impl(NULL)
{
	ObjectData::locked_reset(impl,o.impl);
}

Logger& Logger::operator=(const Logger& o)
{
	ObjectData::locked_reset(impl,o.impl);
	return *this;
}

int Logger::Src()
{
	return impl->m_nSrc;
}

void Logger::Src(int src)
{
	impl->m_nSrc=src;
}

int Logger::Id()
{
	return impl->m_nId;
}

void Logger::Id(int id)
{
	impl->m_nId=id;
}

bool Logger::Cache()
{
	return impl->flags.get(LoggerImpl::FLAG_CACHED);
}

void Logger::Cache(bool c)
{
	impl->flags.set(LoggerImpl::FLAG_CACHED,c);
}

Logger::~Logger()
{
	impl->DecRef();
}

LogTarget* Logger::get()
{
	return impl->GetData();
}

void Logger::reset(LogTarget* p)
{
	if(!p) p=new LogNull();
	return impl->SetData(p);
}

bool Logger::Test(int t)
{

	bool needsend=true;

	switch(t)
	{
	case MSG_IF_ANY:
		needsend=true;
		break;
	case MSG_IF_WARNING:
		needsend=(impl->m_nErrCount|impl->m_nErrCount)!=0;
		break;
	case MSG_IF_ERROR:
		needsend=impl->m_nErrCount!=0;
		break;
	default:
		needsend=false;
	};

	needsend=needsend && impl->m_refData!=NULL;

	if(needsend)
	{
		for(size_t i=0; i<impl->m_aMsg.size(); i++)
		{
			impl->m_refData->Handle(impl->m_aMsg[i]);
		}
	}

	bool flag=Ok();
	Clear();
	return flag;
}

bool Logger::Ok()
{
	return impl->m_nErrCount==0;
}

void Logger::Clear()
{
	impl->m_nErrCount=0;
	impl->m_nWrnCount=0;
	impl->m_aMsg.clear();
}

void Logger::def(LogTarget* p)
{
	LoggerImpl::def()->SetData(p);
}

LogTarget* Logger::def()
{
	return LoggerImpl::def()->GetData();
}


LoggerSwap::LoggerSwap()
{
	reset(new LogNull);
	swap(this_logger());
}

LoggerSwap::LoggerSwap(Logger& logger):Logger(logger)
{
	swap(this_logger());
}

LoggerSwap::~LoggerSwap()
{
	swap(this_logger());
}

VHWD_LEAVE
