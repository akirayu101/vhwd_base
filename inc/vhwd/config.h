// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.


#ifndef __H_VHWD_CONFIG__
#define __H_VHWD_CONFIG__

#include "tl.h"

// enable c++11
//#define VHWD_C11

// use MemPoolDebug to detect memory leaks.
//#define VHWD_MEMDEBUG

// use MemPoolPaging as default memory allocator
//#define VHWD_MEMUSEPOOL


//#define VHWD_NO64BIT_ATOMIC 1

#if !defined(_WIN32) || defined(_MSC_VER)
#define VHWD_USE_COROUTINE
#endif

#define MACRO2STR(x) MACRO2STR_ESC(x)
#define MACRO2STR_ESC(x) #x

#ifdef VHWD_DLL
#ifdef _MSC_VER
#define VHWD_EXPORT __declspec(dllexport)
#define VHWD_IMPORT __declspec(dllimport)
#else
#define VHWD_EXPORT __attribute__((dllexport))
#define VHWD_IMPORT __attribute__((dllimport))
#endif
#else
#define VHWD_EXPORT
#define VHWD_IMPORT
#endif

#ifdef _MSC_VER
#pragma warning(disable:4251)
#endif

#ifdef VHWD_BUILDING
#define VHWD_DLLIMPEXP VHWD_EXPORT
#else
#define VHWD_DLLIMPEXP VHWD_IMPORT
#endif



#ifdef _MSC_VER

typedef signed char int8_t;
typedef unsigned char uint8_t;

typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;

typedef tl::meta_if<sizeof(void*)==4,int32_t,int64_t>::type intptr_t;

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#else
#include <sys/types.h>
#include <stdint.h>
#endif


typedef float float32_t;
typedef double float64_t;

#define TRANSLATE(x) vhwd::Translate((x))

#ifndef _kT
#define _kT(x) x
#endif

#ifndef _hT
#define _hT(x) TRANSLATE(x)
#endif


#if defined(VHWD_MEMDEBUG) || defined(VHWD_MEMUSEPOOL)

void* operator new(size_t);
void* operator new(size_t,const char*,int);
void* operator new(size_t,int,const char*,int);
void* operator new[](size_t);
void* operator new[](size_t,const char*,int);
void* operator new[](size_t,int,const char*,int);

void operator delete(void*);
void operator delete(void*,const char*,int);
void operator delete(void*,int,const char*,int);
void operator delete[](void*);
void operator delete[](void*,const char*,int);
void operator delete[](void*,int,const char*,int);

#endif


#if defined(VHWD_MEMDEBUG)

#define new DEBUG_NEW
#define DEBUG_NEW  new (__FILE__,__LINE__)

#endif

#define DEFINE_OBJECT_NAME(TYPE,DESC)\
	template<> class ObjectNameT<TYPE>{public:static String MakeName(const String& s){return (s+"#")+DESC;} };

#define DEFINE_OBJECT_NAME_T(TYPE,DESC)\
	template<typename T> class ObjectNameT<TYPE<T> >{public:static String MakeName(const String& s){return ObjectNameT<T>::MakeName((s+"#")+DESC);} };


#define VHWD_ENTER namespace vhwd{
#define VHWD_LEAVE };

#ifndef NDEBUG
#define wassert(x) if(!(x)) vhwd::OnAssertFailure(#x,__FILE__,__LINE__);
#define sassert(x) {char buffer[(x)?1:0];(void)&buffer;}
#else
#define wassert(x)
#define sassert(x)
#endif


#ifndef NULL
#ifdef __cplusplus
#define NULL	0
#else
#define NULL	((void *)0)
#endif
#endif


VHWD_ENTER

// Prevent form copy
class VHWD_DLLIMPEXP NonCopyable
{
	NonCopyable(const NonCopyable&);
	NonCopyable& operator=(const NonCopyable&);
protected:
	NonCopyable() {}
	~NonCopyable() {}
};

template<typename T> class ObjectNameT;

#pragma push_macro("new")
#undef new

// class SmallObject use MemPoolPaging to allocate and deallocate memory
class VHWD_DLLIMPEXP SmallObject
{
public:

	static void* operator new(size_t);
	static void* operator new[](size_t);
	static void* operator new(size_t,const char*,int);
	static void* operator new[](size_t,const char*,int);
	static void operator delete(void*,const char*,int);
	static void operator delete[](void*,const char*,int);
	static void operator delete(void*);
	static void operator delete[](void*);

	static void* operator new(size_t,void*);
	static void operator delete(void*,void*);

};

template<typename T>
class StaticObjectWithoutDeletorT : public NonCopyable
{
	char buf[sizeof(T)];
	static void* operator new(size_t);
public:

	StaticObjectWithoutDeletorT()
	{
		new(this) T();
	}
	~StaticObjectWithoutDeletorT() {}

	operator T&()
	{
		return *reinterpret_cast<T*>(this);
	}
};
#pragma pop_macro("new")


