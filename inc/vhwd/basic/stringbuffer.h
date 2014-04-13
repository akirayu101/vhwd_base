#ifndef __H_VHWD_STRINGBUFFER__
#define __H_VHWD_STRINGBUFFER__

#include "vhwd/config.h"
#include "vhwd/collection/array.h"
#include "vhwd/memory.h"

VHWD_ENTER


template<typename T>
class VHWD_DLLIMPEXP StringBuffer : public arr_1t<T,AllocatorUsePool<T,MemPoolPaging> >
{
public:
	typedef arr_1t<T,AllocatorUsePool<T,MemPoolPaging> > basetype;
	using basetype::assign;

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
};


VHWD_LEAVE
#endif
