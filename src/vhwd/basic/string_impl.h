#include "vhwd/memory/mempool.h"
#include "vhwd/collection/indexer_map.h"


VHWD_ENTER

extern char* gpEmptyString;

class StringPool : public NonCopyable
{
public:
	typedef MemPoolPaging MyPool;

	StringPool():pool(MyPool::current()){}

	static StringPool& current()
	{
		static StringPool p;
		return p;
	}

	MyPool& pool;

	template<typename T>
	inline T* str_alloc(size_t s)
	{
		T* p=(T*)pool.allocate(sizeof(T)*(s+1));
		p[s]=0;
		return p;
	}

	template<typename T>
	inline T* str_dup(const T* s)
	{
		size_t n=std::char_traits<T>::length(s);
		T* dst=str_alloc<T>(n);
		memcpy(dst,s,sizeof(T)*n);
		return dst;
	}

	template<typename T>
	inline T* str_dup(const T* s,size_t n)
	{
		T* dst=str_alloc<T>(n);
		memcpy(dst,s,sizeof(T)*n);
		return dst;
	}

	template<typename T>
	inline void str_free(T* p)
	{
		pool.deallocate((void*)p);
	}

	template<typename T>
	inline T* str_cat(const T* p1,const T* p2)
	{
		size_t n1=std::char_traits<T>::length(p1);
		size_t n2=std::char_traits<T>::length(p2);
		T* dst=str_alloc<T>(n1+n2);
		memcpy(dst,p1,sizeof(T)*n1);
		memcpy(dst+n1,p2,sizeof(T)*n2);
		return dst;
	}

	template<typename T>
	inline T* str_cat(const T* p1,size_t n1,const T* p2,size_t n2)
	{
		T* dst=str_alloc<T>(n1+n2);
		memcpy(dst,p1,sizeof(T)*n1);
		memcpy(dst+n1,p2,sizeof(T)*n2);
		return dst;
	}

};


VHWD_LEAVE
