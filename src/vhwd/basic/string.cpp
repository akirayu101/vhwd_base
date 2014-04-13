#include <cstdarg>
#include <cstdlib>
#include <locale>

#include "string_impl.h"
#include "vhwd/basic/string.h"
#include "vhwd/basic/stringbuffer.h"
#include "vhwd/basic/system.h"
#include "vhwd/basic/codecvt.h"


#ifndef _WIN32
#include <cerrno>
#include <iconv.h>
#else
#include <windows.h>
#endif

VHWD_ENTER

String::String():m_pStr(gpEmptyString)
{

}

String::String(const StringBuffer<char>& o):m_pStr(gpEmptyString)
{
	if(o.size()>0)
	{
		m_pStr=StringPool::current().str_dup(o.data(),o.size());
	}
}

String::String(const StringBuffer<unsigned char>& o):m_pStr(gpEmptyString)
{
	if(o.size()>0)
	{
		m_pStr=StringPool::current().str_dup((char*)o.data(),o.size());
	}
}

String::String(const String& o):m_pStr(gpEmptyString)
{
	if(o.size()>0)
	{
		m_pStr=StringPool::current().str_dup(o.c_str());
	}
}

String::String(const char* p):m_pStr(gpEmptyString)
{
	if(p[0]!='\0')
	{
		m_pStr=StringPool::current().str_dup(p);
	}
}

String::String(const char* p1,uint32_t n):m_pStr(gpEmptyString)
{
	if(n>0)
	{
		m_pStr=StringPool::current().str_dup(p1,n);
	}
}

String::String(const char* p1,const char* p2):m_pStr(gpEmptyString)
{
	if(p2>p1)
	{
		m_pStr=StringPool::current().str_dup(p1,p2-p1);
	}
	else if(p2<p1)
	{
		System::LogTrace("invalid string position at String::String");
	}
}

void String::assign(const char* p1,uint32_t n)
{
	if(m_pStr!=gpEmptyString)
	{
		StringPool::current().str_free(m_pStr);
		m_pStr=gpEmptyString;
	}
	m_pStr=StringPool::current().str_dup(p1,n);
}

void String::assign(const char* p1,const char* p2)
{
	if(m_pStr!=gpEmptyString)
	{
		StringPool::current().str_free(m_pStr);
		m_pStr=gpEmptyString;
	}
	m_pStr=StringPool::current().str_dup(p1,p2-p1);
}

String::String(const wchar_t* p):m_pStr(gpEmptyString)
{
	(*this)=CodeCvt<wchar_t>::current().ws2s(p);
}

String::String(const wchar_t* p,uint32_t n):m_pStr(gpEmptyString)
{
	(*this)=CodeCvt<wchar_t>::current().ws2s(p,n);
}

String::String(const wchar_t* p1,const wchar_t* p2):m_pStr(gpEmptyString)
{
	(*this)=CodeCvt<wchar_t>::current().ws2s(p1,p2-p1);
}

String::~String()
{
	if(m_pStr!=gpEmptyString)
	{
		StringPool::current().str_free(m_pStr);
	}
	m_pStr=NULL;
}

const char* String::c_str() const
{
	return m_pStr;
}

size_t String::size() const
{
	return ::strlen(m_pStr);
}

size_t String::length() const
{
	return ::strlen(m_pStr);
}

void String_FormatV_internal(String& ret,const char* s,va_list arglist)
{
	static const int _ndefault_buffer_size=256;
	char _buffer[_ndefault_buffer_size];
	int _count=vsnprintf(_buffer,_ndefault_buffer_size,s,arglist);

	if(_count<0)
	{
		ret=s;
		return;
	}
	else if(_count<_ndefault_buffer_size)
	{
		ret=_buffer;
		return;
	}

	int _nLen=std::char_traits<char>::length(s);
	arr_1t<char> _vbuf;
	for(int _nbuf=1024;_nbuf<=1024*1024*128;_nbuf<<=1)
	{
		if(_nbuf<_nLen) continue;

		_vbuf.resize(_nbuf);
		_count=::vsnprintf(&_vbuf[0],_nbuf,s,arglist);
		if(_count<0)
		{
			ret=s;
			return;
		}
		else if(_count<_nbuf)
		{
			ret=&_vbuf[0];
			return;
		}
	}

	Exception::XInvalidArgument();
}


