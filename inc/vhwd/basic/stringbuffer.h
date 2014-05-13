// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_STRINGBUFFER__
#define __H_VHWD_STRINGBUFFER__

#include "vhwd/config.h"
#include "vhwd/collection/array.h"
#include "vhwd/collection/array_xt.h"
#include "vhwd/memory.h"

VHWD_ENTER

enum
{
	FILE_BINARY,
	FILE_TEXT,
	FILE_TEXT_ANSI,
	FILE_TEXT_UTF8,
	FILE_TEXT_UTF16_BE,
	FILE_TEXT_UTF16_LE,

	FILE_MASK=(1<<10)-1,
	FILE_TEXT_BOM=1<<10
};


template<typename T>
class VHWD_DLLIMPEXP StringBuffer : public arr_1t<T,AllocatorUsePool<T,MemPoolPaging> >
{
public:
	typedef arr_1t<T,AllocatorUsePool<T,MemPoolPaging> > basetype;
	typedef typename basetype::size_type size_type;

	using basetype::assign;
	using basetype::resize;
	using basetype::data;
	using basetype::size;

	StringBuffer(){}

	StringBuffer(const T* p1);
	StringBuffer(const T* p1,size_t ln){assign(p1,ln);}
	StringBuffer(const T* p1,const T* p2){assign(p1,p2);}
	StringBuffer(const StringBuffer& o):basetype(o){}

#if defined(VHWD_C11)
	StringBuffer(StringBuffer&& p){swap(p);}
	const StringBuffer& operator=(StringBuffer&& p){swap(p);return *this;}
#endif

	StringBuffer(const String& o){(*this)=o;}
	const StringBuffer& operator=(const String& o);

	const StringBuffer& operator+=(const StringBuffer& o){append(o.begin(),o.end());return *this;}
	const StringBuffer& operator+=(const String& o);

	StringBuffer& operator<<(const String& v){(*this)+=v;return *this;}

	bool load(const String& file,int t=FILE_TEXT);
	bool save(const String& file,int t=FILE_TEXT);

};


VHWD_LEAVE
#endif
