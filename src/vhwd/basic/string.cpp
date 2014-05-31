
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


String::String()
{
	m_pStr=StringPool::current().str_empty();
}

String::String(const StringBuffer<char>& o)
{
	m_pStr=StringPool::current().str_dup(o.data(),o.size());
}

String::String(const StringBuffer<unsigned char>& o)
{
	m_pStr=StringPool::current().str_dup((char*)o.data(),o.size());
}

String::String(const String& o)
{
	m_pStr=StringPool::current().str_dup(o.c_str());
}

String::String(const char* p)
{
	m_pStr=StringPool::current().str_dup(p);
}

String::String(const char* p1,uint32_t n)
{
	m_pStr=StringPool::current().str_dup(p1,n);
}

String::String(const char* p1,const char* p2)
{
	m_pStr=StringPool::current().str_dup(p1,safe_distance(p1,p2));
}

String::String(const unsigned char* p)
{
	m_pStr=StringPool::current().str_dup((char*)p);
}

String::String(const unsigned char* p1,uint32_t n)
{
	m_pStr=StringPool::current().str_dup((char*)p1,n);
}

String::String(const unsigned char* p1,unsigned const char* p2)
{
	m_pStr=StringPool::current().str_dup((char*)p1,safe_distance(p1,p2));
}

void String::assign(const char* p1,uint32_t n)
{
	char* _pnewstr=StringPool::current().str_dup(p1,n);
	StringPool::current().str_free(m_pStr);
	m_pStr=_pnewstr;
}

void String::assign(const char* p1,const char* p2)
{
	assign(p1,safe_distance(p1,p2));
}

void String::assign(const unsigned char* p1,uint32_t n)
{
	char* _pnewstr=StringPool::current().str_dup((char*)p1,n);
	StringPool::current().str_free(m_pStr);
	m_pStr=_pnewstr;
}

void String::assign(const unsigned char* p1,const unsigned char* p2)
{
	assign((char*)p1,safe_distance(p1,p2));
}


String::String(const wchar_t* p)
{
	m_pStr=StringPool::current().str_empty();
	StringBuffer<char> sb;
	IConv::unicode_to_ansi(sb,p,std::char_traits<wchar_t>::length(p));
	(*this)=sb;
	//(*this)=CodeCvt<wchar_t>::ws2s(p);
}

String::String(const wchar_t* p,uint32_t n)
{
	m_pStr=StringPool::current().str_empty();
	StringBuffer<char> sb;
	IConv::unicode_to_ansi(sb,p,n);
	(*this)=sb;
	//(*this)=CodeCvt<wchar_t>::ws2s(p,n);
}

String::String(const wchar_t* p1,const wchar_t* p2)
{
	m_pStr=StringPool::current().str_empty();
	//(*this)=CodeCvt<wchar_t>::ws2s(p1,p2);
	StringBuffer<char> sb;
	IConv::unicode_to_ansi(sb,p1,p2-p1);
	(*this)=sb;
}

String::~String()
{
	StringPool::current().str_free(m_pStr);
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
	StringPool::current().str_free(m_pStr);
	m_pStr=_pnewstr;
}

void String::append(const char* p1,const char* p2)
{
	char* _pnewstr=StringPool::current().str_cat(c_str(),size(),p1,safe_distance(p1,p2));
	StringPool::current().str_free(m_pStr);
	m_pStr=_pnewstr;
}


String& String::operator=(const char* p)
{
	char* _pnewstr=StringPool::current().str_dup(p);
	StringPool::current().str_free(m_pStr);
	m_pStr=_pnewstr;
	return *this;
}

String& String::operator=(const String& p)
{
	char* _pnewstr=StringPool::current().str_dup(p.c_str());
	StringPool::current().str_free(m_pStr);
	m_pStr=_pnewstr;
	return *this;
}

String& String::operator=(const wchar_t* p)
{
	//(*this)=CodeCvt<wchar_t>::ws2s(p);

	StringBuffer<char> sb;
	IConv::unicode_to_ansi(sb,p,std::char_traits<wchar_t>::length(p));
	(*this)=sb;

	return *this;
}

String& String::operator+=(const char* p)
{
	char* _pnewstr=StringPool::current().str_cat(m_pStr,p);
	StringPool::current().str_free(m_pStr);
	m_pStr=_pnewstr;
	return *this;
}

String& String::operator+=(const String& p)
{
	char* _pnewstr=StringPool::current().str_cat(m_pStr,p.c_str());
	StringPool::current().str_free(m_pStr);
	m_pStr=_pnewstr;
	return *this;
}


