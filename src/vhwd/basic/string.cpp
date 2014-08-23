
#include "string_impl.h"
#include "vhwd/basic/string.h"
#include "vhwd/basic/stringbuffer.h"
#include "vhwd/basic/system.h"
#include "vhwd/basic/codecvt.h"

#include "stdio.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <cerrno>
#include <iconv.h>
#endif


VHWD_ENTER


String::String()
{
	m_ptr=StringPool::current().str_empty();
}

String::String(const StringBuffer<char>& o)
{
	m_ptr=StringPool::current().str_dup(o.data(),o.size());
}

String::String(const StringBuffer<unsigned char>& o)
{
	m_ptr=StringPool::current().str_dup((char*)o.data(),o.size());
}

String::String(const String& o)
{
	m_ptr=StringPool::current().str_dup(o.c_str());
}

String::String(const char* p)
{
	m_ptr=StringPool::current().str_dup(p);
}

String::String(const char* p1,uint32_t n)
{
	m_ptr=StringPool::current().str_dup(p1,n);
}

String::String(const char* p1,const char* p2)
{
	m_ptr=StringPool::current().str_dup(p1,safe_distance(p1,p2));
}

String::String(const unsigned char* p)
{
	m_ptr=StringPool::current().str_dup((char*)p);
}

String::String(const unsigned char* p1,uint32_t n)
{
	m_ptr=StringPool::current().str_dup((char*)p1,n);
}

String::String(const unsigned char* p1,unsigned const char* p2)
{
	m_ptr=StringPool::current().str_dup((char*)p1,safe_distance(p1,p2));
}

void String::assign(const char* p1,uint32_t n)
{
	char* _pnewstr=StringPool::current().str_dup(p1,n);
	StringPool::current().str_free(m_ptr);
	m_ptr=_pnewstr;
}

void String::assign(const char* p1,const char* p2)
{
	assign(p1,safe_distance(p1,p2));
}

void String::assign(const unsigned char* p1,uint32_t n)
{
	char* _pnewstr=StringPool::current().str_dup((char*)p1,n);
	StringPool::current().str_free(m_ptr);
	m_ptr=_pnewstr;
}

void String::assign(const unsigned char* p1,const unsigned char* p2)
{
	assign((char*)p1,safe_distance(p1,p2));
}


String::String(const wchar_t* p)
{
	m_ptr=StringPool::current().str_empty();
	StringBuffer<char> sb;
	IConv::unicode_to_ansi(sb,p,std::char_traits<wchar_t>::length(p));
	(*this)=sb;
}

String::String(const wchar_t* p,uint32_t n)
{
	m_ptr=StringPool::current().str_empty();
	StringBuffer<char> sb;
	IConv::unicode_to_ansi(sb,p,n);
	(*this)=sb;
}

String::String(const wchar_t* p1,const wchar_t* p2)
{
	m_ptr=StringPool::current().str_empty();
	StringBuffer<char> sb;
	IConv::unicode_to_ansi(sb,p1,p2-p1);
	(*this)=sb;
}

String::~String()
{
	StringPool::current().str_free(m_ptr);
}

const char* String::c_str() const
{
	return m_ptr;
}

size_t String::size() const
{
	return ::strlen(m_ptr);
}

size_t String::length() const
{
	return ::strlen(m_ptr);
}


