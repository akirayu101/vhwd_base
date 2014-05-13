// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_BASIC_CODECVT__
#define __H_VHWD_BASIC_CODECVT__

#include "vhwd/config.h"
#include "vhwd/basic/string.h"
#include "vhwd/basic/stringbuffer.h"
#include <locale>

#ifdef _WIN32
#include "windows.h"
#else
#include <iconv.h>
#include <errno.h>
#endif

VHWD_ENTER

// CodeCvt is used to convert between char and wchar_t.
template<typename T>
class VHWD_DLLIMPEXP CodeCvt
{
public:

	typedef T char_wcs;
	typedef char char_mbs;

	static bool s2ws(StringBuffer<char_wcs>& sb,const char_mbs* p1,size_t ln);
	static bool ws2s(StringBuffer<char_mbs>& sb,const char_wcs* p1,size_t ln);

	static StringBuffer<char_wcs> s2ws(const char_mbs* p1,size_t ln)
	{
		StringBuffer<char_wcs> sb;
		s2ws(sb,p1,ln);
		return sb;
	}

	static StringBuffer<char_wcs> s2ws(const char_mbs* p1)
	{
		StringBuffer<char_wcs> sb;
		s2ws(sb,p1,std::char_traits<char_mbs>::length(p1));
		return sb;
	}

	static StringBuffer<char_wcs> s2ws(const String& p1)
	{
		StringBuffer<char_wcs> sb;
		s2ws(sb,p1.c_str(),p1.size());
		return sb;
	}

	static String ws2s(const char_wcs* p1,size_t ln)
	{
		StringBuffer<char_mbs> sb;
		ws2s(sb,p1,ln);
		return sb;
	}

	static StringBuffer<char_wcs> s2ws(const char_mbs* p1,const char_mbs* p2)
	{
		return s2ws(p1,safe_distance(p1,p2));
	}

	static String ws2s(const char_wcs* p1,const char_wcs* p2)
	{
		return ws2s(p1,safe_distance(p1,p2));
	}

	static String ws2s(const char_wcs* p1)
	{
		StringBuffer<char_mbs> sb;
		ws2s(sb,p1,std::char_traits<char_wcs>::length(p1));
		return sb;
	}

	static String ws2s(const StringBuffer<char_wcs>& p1)
	{
		StringBuffer<char_mbs> sb;
		ws2s(sb,p1.data(),p1.size());
		return sb;
	}

};


VHWD_LEAVE
#endif
