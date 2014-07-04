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

//#include <cstdarg>
//#include <cstdlib>
//#include <clocale>
//#include <memory>

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
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif


VHWD_ENTER

// Prevent form copy
class VHWD_DLLIMPEXP NonCopyable
{
	NonCopyable(const NonCopyable&);
	NonCopyable& operator=(const NonCopyable&);
protected:
	NonCopyable(){}
	~NonCopyable(){}
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

};

template<typename T>
class StaticObjectWithoutDeletorT : public NonCopyable
{
	char buf[sizeof(T)];
	static void* operator new(size_t);
public:

	StaticObjectWithoutDeletorT(){new(this) T();}
	~StaticObjectWithoutDeletorT(){}

	operator T&(){return *reinterpret_cast<T*>(this);}
};
#pragma pop_macro("new")


class VHWD_DLLIMPEXP String;
class VHWD_DLLIMPEXP TimeSpan;
class VHWD_DLLIMPEXP TimePoint;

template<typename T>
class hash_t;


template<int N>
class hash_pod;

template<>
class hash_pod<1>
{
public:
	static inline uint32_t hash(const void* p,int n)
	{
		const char *key  = (const char *)p;
		int32_t seed1 = 0x7FED7FED;
		int32_t seed2 = 0xEEEEEEEE;
		int32_t seed3 = 0x3123EEEE;
		int32_t ch;

		for(int i=0;i<n;i++)
		{
			ch = *key++;
			seed1 = (ch * seed3) ^ (seed1 + seed2);
			seed2 = (ch + seed1) + seed2 + (seed2 << 5) + 3;
		}
		return (int32_t)seed1;
	}
};

template<>
class hash_pod<4>
{
public:
	static inline uint32_t hash(const void* p,int n)
	{
		const uint32_t *hkey  = (const uint32_t *)p;
		uint32_t seed1 = 0x7FED7FED;
		uint32_t seed2 = 0xEEEEEEEE;
		uint32_t seed3 = 0x3123EEEE;
		uint32_t ch;

		for(int i=0;i<n;i++)
		{
			ch = *hkey++;
			seed1 = (ch * seed3) ^ (seed1 + seed2);
			seed2 = (ch + seed1) + seed2 + (seed2 << 5) + 3;
		}
		return (uint32_t)seed1;
	}
};

template<int N,int D>
class hash_base
{
public:
	static inline uint32_t hash(const void* p)
	{
		return hash_pod<1>::hash(p,N);
	}
};


template<int N>
class hash_base<N,0>
{
public:
	static inline uint32_t hash(const void* p)
	{
		return hash_pod<4>::hash(p,N>>2);
	}
};

template<typename T> class hash_impl : public hash_base<sizeof(T),sizeof(T)%4>
{
public:
	typedef hash_base<sizeof(T),sizeof(T)%4> basetype;
	inline uint32_t operator()(const T& val){return basetype::hash(&val);}
};

template<typename T> class hash_raw
{
public:
	inline uint32_t operator()(const T val)
	{
		return (int32_t)val;
	}
};

template<> class hash_t<int16_t> : public hash_raw<int16_t>{};
template<> class hash_t<uint16_t> : public hash_raw<uint16_t>{};
template<> class hash_t<int32_t> : public hash_raw<int32_t>{};
template<> class hash_t<uint32_t> : public hash_raw<uint32_t>{};
template<> class hash_t<float32_t> : public hash_raw<float32_t>{};
template<> class hash_t<wchar_t> : public hash_raw<wchar_t>{};

template<> class hash_t<int64_t> : public hash_impl<int64_t>{};
template<> class hash_t<uint64_t> : public hash_impl<uint64_t>{};
template<> class hash_t<float64_t> : public hash_impl<float64_t>{};
template<> class hash_t<void*> : public hash_impl<void*>{};


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


template<template<unsigned> class P>
class lookup_table
{
public:
	static unsigned char test(unsigned char text){return cmap[text];}
	unsigned char operator[](unsigned char text) const {return cmap[text];}
	unsigned char operator()(unsigned char text) const {return cmap[text];}
	static const unsigned char cmap[256];
};


template<unsigned N>
class lkt2uppercase
{
public:
	static const int value=(N>='a' && N<='z')?(N+'A'-'a'):N;
};

template<unsigned N>
class lkt2lowercase
{
public:
	static const int value=(N>='A'&&N<='Z')?(N-'A'+'a'):N;
};


