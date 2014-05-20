#include "vhwd/memory/mempool.h"
#include "vhwd/basic/codecvt.h"
#include "vhwd/collection/indexer_map.h"
#include <cstring>

VHWD_ENTER

void GBKTable_init();

class StringPool : public NonCopyable
{
public:
	typedef MemPoolPaging MyPool;
	typedef char char_type;

	static char_type* gpEmptyString;

	StringPool():pool(MyPool::current())
	{
		GBKTable_init();
	}
	MyPool& pool;

	static StringPool& current()
	{
		static StringPool p;
		return p;
	}

	static inline char_type* str_empty()
	{
		return gpEmptyString;
	}

	inline char_type* str_alloc(size_t s)
	{
		char_type* p=(char_type*)pool.allocate(sizeof(char_type)*(s+1));
		if(p==NULL)
		{
			Exception::XBadAlloc();
		}
		p[s]=0;
		return p;
	}

	inline char_type* str_dup(const char_type* s)
	{
		size_t n=std::char_traits<char_type>::length(s);
		return str_dup(s,n);
	}


	inline char_type* str_dup(const char_type* s,size_t n)
	{
		if(n==0)
		{
			return str_empty();
		}
		char_type* dst=str_alloc(n);
		memcpy(dst,s,sizeof(char_type)*n);
		return dst;
	}

	inline void str_free(char_type* p)
	{
		if(p!=gpEmptyString)
		{
			pool.deallocate((void*)p);
		}

	}

	inline char_type* str_cat(const char_type* p1,const char_type* p2)
	{
		size_t n1=std::char_traits<char_type>::length(p1);
		size_t n2=std::char_traits<char_type>::length(p2);
		return str_cat(p1,n1,p2,n2);
	}

	inline char_type* str_cat(const char_type* p1,size_t n1,const char_type* p2,size_t n2)
	{
		size_t n=n1+n2;
		char_type* dst=str_alloc(n);
		memcpy(dst,p1,sizeof(char_type)*n1);
		memcpy(dst+n1,p2,sizeof(char_type)*n2);
		return dst;
	}

};


VHWD_LEAVE