String& String::operator+=(const wchar_t* p)
{
	//(*this)+=CodeCvt<wchar_t>::ws2s(p);

	StringBuffer<char> sb;
	IConv::unicode_to_ansi(sb,p,std::char_traits<wchar_t>::length(p));
	(*this)+=sb;

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
	//(*this)+=CodeCvt<wchar_t>::ws2s(v);

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
//
//#ifndef _WIN32
//
//String String::ansi_to_utf8(const String& o)
//{
//	return o;
//}
//
//String String::utf8_to_ansi(const String& o)
//{
//	return o;
//}
//
//String String::ansi_to_utf8(const char* o,size_t s)
//{
//	return String(o,s);
//}
//
//String String::utf8_to_ansi(const char* o,size_t s)
//{
//	return String(o,s);
//}
//
//
//bool String::ansi_to_utf8(StringBuffer<char>& o,const char* p1,size_t ln)
//{
//	o.resize(ln);
//	if(ln>0) memcpy(&o[0],p1,ln);
//	return true;
//}
//
//bool String::utf8_to_ansi(StringBuffer<char>& o,const char* p1,size_t ln)
//{
//	o.resize(ln);
//	if(ln>0) memcpy(&o[0],p1,ln);
//	return true;
//}
//
//bool String::ansi_to_utf8(StringBuffer<char>& o,StringBuffer<char>& s)
//{
//	if(&o==&s) return true;
//	return ansi_to_utf8(o,s.data(),s.size());
//}
//
//bool String::utf8_to_ansi(StringBuffer<char>& o,StringBuffer<char>& s)
//{
//	if(&o==&s) return true;
//	return utf8_to_ansi(o,s.data(),s.size());
//}
//
//bool String::wstr_to_utf8(StringBuffer<char>& va,StringBuffer<wchar_t>& vh)
//{
//	return CodeCvt<wchar_t>::ws2s(va,vh.data(),vh.size());
//}
//
//bool String::utf8_to_wstr(StringBuffer<wchar_t>& vh,StringBuffer<char>& o)
//{
//	return CodeCvt<wchar_t>::s2ws(vh,o.data(),o.size());
//}
//
//#else
//
//bool String::wstr_to_utf8(StringBuffer<char>& va,StringBuffer<wchar_t>& vh)
//{
//	if(vh.empty())
//	{
//		va.resize(0);
//		return true;
//	}
//
//	va.resize(vh.size()*3+1);
//	int wl=::WideCharToMultiByte(CP_UTF8,0,&vh[0],vh.size(),&va[0],va.size(),NULL,NULL);
//	va.resize(wl);
//
//	if(wl<=0)
//	{
//		vh.resize(0);
//		System::CheckError("wstr_to_utf8");
//		return false;
//	}
//	vh.resize(wl);
//	return true;
//}
//
//bool String::utf8_to_wstr(StringBuffer<wchar_t>& vh,StringBuffer<char>& va)
//{
//	if(va.empty())
//	{
//		vh.resize(0);
//		return true;
//	}
//
//	char* p1=va.data();size_t ln=va.size();
//	vh.resize(ln+1);
//	int wl=MultiByteToWideChar(CP_UTF8,0,p1,ln,&vh[0],vh.size());
//
//	if(wl<=0)
//	{
//		vh.resize(0);
//		System::CheckError("utf8_to_wstr");
//		return false;
//	}
//	vh.resize(wl);
//	return true;
//}
//
//bool String::ansi_to_utf8(StringBuffer<char>& o,StringBuffer<char>& s)
//{
//	return ansi_to_utf8(o,s.data(),s.size());
//}
//
//bool String::utf8_to_ansi(StringBuffer<char>& o,StringBuffer<char>& s)
//{
//	return utf8_to_ansi(o,s.data(),s.size());
//}
//
//bool String::ansi_to_utf8(StringBuffer<char>& va,const char* p1,size_t ln)
//{
//	if(ln==0)
//	{
//		va.resize(0);
//		return true;
//	}
//
//	StringBuffer<wchar_t> vh;
//	if(!CodeCvt<wchar_t>::s2ws(vh,p1,ln))
//	{
//		return false;
//	}
//
//	va.resize(vh.size()*3+1);
//	int wl=::WideCharToMultiByte(CP_UTF8,0,&vh[0],vh.size(),&va[0],va.size(),NULL,NULL);
//
//	if(wl<=0)
//	{
//		va.resize(0);
//		System::CheckError(String::Format("ansi_to_utf8:%s",p1));
//		return false;
//	}
//
//	va[wl]='\0';
//	va.resize(wl);
//	return true;;
//}
//
//bool String::utf8_to_ansi(StringBuffer<char>& va,const char* p1,size_t ln)
//{
//
//	StringBuffer<wchar_t> vh;
//	vh.resize(ln+1);
//	int wl=MultiByteToWideChar(CP_UTF8,0,p1,ln,&vh[0],vh.size());
//	if(wl<=0)
//	{
//		System::CheckError(String::Format("utf8_to_ansi:%s",p1));
//		va.resize(0);
//		return false;
//	}
//
//	return CodeCvt<wchar_t>::ws2s(va,&vh[0],wl);
//}
//
//
//String String::ansi_to_utf8(const char* o,size_t s)
//{
//	StringBuffer<char> va;
//	if(ansi_to_utf8(va,o,s))
//	{
//		return va;
//	}
//	else
//	{
//		System::LogTrace("ansi_to_utf8 failed:%s",o);
//		return "???";
//	}
//
//}
//
//String String::ansi_to_utf8(const String& o)
//{
//	return ansi_to_utf8(o.c_str(),o.size());
//}
//
//String String::utf8_to_ansi(const char* o,size_t s)
//{
//	if(s==0) return "";
//
//	StringBuffer<char> va;
//	if(utf8_to_ansi(va,o,s))
//	{
//		return va;
//	}
//	else
//	{
//		System::LogTrace("utf8_to_ansi failed:%s",o);
//		return "???";
//	}
//}
//
//String String::utf8_to_ansi(const String& o)
//{
//	return utf8_to_ansi(o.c_str(),o.size());
//}
//
//
//
//#endif



String String::substr(size_t pos,size_t len) const
{
	wassert(pos<size());
	String s(m_pStr+pos,std::min(len,size()-pos));
	return s;
}


int String::replace(char c1,char c2)
{
	int n=0;
	StringBuffer<char> sb(*this);
	for(size_t i=0;i<sb.size();i++)
	{
		if(sb[i]==c1)
		{
			sb[i]=c2;n++;
		}
	}	
	//std::replace(sb.begin(),sb.end(),c1,c2);
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

	const char* p1=m_pStr;
	const char* p2=m_pStr+size();
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


VHWD_LEAVE
