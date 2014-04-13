#ifndef __H_VHWD_LOG_TARGET__
#define __H_VHWD_LOG_TARGET__


#include "vhwd/basic/pointer.h"
#include "vhwd/basic/object.h"
#include "vhwd/basic/bitflags.h"
#include "vhwd/collection/array.h"

VHWD_ENTER

class VHWD_DLLIMPEXP LogRecord;

class VHWD_DLLIMPEXP LogTarget : public ObjectData
{
public:

	enum
	{
		FLAG_NEWLINE	=1<<0,
		FLAG_SHOWALL	=1<<1,
		FLAG_SHOWDATE	=1<<2,
		FLAG_SHOWTIME	=1<<3,
		FLAG_COLORED	=1<<4,
	};

	LogTarget();

	// Handle the record
	virtual void Handle(const LogRecord& o)=0;

	// cast logrecord to string
	virtual String Format(const LogRecord& o) const;

	// flush if cached.
	virtual void Flush();

	BitFlags flags;
};

// LogNull, ignore all messages.
class VHWD_DLLIMPEXP LogNull : public LogTarget
{
public:
	virtual void Handle(const LogRecord&){}
};

// LogConsole, display messages in console window.
class VHWD_DLLIMPEXP LogConsole : public LogTarget
{
public:
	LogConsole();
	virtual void Handle(const LogRecord&);

protected:
	arr_1t<int> aColors;
};

// LogFile, write messages to file.
class VHWD_DLLIMPEXP LogFile : public LogTarget
{
public:

	LogFile(const String& filename,bool app=true);
	~LogFile();

	virtual void Handle(const LogRecord&);

	bool IsOk(){return fp!=NULL;}

protected:
	FILE* fp;
};

// LogFile, redirect messages to m_refData.
class VHWD_DLLIMPEXP LogPtr : public ObjectT<LogTarget,LogTarget>
{
public:

	void Handle(const LogRecord& o);
};


VHWD_LEAVE
#endif