const String& String::PrintfImpl(const char* s,...)
{
	va_list arglist;

	va_start(arglist,s);
	try
	{
		String_FormatV_internal(*this,s,arglist);
	}
	catch(...)
	{
		va_end(arglist);
		throw;
	}
	va_end(arglist);
	return *this;
}

const String& String::PrintfV(const char* s,va_list arglist)
{
	String_FormatV_internal(*this,s,arglist);
	return *this;
}

String String::FormatV(const char* s,va_list arglist)
{
	String ret;
	String_FormatV_internal(ret,s,arglist);
	return ret;
}

String String::FormatImpl(const char* s,...)
{
	va_list arglist;
	String ret;
	va_start(arglist,s);
	try
	{
		String_FormatV_internal(ret,s,arglist);
	}
	catch(...)
	{
		va_end(arglist);
		throw;
	}
	va_end(arglist);
	return ret;
}

void String::append(const char* p,uint32_t n)
{
	char* _newstr=StringPool::current().str_cat(c_str(),size(),p,n);
	if(m_pStr!=gpEmptyString)
	{
		StringPool::current().str_free(m_pStr);
	}
	m_pStr=_newstr;
}

void String::append(const char* p1,const char* p2)
{
	char* _newstr=StringPool::current().str_cat(c_str(),size(),p1,p2-p1);
	if(m_pStr!=gpEmptyString)
	{
		StringPool::current().str_free(m_pStr);
	}
	m_pStr=_newstr;
}


const String& String::operator=(const char* p)
{
	char* _pnewstr=StringPool::current().str_dup(p);
	if(m_pStr!=gpEmptyString)
	{
		StringPool::current().str_free(m_pStr);
	}
	m_pStr=_pnewstr;
	return *this;
}
const String& String::operator=(const String& p)
{
	char* _pnewstr=StringPool::current().str_dup(p.c_str());
	if(m_pStr!=gpEmptyString) StringPool::current().str_free(m_pStr);
	m_pStr=_pnewstr;
	return *this;
}

const String& String::operator=(const wchar_t* p)
{
	(*this)=CodeCvt<wchar_t>::current().ws2s(p);
	return *this;
}

const String& String::operator+=(const char* p)
{
	char* h=StringPool::current().str_cat(m_pStr,p);
	if(m_pStr!=gpEmptyString) StringPool::current().str_free(m_pStr);
	m_pStr=h;
	return *this;
}

const String& String::operator+=(const String& p)
{
	char* h=StringPool::current().str_cat(m_pStr,p.c_str());
	if(m_pStr!=gpEmptyString) StringPool::current().str_free(m_pStr);
	m_pStr=h;
	return *this;
}


const String& String::operator+=(const wchar_t* p)
{
	(*this)+=CodeCvt<wchar_t>::current().ws2s(p);
	return *this;
}

bool String::ToLong(int64_t* val) const
{
	if(!val) return false;
	*val=::atoi(c_str());
	return true;
}

bool String::ToDouble(float64_t* val) const
{
	if(!val) return false;
	*val=::atof(c_str());
	return true;
}


std::ostream& operator<<(std::ostream&o,const String& s)
{
    std::string stds(s.c_str());
	o<<stds;
	return o;
}

std::istream& operator>>(std::istream&o,String& s)
{
	std::string tmp;
	o>>tmp;
	s=tmp.c_str();
	return o;
}

String operator+(const String& lhs,const String& rhs)
{
	String tmp(lhs);tmp+=rhs;return tmp;
}

String operator+(const char* lhs,const String& rhs)
{
	String tmp(lhs);tmp+=rhs;return tmp;
}