void String_FormatV_internal(String& ret,const char* s,va_list arglist)
{
	static const int _ndefault_buffer_size=1024;
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
	for(int _nbuf=1024; _nbuf<=1024*1024*128; _nbuf<<=1)
	{
		if(_nbuf<_nLen) continue;

		_vbuf.resize(_nbuf);
		_count=vsnprintf(&_vbuf[0],_nbuf,s,arglist);
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

String& String::PrintfV(const char* s,va_list arglist)
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
	char* _pnewstr=StringPool::current().str_cat(c_str(),size(),p,n);
	StringPool::current().str_free(m_ptr);
	m_ptr=_pnewstr;
}

void String::append(const char* p1,const char* p2)
{
	char* _pnewstr=StringPool::current().str_cat(c_str(),size(),p1,safe_distance(p1,p2));
	StringPool::current().str_free(m_ptr);
	m_ptr=_pnewstr;
}


String& String::operator=(const char* p)
{
	char* _pnewstr=StringPool::current().str_dup(p);
	StringPool::current().str_free(m_ptr);
	m_ptr=_pnewstr;
	return *this;
}

String& String::operator=(const String& p)
{
	char* _pnewstr=StringPool::current().str_dup(p.c_str());
	StringPool::current().str_free(m_ptr);
	m_ptr=_pnewstr;
	return *this;
}

String& String::operator=(const wchar_t* p)
{
	StringBuffer<char> sb;
	IConv::unicode_to_ansi(sb,p,std::char_traits<wchar_t>::length(p));
	(*this)=sb;

	return *this;
}

String& String::operator+=(const char* p)
{
	char* _pnewstr=StringPool::current().str_cat(m_ptr,p);
	StringPool::current().str_free(m_ptr);
	m_ptr=_pnewstr;
	return *this;
}

String& String::operator+=(const String& p)
{
	char* _pnewstr=StringPool::current().str_cat(m_ptr,p.c_str());
	StringPool::current().str_free(m_ptr);
	m_ptr=_pnewstr;
	return *this;
}


String& String::operator+=(const wchar_t* p)
{
	StringBuffer<char> sb;
	IConv::unicode_to_ansi(sb,p,std::char_traits<wchar_t>::length(p));
	(*this)+=sb;

	return *this;

}



bool String::ToNumber(int64_t* val) const
{
	if(!val) return false;
	*val=::atoi(c_str());
	return true;
}

bool String::ToNumber(int32_t* val) const
{
	if(!val) return false;
	*val=::atoi(c_str());
	return true;
}

bool String::ToNumber(float32_t* val) const
{
	if(!val) return false;
	*val=::atof(c_str());
	return true;
}

bool String::ToNumber(float64_t* val) const
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
	String tmp(lhs);
	tmp+=rhs;
	return tmp;
}

String operator+(const char* lhs,const String& rhs)
{
	String tmp(lhs);
	tmp+=rhs;
	return tmp;
}

String operator+(const String& lhs,const char* rhs)
{
	String tmp(lhs);
	tmp+=rhs;
	return tmp;
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
	(*this)+=String::Format("%lld",v);
	return (*this);
}

String& String::operator<<(uint32_t v)
{
	(*this)+=String::Format("%u",v);
	return (*this);
}

String& String::operator<<(uint64_t v)
{
	(*this)+=String::Format("%llu",v);
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

String& String::operator<<(const wchar_t* p)
{
	StringBuffer<char> sb;
	IConv::unicode_to_ansi(sb,p,std::char_traits<wchar_t>::length(p));
	(*this)+=sb;

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

String String::substr(size_t pos,size_t len) const
{
	wassert(pos<size());
	String s(m_ptr+pos,std::min(len,size()-pos));
	return s;
}

int String::find (char c, int pos) const
{
	int n=(int)size();
	for(int i=pos; i<n; i++)
	{
		if(m_ptr[i]==c) return i;
	}
	return -1;
}

int String::replace(char c1,char c2)
{
	int n=0;
	StringBuffer<char> sb(*this);
	for(size_t i=0; i<sb.size(); i++)
	{
		if(sb[i]==c1)
		{
			sb[i]=c2;
			n++;
		}
	}

	(*this)=sb;
	return n;
}

int String::replace(const String& c1,const String& c2)
{
	int n=0;

	StringBuffer<char> sb;

	const char* _pOld=c1.c_str();
	size_t _nOld=c1.size();

	const char* _pNew=c2.c_str();
	size_t _nNew=c2.size();

	const char* p1=m_ptr;
	const char* p2=m_ptr+size();
	for(;;)
	{
		const char* pt=::strstr(p1,_pOld);
		if(pt==NULL)
		{
			sb.append(p1,p2);
			break;
		}
		sb.append(p1,pt);
		sb.append(_pNew,_nNew);
		p1=pt+_nOld;
		n++;
	}
	(*this)=sb;
	return n;
}

const char const_empty_buffer[64]= {0};

VHWD_LEAVE
