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

#ifdef _WIN32
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

static const char* Exception_copy_str(const char* p,bool d)
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
	m_sWhat=Exception_copy_str(o.m_sWhat,m_bDelete);
}

const Exception& Exception::operator=(const Exception& o)
{
	m_bDelete=o.m_bDelete;
	m_sWhat=Exception_copy_str(o.m_sWhat,m_bDelete);
	return *this;
}

Exception::Exception(const char* p,bool del)
{
	m_bDelete=del;
	m_sWhat=Exception_copy_str(p,m_bDelete);
}

Exception::Exception(const String& p)
{
	m_bDelete=true;
	m_sWhat=Exception_copy_str(p.c_str(),m_bDelete);
}

Exception::~Exception() throw()
{
	if(m_bDelete && m_sWhat!=NULL)
	{
		::free((void*)m_sWhat);
	}
}

void Exception::XError(const char*p, bool del)
{
	throw Exception(p,del);

}

void Exception::XBadAlloc()
{
	throw std::bad_alloc();
}

void Exception::XBadCast()
{
	throw Exception("badcast",false);
}

void Exception::XInvalidArgument()
{
	throw Exception("invalid_argument",false);
}

void Exception::XInvalidIndex()
{
	throw Exception("invalid_index",false);
}

void Exception::XReadOnly()
{
	throw Exception("read_only",false);
}

void Exception::XTypeLocked()
{
	throw Exception("type_locked",false);
}

void Exception::XBadFactor()
{
	throw Exception("bad_factor",false);
}

void Exception::XNotFound()
{
	throw Exception("not_found",false);
}

VHWD_LEAVE