String operator+(const String& lhs,const char* rhs)
{
	String tmp(lhs);tmp+=rhs;return tmp;
}

String& String::operator<<(char v)
{
	(*this)+=String::Format("%c",v);
	return (*this);
}

String& String::operator<<(int32_t v)
{
	(*this)+=String::Format("%d",v);
	return (*this);
}

String& String::operator<<(int64_t v)
{
	(*this)+=String::Format("%ld",v);
	return (*this);
}

String& String::operator<<(uint32_t v)
{
	(*this)+=String::Format("%d",v);
	return (*this);
}

String& String::operator<<(uint64_t v)
{
	(*this)+=String::Format("%ld",v);
	return (*this);
}

String& String::operator<<(float v)
{
	(*this)+=String::Format("%f",v);
	return (*this);
}

String& String::operator<<(double v)
{
	(*this)+=String::Format("%g",v);
	return (*this);
}

String& String::operator<<(const char* v)
{
	(*this)+=v;
	return (*this);
}

String& String::operator<<(const wchar_t* v)
{
	(*this)+=CodeCvt<wchar_t>::current().ws2s(v);
	return (*this);
}

String& String::operator<<(const String& v)
{
	(*this)+=v;
	return (*this);
}

String& String::operator<<(const StringBuffer<char>& o)
{
	append(o.data(),o.size());
	return *this;
}

String& String::operator<<(const StringBuffer<unsigned char>& o)
{
	append((char*)o.data(),o.size());
	return *this;
}

#ifndef _WIN32

String String::ansi_to_utf8(const String& o)
{
	return o;
}

String String::utf8_to_ansi(const String& o)
{
	return o;
}

bool String::ansi_to_utf8(StringBuffer<char>& o,const char* p1,size_t ln)
{
	o.resize(ln);
	if(ln>0) memcpy(&o[0],p1,ln);
	return true;
}

bool String::utf8_to_ansi(StringBuffer<char>& o,const char* p1,size_t ln)
{
	o.resize(ln);
	if(ln>0) memcpy(&o[0],p1,ln);
	return true;
}

#else

bool String::ansi_to_utf8(StringBuffer<char>& va,const char* p1,size_t ln)
{
	StringBuffer<wchar_t> vh;
	if(!CodeCvt<wchar_t>::current().s2ws(vh,p1,ln))
	{
		return false;
	}
	va.resize(vh.size()*3+1);
	int wl=::WideCharToMultiByte(CP_UTF8,0,&vh[0],vh.size(),&va[0],va.size(),NULL,NULL);

	if(wl<0)
	{
		va.resize(0);
		return false;
	}
	va[wl]='\0';
	va.resize(wl);
	return true;;
}

bool String::utf8_to_ansi(StringBuffer<char>& va,const char* p1,size_t ln)
{

	StringBuffer<wchar_t> vh;
	vh.resize(ln+1);
	int wl=MultiByteToWideChar(CP_UTF8,0,p1,ln,&vh[0],vh.size());
	if(wl<0)
	{
		va.resize(0);
		return false;
	}

	return CodeCvt<wchar_t>::current().ws2s(va,&vh[0],wl);
}


String String::ansi_to_utf8(const String& o)
{
	StringBuffer<char> va;
	if(ansi_to_utf8(va,o.c_str(),o.size()))
	{
		String s(va);
		return s;
	}
	else
	{
		System::LogTrace("ansi_to_utf8 failed:%s",o.c_str());
		return "???";
	}

}

String String::utf8_to_ansi(const String& o)
{
	StringBuffer<char> va;
	if(utf8_to_ansi(va,o.c_str(),o.size()))
	{
		String s(va);
		return s;
	}
	else
	{
		System::LogTrace("utf8_to_ansi failed:%s",o.c_str());
		return "???";
	}

}



#endif



String String::substr(size_t pos,size_t len) const
{
	wassert(pos<size());
	String s(m_pStr+pos,std::min(len,size()-pos));
	return s;
}




VHWD_LEAVE