class VHWD_DLLIMPEXP String;
class VHWD_DLLIMPEXP TimeSpan;
class VHWD_DLLIMPEXP TimePoint;


template<typename T>
class hash_t;


template<typename T,int N=0>
class hash_impl
{
public:
	static const int count=N;

	typedef T type1;
	typedef typename tl::meta_if<sizeof(T)==8,uint64_t,uint32_t>::type type2;
	static inline uint32_t hash(const void* p)
	{
		const type1 *pdata  = (const type1*)p;
		type2 seed1 = 0x7FED7FED;
		type2 seed2 = 0xEEEEEEEE;
		type2 seed3 = 0x3123EEEE;
		for(int i=0; i<N; i++)
		{
			type2 temp0 = (type2)*pdata++;
			seed1 = (temp0 * seed3) ^ (seed1 + seed2);
			seed2 = (temp0 + seed1) + seed2 + (seed2 << 5) + 3;
		}
		return (uint32_t)seed1;
	}

	static inline uint32_t hash(const void* p,int n)
	{
		const type1 *pdata  = (const type1*)p;
		type2 seed1 = 0x7FED7FED;
		type2 seed2 = 0xEEEEEEEE;
		type2 seed3 = 0x3123EEEE;
		for(int i=0; i<n; i++)
		{
			type2 temp0 = (type2)*pdata++;
			seed1 = (temp0 * seed3) ^ (seed1 + seed2);
			seed2 = (temp0 + seed1) + seed2 + (seed2 << 5) + 3;
		}
		return (uint32_t)seed1;
	}

};

template<int N>
class hash_raw;

template<> class hash_raw<1> : public hash_impl<uint8_t>{};
template<> class hash_raw<2> : public hash_impl<uint16_t>{};
template<> class hash_raw<4> : public hash_impl<uint32_t>{};
template<> class hash_raw<8> : public hash_impl<uint64_t>{};

template<int N,int D> class hash_base : public hash_impl<uint8_t,N>{};
template<int N> class hash_base<N,0> : public hash_impl<uint64_t,N/8>{};
template<int N> class hash_base<N,2> : public hash_impl<uint16_t,N/2>{};
template<int N> class hash_base<N,4> : public hash_impl<uint32_t,N/4>{};
template<int N> class hash_base<N,6> : public hash_impl<uint16_t,N/2>{};


template<typename T> 
class hash_pod : public hash_base<sizeof(T),sizeof(T)%8>
{
public:
	typedef hash_base<sizeof(T),sizeof(T)%8> basetype;
	uint32_t operator()(const T& val)
	{
		return basetype::hash(&val);
	}
};

template<typename T> 
class hash_origin
{
public:
	uint32_t operator()(const T val)
	{
		return (T)(val);
	}
};

template<> class hash_t<bool> : public hash_origin<bool> {};
template<> class hash_t<int8_t> : public hash_origin<int8_t> {};
template<> class hash_t<uint8_t> : public hash_origin<uint8_t> {};

template<> class hash_t<int16_t> : public hash_pod<int16_t> {};
template<> class hash_t<uint16_t> : public hash_pod<uint16_t> {};
template<> class hash_t<int32_t> : public hash_pod<int32_t> {};
template<> class hash_t<uint32_t> : public hash_pod<uint32_t> {};
template<> class hash_t<float32_t> : public hash_pod<float32_t> {};
template<> class hash_t<wchar_t> : public hash_pod<wchar_t> {};

template<> class hash_t<int64_t> : public hash_pod<int64_t> {};
template<> class hash_t<uint64_t> : public hash_pod<uint64_t> {};
template<> class hash_t<float64_t> : public hash_pod<float64_t> {};
template<> class hash_t<void*> : public hash_pod<void*> {};

template<typename T> class hash_t<const T> : public hash_t<T>{};


template<typename T>
class hash_array
{
public:

	static uint32_t hash(const T* p,size_t n)
	{
		if(tl::is_pod<T>::value)
		{
			return hash_pod<T>::hash(p,n*hash_pod<T>::count);
		}
		else
		{
			hash_t<T> h;uint32_t v=0;
			for(size_t i=0;i<n;i++)
			{
				v^=h(p[i]);
			}
			return v;
		}
	}

	template<typename IT>
	static uint32_t hash(IT v1,IT v2)
	{
		hash_t<T> h;uint32_t v=0;
		for(IT it=v1;it!=v2;++it)
		{
			v^=h(*it);
		}
		return v;
	}

};


void VHWD_DLLIMPEXP OnAssertFailure(const char* what,const char* file,long line);
const String& VHWD_DLLIMPEXP Translate(const String& msg);


enum
{
	LOGLEVEL_DEBUG,
	LOGLEVEL_INFO,
	LOGLEVEL_TRACE,
	LOGLEVEL_PRINT,
	LOGLEVEL_MESSAGE,
	LOGLEVEL_USER,
	LOGLEVEL_WARNING=20,
	LOGLEVEL_ERROR,
	LOGLEVEL_FETAL,
	LOGLEVEL_MAX,
};



VHWD_LEAVE

#endif
