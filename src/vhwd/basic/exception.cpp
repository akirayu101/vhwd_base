#include "vhwd/basic/exception.h"
#include "vhwd/basic/string.h"
#include <cstdlib>

VHWD_ENTER


static const char* s_exception_desc="unknown_exception";

Exception::Exception()
{
	m_sWhat=NULL;
	m_bDelete=false;
}

#ifdef VHWD_WINDOWS
const char* Exception::what() const throw()
{
	return m_sWhat?m_sWhat:s_exception_desc;
}
#else
const char* Exception::what() throw()
{
	return m_sWhat?m_sWhat:s_exception_desc;
}
#endif

static const char* Exception_strdup(const char* p,bool d)
{
	if(!p) return NULL;
	if(!d) return p;

	char* s=(char*)::malloc(strlen(p)+1);
	if(!s) return NULL;
	strcpy(s,p);
	return s;
}

Exception::Exception(const Exception& o)
{
	m_bDelete=o.m_bDelete;
	m_sWhat=Exception_strdup(o.m_sWhat,m_bDelete);
}

Exception& Exception::operator=(const Exception& o)
{
	m_bDelete=o.m_bDelete;
	m_sWhat=Exception_strdup(o.m_sWhat,m_bDelete);
	return *this;
}

Exception::Exception(const char* p,bool del)
{
	m_bDelete=del;
	m_sWhat=Exception_strdup(p,m_bDelete);
}

Exception::Exception(const String& p)
{
	m_bDelete=true;
	m_sWhat=Exception_strdup(p.c_str(),m_bDelete);
}

Exception::~Exception() throw()
{
	if(m_bDelete && m_sWhat!=NULL)
	{
		::free((void*)m_sWhat);
	}
}

template<typename T>
inline void Exception_Throw(const T& x)
{
	throw x;
}


void Exception::XError()
{
	static const Exception _tException("unknown_exception",false);
	Exception_Throw(_tException);
}

void Exception::XError(const String& s)
{
	Exception _tException(s);
	Exception_Throw(_tException);
}


void Exception::XError(const char*p, bool del)
{	
	Exception _tException(p,del);
	Exception_Throw(_tException);
}

class ExceptionBadAlloc : public std::bad_alloc
{
public:

#ifdef VHWD_WINDOWS
	const char* what() const throw()
#else
	const char* what() throw()
#endif
	{
		return "not_enough_memory";
	}
};

void Exception::XBadAlloc()
{
	static const ExceptionBadAlloc _tException;
	Exception_Throw(_tException);
}

void Exception::XBadCast()
{
	static const Exception _tException("badcast",false);
	Exception_Throw(_tException);
}

void Exception::XInvalidArgument()
{
	static const Exception _tException("invalid_argument",false);
	Exception_Throw(_tException);
}

void Exception::XInvalidIndex()
{
	static const Exception _tException("invalid_index",false);
	Exception_Throw(_tException);
}

void Exception::XReadOnly()
{
	static const Exception _tException("read_only",false);
	Exception_Throw(_tException);
}

void Exception::XTypeLocked()
{
	static const Exception _tException("type_locked",false);
	Exception_Throw(_tException);
}

void Exception::XBadFunctor()
{
	static const Exception _tException("bad_functor",false);
	Exception_Throw(_tException);
}

void Exception::XNotFound()
{
	static const Exception _tException("not_found",false);
	Exception_Throw(_tException);
}

VHWD_LEAVE
