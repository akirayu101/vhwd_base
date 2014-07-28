// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_STRING__
#define __H_VHWD_STRING__

#include <cstring>
#include <cstdarg>

#include "vhwd/config.h"
#include "vhwd/basic/language.h"
#include "vhwd/basic/system.h"
#include "vhwd/basic/string_detail.h"

VHWD_ENTER

template<typename T>
class VHWD_DLLIMPEXP StringBuffer;

template<typename T>
inline ptrdiff_t safe_distance(const T* p1,const T* p2)
{
	ptrdiff_t n=p2-p1;
	if(n<0)
	{
		return 0;
	}
	return n;
}


class VHWD_DLLIMPEXP String
{
public:

	String();
	String(const String& o);
	String(const StringBuffer<char>& o);
	String(const StringBuffer<unsigned char>& o);

	String(const char* p);
	String(const char* p,uint32_t n);
	String(const char* p1,const char* p2);

	String(const unsigned char* p);
	String(const unsigned char* p,uint32_t n);
	String(const unsigned char* p1,unsigned const char* p2);

	String(const wchar_t* p);
	String(const wchar_t* p,uint32_t n);
	String(const wchar_t* p1,const wchar_t* p2);

	void assign(const char* p,uint32_t n);
	void assign(const char* p1,const char* p2);

	void assign(const unsigned char* p,uint32_t n);
	void assign(const unsigned char* p1,const unsigned char* p2);

	void append(const char* p,uint32_t n);
	void append(const char* p1,const char* p2);

	template<typename T>
	String(const std::basic_string<T>& o);

#if defined(VHWD_C11)
	String(String&& p)
	{
		m_pStr=p.m_pStr;
		p.m_pStr=NULL;
	}
	const String& operator=(String&& p)
	{
		swap(p);
		return *this;
	}
#endif

	~String();

	void swap(String& s1)
	{
		std::swap(m_pStr,s1.m_pStr);
	}

	const char* c_str() const;
	size_t size() const;
	size_t length() const;

	bool empty() const
	{
		return size()==0;
	}

	String& operator=(const String& p);
	String& operator=(const char* p);
	String& operator=(const wchar_t* p);

	template<typename T>
	String& operator=(const std::basic_string<T>& o)
	{
		return (*this)=o.c_str();
	}

	String& operator+=(const String& p);
	String& operator+=(const char* p);
	String& operator+=(const wchar_t* p);

	template<typename T>
	String& operator+=(const std::basic_string<T>& o)
	{
		return (*this)+=o.c_str();
	}


#define STRING_FORMAT_IMPL(X,Y) return FormatImpl(X)
	STRING_FORMAT_FUNCTIONS(static String Format,STRING_FORMAT_IMPL, )

#define STRING_PRINTF_IMPL(X,Y) return PrintfImpl(X)
	STRING_FORMAT_FUNCTIONS(const String& Printf,STRING_PRINTF_IMPL, )

	static String FormatV(const char* s,va_list vl);
	String& PrintfV(const char* s,va_list vl);

	friend VHWD_DLLIMPEXP std::ostream& operator<<(std::ostream&o,const String& s);
	friend VHWD_DLLIMPEXP std::istream& operator>>(std::istream&o,String& s);

	bool ToNumber(int64_t* val) const;
	bool ToNumber(int32_t* val) const;
	bool ToNumber(float32_t* val) const;
	bool ToNumber(float64_t* val) const;

	String& operator<<(char v);
	String& operator<<(int32_t v);
	String& operator<<(int64_t v);
	String& operator<<(uint32_t v);
	String& operator<<(uint64_t v);
	String& operator<<(float v);
	String& operator<<(double v);
	String& operator<<(const char* v);
	String& operator<<(const wchar_t* v);
	String& operator<<(const String& v);

	String& operator<<(const StringBuffer<char>& o);
	String& operator<<(const StringBuffer<unsigned char>& o);

	template<typename T>
	String& operator<<(const std::basic_string<T>& v)
	{
		return (*this)<<v.c_str();
	}

	static const size_t npos=(size_t)(-1);

	String substr(size_t pos,size_t len=npos) const;

	int replace(char c1,char c2);
	int replace(const String& s1,const String& c2);

	int find (char c, int pos = 0) const;

private:
	static String FormatImpl(const char* s,...);
	const String& PrintfImpl(const char* s,...);

	char* m_pStr;
};


VHWD_DLLIMPEXP std::ostream& operator<<(std::ostream&o,const String& s);
VHWD_DLLIMPEXP std::istream& operator>>(std::istream&o,String& s);

template<typename T>
String::String(const std::basic_string<T>& o):m_pStr(NULL)
{
	(*this)=o.c_str();
}

String VHWD_DLLIMPEXP operator+(const String& lhs,const String& rhs);
String VHWD_DLLIMPEXP operator+(const char* lhs,const String& rhs);
String VHWD_DLLIMPEXP operator+(const String& lhs,const char* rhs);

template<typename T>
String operator+(const std::basic_string<T>& lhs,const String& rhs)
{
	String ret(lhs.c_str());
	ret+=rhs;
	return ret;
}

template<typename T>
String operator+(const String& lhs,const std::basic_string<T>& rhs)
{
	String ret(lhs);
	ret+=rhs.c_str();
	return ret;
}

#define STRING_REL_OP2(X)	\
	inline bool operator X (const String& lhs,const String& rhs){return ::strcmp(StringParamCast::g(lhs),StringParamCast::g(rhs)) X 0;}\
	inline bool operator X (const char* lhs,const String& rhs){return ::strcmp(StringParamCast::g(lhs),StringParamCast::g(rhs)) X 0;}\
	inline bool operator X (const String& lhs,const char* rhs){return ::strcmp(StringParamCast::g(lhs),StringParamCast::g(rhs)) X 0;}\

STRING_REL_OP2(==)
STRING_REL_OP2(!=)
STRING_REL_OP2(>=)
STRING_REL_OP2(<=)
STRING_REL_OP2(<)
STRING_REL_OP2(>)


template<> class hash_t<String>
{
public:
	inline uint32_t operator()(const String& v)
	{
		return hash_raw<1>::hash(v.c_str(),v.size());
	}
};

template<typename T> class hash_t<StringBuffer<T> >
{
public:
	inline uint32_t operator()(const String& v)
	{
		return hash_raw<1>::hash(v.c_str(),sizeof(T)*v.size());
	}
};

template<> class hash_t<char*>
{
public:
	inline uint32_t operator()(const char* v)
	{
		return hash_raw<1>::hash(v,::strlen(v));
	}
};


inline const char* StringParamCast::g(const String& v)
{
	return v.c_str();
}

VHWD_DLLIMPEXP extern const char const_empty_buffer[64];

VHWD_LEAVE


#endif
