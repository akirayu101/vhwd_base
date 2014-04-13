#include "string_impl.h"
#include "vhwd/basic/stringbuffer.h"
#include "vhwd/basic/codecvt.h"

#include <cstring>
#include <clocale>

VHWD_ENTER

template<typename T>
StringBuffer<T>::StringBuffer(const T* p1)
{
	assign(p1,std::char_traits<T>::length(p1));
}


template<typename T>
class StringBufferHelper
{
public:

	static void equal(StringBuffer<T>& h,const String& o)
	{
		CodeCvt<T>::current().s2ws(h,o.c_str(),o.size());
	}

	static void eqadd(StringBuffer<T>& h,const String& o)
	{
		StringBuffer<T> tmp;
		CodeCvt<T>::current().s2ws(tmp,o.c_str(),o.size());
		h+=tmp;
	}
};

template<>
class StringBufferHelper<char>
{
public:
	typedef char T;

	static void equal(StringBuffer<T>& h,const String& o)
	{
		h.assign(o.c_str(),o.size());
	}

	static void eqadd(StringBuffer<T>& h,const String& o)
	{
		h.append(o.c_str(),o.size());
	}
};


template<>
class StringBufferHelper<unsigned char>
{
public:
	typedef unsigned char T;

	static void equal(StringBuffer<T>& h,const String& o)
	{
		h.assign((T*)o.c_str(),o.size());
	}

	static void eqadd(StringBuffer<T>& h,const String& o)
	{
		h.append((T*)o.c_str(),o.size());
	}
};


template<typename T>
const StringBuffer<T>& StringBuffer<T>::operator=(const String& o)
{
	StringBufferHelper<T>::equal((*this),o);
	return *this;
}

template<typename T>
const StringBuffer<T>& StringBuffer<T>::operator+=(const String& o)
{
	StringBufferHelper<T>::eqadd((*this),o);
	return *this;
}


template class StringBuffer<char>;
template class StringBuffer<unsigned char>;
template class StringBuffer<short>;
template class StringBuffer<unsigned short>;
template class StringBuffer<int>;
template class StringBuffer<unsigned int>;

template class StringBuffer<wchar_t>;


VHWD_LEAVE
