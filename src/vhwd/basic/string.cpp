
#include "string_impl.h"
#include "vhwd/basic/string.h"
#include "vhwd/basic/stringbuffer.h"
#include "vhwd/basic/system.h"
#include "vhwd/basic/codecvt.h"

#include "stdio.h"

#ifdef VHWD_WINDOWS
#include <windows.h>
#else
#include <cerrno>
#include <iconv.h>
#endif


VHWD_ENTER


inline void String::_do_append(const char* p1,uintptr_t n)
{
	char* _pnewstr=StringPool::current().str_cat(m_ptr,::strlen(m_ptr),p1,n);
	StringPool::current().str_free(m_ptr);
	m_ptr=_pnewstr;
}

inline void String::_do_append(const wchar_t* p1,uintptr_t n)
{
	StringBuffer<char> sb;
	IConv::unicode_to_ansi(sb,p1,n);
	_do_append(sb.data(),sb.size());
}

inline void String::_do_assign(const char* p1,uintptr_t n)
{
	char* _pnewstr=StringPool::current().str_dup(p1,n);
	StringPool::current().str_free(m_ptr);
	m_ptr=_pnewstr;
}

inline void String::_do_assign(const wchar_t* p1,uintptr_t n)
{
	StringBuffer<char> sb;
	IConv::unicode_to_ansi(sb,p1,n);

	_do_assign(sb.data(),sb.size());
}


template<typename T>
inline void String::_do_format_integer(T v)
{
	char buf[32];

	char* p1=buf+31;
	char* p2=p1;

	*p2=0;


	if(v==0)
	{
		*--p1='0';
	}
	else if(v<(T)0)
	{
		v=(~v)+1;

		do
		{
			*--p1='0'+(v%10);
			v=v/10;
		}while(v>0);

		*--p1='-';
	}
	else
	{
		do
		{
			*--p1='0'+(v%10);
			v=v/10;
		}while(v>0);
	}

	_do_append(p1,p2-p1);
}



inline int String_do_prinfv_err(String& ret)
{
	ret="";
	System::LogTrace("vsnprintf error");
	return -1;
}

int String_do_prinfv_try(String& ret,const char* s,va_list arglist)
{

	int _nLen=std::char_traits<char>::length(s);
	arr_1t<char> _vbuf;
	int _count;

	try
	{
		for(int _nbuf=1024<<1; _nbuf<=1024*1024*128; _nbuf<<=1)
		{
			if(_nbuf<_nLen) continue;

			_vbuf.resize(_nbuf);
			_count=vsnprintf(&_vbuf[0],_nbuf,s,arglist);
			if(_count<0)
			{
				break;
			}
			else if(_count<_nbuf)
			{
				ret=&_vbuf[0];
				return _count;
			}
		}
	}
	catch(...)
	{
	}

	return String_do_prinfv_err(ret);
}


inline int String::_do_prinfv(const char* s,va_list vl)
{
	static const int _ndefault_buffer_size=1024;
	char _buffer[_ndefault_buffer_size];

	int _count=vsnprintf(_buffer,_ndefault_buffer_size,s,vl);

	if(_count<0)
	{		
		return String_do_prinfv_err(*this);
	}
	else if(_count<_ndefault_buffer_size)
	{
		char* _pnewstr=StringPool::current().str_dup(_buffer,_count);
		StringPool::current().str_free(m_ptr);
		m_ptr=_pnewstr;
		return _count;
	}
	else
	{
		return String_do_prinfv_try(*this,s,vl);
	}
}

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
	m_ptr=StringPool::current().str_dup(p1,p2-p1);
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
	m_ptr=StringPool::current().str_dup((char*)p1,p2-p1);
}

void String::assign(const char* p1,uint32_t n)
{
	_do_assign(p1,n);
}

void String::assign(const char* p1,const char* p2)
{
	_do_assign(p1,p2-p1);
}

void String::assign(const unsigned char* p1,uint32_t n)
{
	_do_assign((const char*)p1,n);
}

void String::assign(const unsigned char* p1,const unsigned char* p2)
{
	assign((char*)p1,p2-p1);
}


String::String(const wchar_t* p)
{
	m_ptr=StringPool::current().str_empty();
	_do_assign(p,std::char_traits<wchar_t>::length(p));
}

String::String(const wchar_t* p,uint32_t n)
{
	m_ptr=StringPool::current().str_empty();
	_do_assign(p,n);
}

String::String(const wchar_t* p1,const wchar_t* p2)
{
	m_ptr=StringPool::current().str_empty();
	_do_assign(p1,p2-p1);
}

String::~String()
{
	StringPool::current().str_free(m_ptr);
}

size_t String::size() const
{
	return ::strlen(m_ptr);
}

size_t String::length() const
{
	return ::strlen(m_ptr);
}



const String& String::PrintfImpl(const char* s,...)
{
	va_list arglist;
	va_start(arglist,s);
	_do_prinfv(s,arglist);
	va_end(arglist);
	return *this;
}

String& String::PrintfV(const char* s,va_list arglist)
{
	_do_prinfv(s,arglist);
	return *this;
}

String String::FormatV(const char* s,va_list arglist)
{
	String ret;
	ret._do_prinfv(s,arglist);
	return ret;
}

String String::FormatImpl(const char* s,...)
{
	va_list arglist;
	String ret;
	va_start(arglist,s);
	ret._do_prinfv(s,arglist);
	va_end(arglist);
	return ret;
}

void String::append(const char* p,uint32_t n)
{
	_do_append(p,n);
}

void String::append(const char* p1,const char* p2)
{
	_do_append(p1,p2-p1);
}


String& String::operator=(const char* p)
{
	_do_assign(p,::strlen(p));
	return *this;
}

String& String::operator=(const String& p)
{
	_do_assign(p.c_str(),p.size());
	return *this;
}

String& String::operator=(const wchar_t* p)
{
	_do_assign(p,std::char_traits<wchar_t>::length(p));
	return *this;
}

String& String::operator+=(const char* p)
{
	_do_append(p,::strlen(p));
	return *this;
}

String& String::operator+=(const String& p)
{
	_do_append(p.c_str(),p.size());
	return *this;
}

String& String::operator+=(const wchar_t* p)
{
	_do_append(p,std::char_traits<wchar_t>::length(p));
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
	_do_append(&v,1);
	return (*this);
}

String& String::operator<<(int32_t v)
{
	_do_format_integer(v);
	return (*this);
}

String& String::operator<<(int64_t v)
{
	_do_format_integer(v);
	return (*this);
}

String& String::operator<<(uint32_t v)
{
	_do_format_integer(v);
	return (*this);
}

String& String::operator<<(uint64_t v)
{
	_do_format_integer(v);
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
	append(v,::strlen(v));
	return (*this);
}

String& String::operator<<(const wchar_t* p)
{
	_do_append(p,std::char_traits<wchar_t>::length(p));
	return (*this);
}

String& String::operator<<(const String& v)
{
	_do_append(v.c_str(),v.size());
	return (*this);
}

String& String::operator<<(const StringBuffer<char>& o)
{
	_do_append(o.data(),o.size());
	return *this;
}

String& String::operator<<(const StringBuffer<unsigned char>& o)
{
	_do_append((char*)o.data(),o.size());
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
