
#ifndef __H_VHWD_CONFIG__
#define __H_VHWD_CONFIG__


// enable c++11
//#define VHWD_C11

// use MemPoolDebug to detect memory leaks.
//#define VHWD_MEMDEBUG

// use MemPoolPaging as default memory allocator
//#define VHWD_MEMUSEPOOL




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


#ifdef _WIN32
    typedef int int32_t;
	typedef __int64 int64_t;
	typedef unsigned int uint32_t;
	typedef unsigned __int64 uint64_t;

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

#include "tl.h"

#include <cstdarg>
#include <cstdlib>
#include <memory>
#include <locale>


#if defined(VHWD_MEMDEBUG) || defined(VHWD_MEMUSEPOOL)

void* operator new(size_t);
void* operator new[](size_t);
void* operator new(size_t,const char*,long);
void* operator new[](size_t,const char*,long);
void operator delete(void*,const char*,long);
void operator delete(void*);

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
	const NonCopyable& operator=(const NonCopyable&);
protected:
	NonCopyable(){}
	~NonCopyable(){}
};

template<typename T> class ObjectNameT;

#pragma push_macro("new")
#undef new

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
	static inline int32_t hash(const void* p,int n)
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
	static inline int32_t hash(const void* p,int n)
	{
		const int32_t *key  = (const int32_t *)p;
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

template<int N,int D>
class hash_base
{
public:
	static inline int32_t hash(const void* p)
	{
		return hash_pod<1>::hash(p,N);
	}
};


template<int N>
class hash_base<N,0>
{
public:
	static inline int32_t hash(const void* p)
	{
		return hash_pod<4>::hash(p,N>>2);
	}
};

template<typename T> class hash_impl : public hash_base<sizeof(T),sizeof(T)%4>
{
public:
	typedef hash_base<sizeof(T),sizeof(T)%4> basetype;
	inline int32_t operator()(const T& val){return basetype::hash(&val);}
};

template<> class hash_t<int32_t> : public hash_impl<int32_t>{};
template<> class hash_t<uint32_t> : public hash_impl<uint32_t>{};
template<> class hash_t<int64_t> : public hash_impl<int64_t>{};
template<> class hash_t<uint64_t> : public hash_impl<uint64_t>{};
template<> class hash_t<float32_t> : public hash_impl<float32_t>{};
template<> class hash_t<float64_t> : public hash_impl<float64_t>{};
template<> class hash_t<void*> : public hash_impl<void*>{};

void VHWD_DLLIMPEXP OnAssertFailure(const char* what,const char* file,long line);
const String& VHWD_DLLIMPEXP Translate(const String& msg);

VHWD_LEAVE



#endif
