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

#if defined(_WIN64)
	#define VHWD_WIN64
	#define VHWD_X64
	#define VHWD_WINDOWS
#elif defined(_WIN32)
	#define VHWD_WIN32
	#define VHWD_X86
	#define VHWD_WINDOWS
#else
	#ifdef __LP64__
		#define VHWD_X64
	#else
		#define VHWD_X86
	#endif
	#define VHWD_LINUX
#endif




#define MACRO2STR(x) MACRO2STR_ESC(x)
#define MACRO2STR_ESC(x) #x

#ifdef _MSC_VER
#define VWHD_ATTRIBUTE(X) __declspec(X)
#else
#define VWHD_ATTRIBUTE(X) __attribute__((X))
#endif

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


#ifndef _UINTPTR_T_DEFINED
#ifdef  _WIN64
typedef uint64_t uintptr_t;
typedef int64_t intptr_t;
#else
typedef uint32_t uintptr_t;
typedef int32_t intptr_t;
#endif
#define _UINTPTR_T_DEFINED
#endif



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

#pragma pop_macro("new")


class VHWD_DLLIMPEXP String;
class VHWD_DLLIMPEXP TimeSpan;
class VHWD_DLLIMPEXP TimePoint;

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

template<typename T>
class hash_t;

VHWD_LEAVE

#endif
