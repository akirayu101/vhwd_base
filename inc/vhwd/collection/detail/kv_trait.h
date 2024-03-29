
#ifndef __H_VHWD_COLLECTION_KV_TRAIT__
#define __H_VHWD_COLLECTION_KV_TRAIT__

#include "vhwd/config.h"
#include <utility>

VHWD_ENTER


template<typename K,typename V=void>
class kv_trait
{
public:

	typedef K key_type;
	typedef V mapped_type;
	typedef std::pair<const key_type,mapped_type> value_type;
	typedef std::pair<key_type,mapped_type> value_proxy;

	static const key_type& key(const key_type& v)
	{
		return v;
	}
	static const key_type& key(const value_type& v)
	{
		return v.first;
	}

	static const mapped_type& value(const value_type& v)
	{
		return v.second;
	}

	static value_type pair(const key_type& k,const mapped_type& v=mapped_type())
	{
		return value_type(k,v);
	}

	static const value_type& pair(const value_type& k)
	{
		return k;
	}

#ifdef VHWD_C11
	static value_type pair(value_type&& k)
	{
		return std::move(k);
	}
#endif
};


template<typename K>
class kv_trait<K,void>
{
public:

	typedef K key_type;
	typedef K mapped_type;
	typedef const K value_type;
	typedef K value_proxy;

	static const key_type& key(const value_type& v)
	{
		return v;
	}

	static const mapped_type& value(const value_type& v)
	{
		return v;
	}

	static const value_type& pair(const key_type& k)
	{
		return k;
	}

#ifdef VHWD_C11
	static value_type&& pair(value_type&& k)
	{
		return std::move(k);
	}
#endif
};


VHWD_LEAVE

#endif
