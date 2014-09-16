#include "vhwd/memory/mempool.h"

#include <cstring>

VHWD_ENTER

extern const char const_empty_buffer[64];

class StringPool : public NonCopyable
{
public:

	typedef char char_type;

	static StringPool& current()
	{
		static StringPool p;
		return p;
	}

	static inline char_type* str_empty()
	{
		return (char_type*)const_empty_buffer;
	}

	static inline char_type* str_alloc(size_t s)
	{
		char_type* p=(char_type*)MemPool::current().allocate(sizeof(char_type)*(s+1));
		p[s]=0;
		return p;
	}

	static inline char_type* str_dup(const char_type* s)
	{
		size_t n=std::char_traits<char_type>::length(s);
		return str_dup(s,n);
	}

	static inline char_type* str_dup(const char_type* s,size_t n)
	{
		if(n==0)
		{
			return str_empty();
		}
		char_type* dst=str_alloc(n);
		memcpy(dst,s,sizeof(char_type)*n);
		return dst;
	}

	static inline void str_free(char_type* p)
	{
		if(p!=const_empty_buffer)
		{
			MemPool::current().deallocate((void*)p);
		}
	}

	static inline char_type* str_cat(const char_type* p1,const char_type* p2)
	{
		size_t n1=std::char_traits<char_type>::length(p1);
		size_t n2=std::char_traits<char_type>::length(p2);
		return str_cat(p1,n1,p2,n2);
	}

	static inline char_type* str_cat(const char_type* p1,size_t n1,const char_type* p2,size_t n2)
	{
		size_t n=n1+n2;
		char_type* dst=str_alloc(n);
		memcpy(dst,p1,sizeof(char_type)*n1);
		memcpy(dst+n1,p2,sizeof(char_type)*n2);
		return dst;
	}

};


VHWD_LEAVE
