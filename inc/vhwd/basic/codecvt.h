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

#ifdef VHWD_WINDOWS
#include "windows.h"
#else
#include <iconv.h>
#include <errno.h>
#endif

VHWD_ENTER

template<int N> class char_implN;
template<> class char_implN<1>
{
public:
	typedef unsigned char type;
};
template<> class char_implN<2>
{
public:
	typedef uint16_t type;
};
template<> class char_implN<4>
{
public:
	typedef uint32_t type;
};
template<typename T> class char_impl : public char_implN<sizeof(T)> {};


class VHWD_DLLIMPEXP IConv
{
public:


	static bool unicode_to_gbk(StringBuffer<unsigned char>& aa_,const uint16_t* pw_,size_t ln_);
	static bool unicode_to_gbk(StringBuffer<unsigned char>& aa_,const uint32_t* pw_,size_t ln_);
	static bool unicode_to_gbk(StringBuffer<unsigned char>& aa_,const unsigned char* pw_,size_t ln_)
	{
		return false;
	}

	static bool gbk_to_unicode(StringBuffer<uint16_t>& aw_,const unsigned char* pa_,size_t ln_);
	static bool gbk_to_unicode(StringBuffer<uint32_t>& aw_,const unsigned char* pa_,size_t ln_);
	static bool gbk_to_unicode(StringBuffer<unsigned char>& aw_,const unsigned char* pa_,size_t ln_)
	{
		return false;
	}

	static bool unicode_to_utf8(StringBuffer<unsigned char>& aa_,const uint16_t* pw_,size_t ln_);
	static bool unicode_to_utf8(StringBuffer<unsigned char>& aa_,const uint32_t* pw_,size_t ln_);
	static bool unicode_to_utf8(StringBuffer<unsigned char>& aa_,const unsigned char* pw_,size_t ln_)
	{
		return false;
	}

	static bool utf8_to_unicode(StringBuffer<uint16_t>& aw_,const unsigned char* pa_,size_t ln_);
	static bool utf8_to_unicode(StringBuffer<uint32_t>& aw_,const unsigned char* pa_,size_t ln_);
	static bool utf8_to_unicode(StringBuffer<unsigned char>& aw_,const unsigned char* pa_,size_t ln_)
	{
		return false;
	}


	template<typename WC>
	static bool unicode_to_gbk(StringBuffer<char>& aa_,const WC* pw_,size_t ln_)
	{
		typedef typename char_impl<char>::type char_mbs;
		typedef typename char_impl<WC>::type char_wcs;
		return unicode_to_gbk((StringBuffer<char_mbs>&)aa_,(const char_wcs*)pw_,ln_);
	}

	template<typename WC>
	static bool gbk_to_unicode(StringBuffer<WC>& aw_,const char* pa_,size_t ln_)
	{
		typedef typename char_impl<char>::type char_mbs;
		typedef typename char_impl<WC>::type char_wcs;
		return gbk_to_unicode((StringBuffer<char_wcs>&)aw_,(const char_mbs*)pa_,ln_);
	}

	template<typename WC>
	static bool unicode_to_utf8(StringBuffer<char>& aa_,const WC* pw_,size_t ln_)
	{
		typedef typename char_impl<char>::type char_mbs;
		typedef typename char_impl<WC>::type char_wcs;
		return unicode_to_utf8((StringBuffer<char_mbs>&)aa_,(const char_wcs*)pw_,ln_);
	}

	template<typename WC>
	static bool utf8_to_unicode(StringBuffer<WC>& aw_,const char* pa_,size_t ln_)
	{
		typedef typename char_impl<char>::type char_mbs;
		typedef typename char_impl<WC>::type char_wcs;
		return utf8_to_unicode((StringBuffer<char_wcs>&)aw_,(const char_mbs*)pa_,ln_);
	}


	template<typename WC>
	static bool unicode_to_ansi(StringBuffer<char>& aa_,const WC* pw_,size_t ln_)
	{
#ifdef VHWD_WINDOWS
		return unicode_to_gbk(aa_,pw_,ln_);
#else
		return unicode_to_utf8(aa_,pw_,ln_);
#endif
	}

	template<typename WC>
	static bool ansi_to_unicode(StringBuffer<WC>& aw_,const char* pa_,size_t ln_)
	{
#ifdef VHWD_WINDOWS
		return gbk_to_unicode(aw_,pa_,ln_);
#else
		return utf8_to_unicode(aw_,pa_,ln_);
#endif
	}

	static bool utf8_to_ansi(StringBuffer<char>& aa_,const char* pa_,size_t ln_);

	static bool ansi_to_utf8(StringBuffer<char>& aa_,const char* pa_,size_t ln_);

	static bool utf8_to_gbk(StringBuffer<char>& aa_,const char* pa_,size_t ln_);

	static bool gbk_to_utf8(StringBuffer<char>& aa_,const char* pa_,size_t ln_);
};



VHWD_LEAVE
#endif
