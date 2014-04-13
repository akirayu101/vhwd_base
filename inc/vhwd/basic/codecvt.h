#ifndef __H_VHWD_BASIC_CODECVT__
#define __H_VHWD_BASIC_CODECVT__

#include "vhwd/config.h"
#include "vhwd/basic/string.h"
#include "vhwd/basic/stringbuffer.h"
#include <locale>

VHWD_ENTER


// CodeCvt is used to convert between char and wchar_t.
template<typename T>
class VHWD_DLLIMPEXP CodeCvt
{
public:

	typedef T intern_type;
	typedef char extern_type;
	typedef std::codecvt<intern_type,extern_type,std::mbstate_t> facet_type;

	CodeCvt();

	void Init(const String& loc_name);

	static CodeCvt& current();

	bool s2ws(StringBuffer<intern_type>& sb,const extern_type* si,size_t ln);
	bool ws2s(StringBuffer<extern_type>& sb,const intern_type* si,size_t ln);

	StringBuffer<intern_type> s2ws(const extern_type* se,size_t ln)
	{
		StringBuffer<intern_type> sb;
		s2ws(sb,se,ln);
		return sb;
	}

	StringBuffer<intern_type> s2ws(const extern_type* se)
	{
		StringBuffer<intern_type> sb;
		s2ws(sb,se,std::char_traits<extern_type>::length(se));
		return sb;
	}

	StringBuffer<intern_type> s2ws(const String& se)
	{
		StringBuffer<intern_type> sb;
		s2ws(sb,se.c_str(),se.size());
		return sb;
	}

	String ws2s(const intern_type* si,size_t ln)
	{
		StringBuffer<extern_type> sb;
		ws2s(sb,si,ln);
		return sb;
	}

	String ws2s(const intern_type* si)
	{
		StringBuffer<extern_type> sb;
		ws2s(sb,si,std::char_traits<intern_type>::length(si));
		return sb;
	}

	String ws2s(const StringBuffer<intern_type>& si)
	{
		StringBuffer<extern_type> sb;
		ws2s(sb,si.data(),si.size());
		return sb;
	}


private:
	std::locale loc;
};

template<>
class VHWD_DLLIMPEXP CodeCvt<char>
{
public:

	typedef char intern_type;
	typedef char extern_type;

	static CodeCvt& current();

	bool s2ws(StringBuffer<intern_type>& sb,const extern_type* si,size_t ln)
	{
		sb.assign(si,ln);return true;
	}

	bool ws2s(StringBuffer<char>& sb,const intern_type* si,size_t ln)
	{
		sb.assign(si,ln);
		return true;
	}

	StringBuffer<intern_type> s2ws(const extern_type* se,size_t ln)
	{
		StringBuffer<intern_type> sb(se,ln);
		return sb;
	}

	StringBuffer<intern_type> s2ws(const extern_type* se)
	{
		return se;
	}

	StringBuffer<intern_type> s2ws(const String& se)
	{
		return se;
	}

	String ws2s(const intern_type* si,size_t ln)
	{
		StringBuffer<extern_type> sb(si,ln);
		return sb;
	}

	String ws2s(const intern_type* si)
	{
		return si;
	}

	String ws2s(const StringBuffer<intern_type>& si)
	{
		return si;
	}
};


template<>
class VHWD_DLLIMPEXP CodeCvt<unsigned char>
{
public:

	typedef unsigned char intern_type;
	typedef char extern_type;

	static CodeCvt& current();

	bool s2ws(StringBuffer<intern_type>& sb,const extern_type* si,size_t ln)
	{
		sb.assign((intern_type*)si,ln);return true;
	}

	bool ws2s(StringBuffer<extern_type>& sb,const intern_type* si,size_t ln)
	{
		sb.assign((extern_type*)si,ln);
		return true;
	}

	StringBuffer<intern_type> s2ws(const extern_type* se,size_t ln)
	{
		StringBuffer<intern_type> sb((intern_type*)se,ln);
		return sb;
	}

	StringBuffer<intern_type> s2ws(const extern_type* se)
	{
		return (intern_type*)se;
	}

	StringBuffer<intern_type> s2ws(const String& se)
	{
		return se;
	}

	String ws2s(const intern_type* si,size_t ln)
	{
		StringBuffer<extern_type> sb((extern_type*)si,ln);
		return sb;
	}

	String ws2s(const intern_type* si)
	{
		return (extern_type*)si;
	}

	String ws2s(const StringBuffer<intern_type>& si)
	{
		return si;
	}
};


VHWD_LEAVE
#endif