template<template<unsigned> class P>
const unsigned char lookup_table<P>::cmap[256]=
{
P<0x00>::value,P<0x01>::value,P<0x02>::value,P<0x03>::value,
P<0x04>::value,P<0x05>::value,P<0x06>::value,P<0x07>::value,
P<0x08>::value,P<0x09>::value,P<0x0A>::value,P<0x0B>::value,
P<0x0C>::value,P<0x0D>::value,P<0x0E>::value,P<0x0F>::value,
P<0x10>::value,P<0x11>::value,P<0x12>::value,P<0x13>::value,
P<0x14>::value,P<0x15>::value,P<0x16>::value,P<0x17>::value,
P<0x18>::value,P<0x19>::value,P<0x1A>::value,P<0x1B>::value,
P<0x1C>::value,P<0x1D>::value,P<0x1E>::value,P<0x1F>::value,
P<0x20>::value,P<0x21>::value,P<0x22>::value,P<0x23>::value,
P<0x24>::value,P<0x25>::value,P<0x26>::value,P<0x27>::value,
P<0x28>::value,P<0x29>::value,P<0x2A>::value,P<0x2B>::value,
P<0x2C>::value,P<0x2D>::value,P<0x2E>::value,P<0x2F>::value,
P<0x30>::value,P<0x31>::value,P<0x32>::value,P<0x33>::value,
P<0x34>::value,P<0x35>::value,P<0x36>::value,P<0x37>::value,
P<0x38>::value,P<0x39>::value,P<0x3A>::value,P<0x3B>::value,
P<0x3C>::value,P<0x3D>::value,P<0x3E>::value,P<0x3F>::value,
P<0x40>::value,P<0x41>::value,P<0x42>::value,P<0x43>::value,
P<0x44>::value,P<0x45>::value,P<0x46>::value,P<0x47>::value,
P<0x48>::value,P<0x49>::value,P<0x4A>::value,P<0x4B>::value,
P<0x4C>::value,P<0x4D>::value,P<0x4E>::value,P<0x4F>::value,
P<0x50>::value,P<0x51>::value,P<0x52>::value,P<0x53>::value,
P<0x54>::value,P<0x55>::value,P<0x56>::value,P<0x57>::value,
P<0x58>::value,P<0x59>::value,P<0x5A>::value,P<0x5B>::value,
P<0x5C>::value,P<0x5D>::value,P<0x5E>::value,P<0x5F>::value,
P<0x60>::value,P<0x61>::value,P<0x62>::value,P<0x63>::value,
P<0x64>::value,P<0x65>::value,P<0x66>::value,P<0x67>::value,
P<0x68>::value,P<0x69>::value,P<0x6A>::value,P<0x6B>::value,
P<0x6C>::value,P<0x6D>::value,P<0x6E>::value,P<0x6F>::value,
P<0x70>::value,P<0x71>::value,P<0x72>::value,P<0x73>::value,
P<0x74>::value,P<0x75>::value,P<0x76>::value,P<0x77>::value,
P<0x78>::value,P<0x79>::value,P<0x7A>::value,P<0x7B>::value,
P<0x7C>::value,P<0x7D>::value,P<0x7E>::value,P<0x7F>::value,
P<0x80>::value,P<0x81>::value,P<0x82>::value,P<0x83>::value,
P<0x84>::value,P<0x85>::value,P<0x86>::value,P<0x87>::value,
P<0x88>::value,P<0x89>::value,P<0x8A>::value,P<0x8B>::value,
P<0x8C>::value,P<0x8D>::value,P<0x8E>::value,P<0x8F>::value,
P<0x90>::value,P<0x91>::value,P<0x92>::value,P<0x93>::value,
P<0x94>::value,P<0x95>::value,P<0x96>::value,P<0x97>::value,
P<0x98>::value,P<0x99>::value,P<0x9A>::value,P<0x9B>::value,
P<0x9C>::value,P<0x9D>::value,P<0x9E>::value,P<0x9F>::value,
P<0xA0>::value,P<0xA1>::value,P<0xA2>::value,P<0xA3>::value,
P<0xA4>::value,P<0xA5>::value,P<0xA6>::value,P<0xA7>::value,
P<0xA8>::value,P<0xA9>::value,P<0xAA>::value,P<0xAB>::value,
P<0xAC>::value,P<0xAD>::value,P<0xAE>::value,P<0xAF>::value,
P<0xB0>::value,P<0xB1>::value,P<0xB2>::value,P<0xB3>::value,
P<0xB4>::value,P<0xB5>::value,P<0xB6>::value,P<0xB7>::value,
P<0xB8>::value,P<0xB9>::value,P<0xBA>::value,P<0xBB>::value,
P<0xBC>::value,P<0xBD>::value,P<0xBE>::value,P<0xBF>::value,
P<0xC0>::value,P<0xC1>::value,P<0xC2>::value,P<0xC3>::value,
P<0xC4>::value,P<0xC5>::value,P<0xC6>::value,P<0xC7>::value,
P<0xC8>::value,P<0xC9>::value,P<0xCA>::value,P<0xCB>::value,
P<0xCC>::value,P<0xCD>::value,P<0xCE>::value,P<0xCF>::value,
P<0xD0>::value,P<0xD1>::value,P<0xD2>::value,P<0xD3>::value,
P<0xD4>::value,P<0xD5>::value,P<0xD6>::value,P<0xD7>::value,
P<0xD8>::value,P<0xD9>::value,P<0xDA>::value,P<0xDB>::value,
P<0xDC>::value,P<0xDD>::value,P<0xDE>::value,P<0xDF>::value,
P<0xE0>::value,P<0xE1>::value,P<0xE2>::value,P<0xE3>::value,
P<0xE4>::value,P<0xE5>::value,P<0xE6>::value,P<0xE7>::value,
P<0xE8>::value,P<0xE9>::value,P<0xEA>::value,P<0xEB>::value,
P<0xEC>::value,P<0xED>::value,P<0xEE>::value,P<0xEF>::value,
P<0xF0>::value,P<0xF1>::value,P<0xF2>::value,P<0xF3>::value,
P<0xF4>::value,P<0xF5>::value,P<0xF6>::value,P<0xF7>::value,
P<0xF8>::value,P<0xF9>::value,P<0xFA>::value,P<0xFB>::value,
P<0xFC>::value,P<0xFD>::value,P<0xFE>::value,P<0xFF>::value
};

VHWD_LEAVE

#endif
