// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_INDEXER_DETAIL__
#define __H_VHWD_INDEXER_DETAIL__

#include "vhwd/collection/collection_base.h"
#include "vhwd/collection/array.h"
#include "vhwd/basic/exception.h"
#include "vhwd/memory/mempool.h"
#include "vhwd/memory/allocator_mempool.h"

#include <algorithm>

VHWD_ENTER



template<typename K,typename I=int32_t,typename A=AllocatorUsePool<K,MemPoolPaging> >
class indexer_set_trait
{
public:

	typedef K key_type;
	typedef K value_type;
	typedef K pair_type;
	typedef I index_type;

	typedef arr_1t<index_type,typename A::template rebind<index_type>::other > chunk_type;
	typedef arr_1t<pair_type,typename A::template rebind<pair_type>::other > value_array;
	typedef arr_1t<chunk_type,typename A::template rebind<chunk_type>::other > chunk_array;

	static index_type chunk_find(const key_type& k,const chunk_type& chunk,const value_array& value)
	{
		for(size_t i=0;i<chunk.size();i++)
		{
			index_type id=chunk[i];
			if(compare(k,value[id]))
			{
				return  id;
			}
		}
		return index_type(-1);
	}

	//static size_t chunk_erase(const key_type& k,chunk_type& chunk,value_array& value)
	//{
	//	for(size_t i=0;i<chunk.size();i++)
	//	{
	//		index_type id=chunk[i];
	//		if(compare(k,value[id]))
	//		{
	//			std::swap(value[id],value.back());
	//			value.pop_back();
	//			std::swap(chunk[i],chunk.back());
	//			chunk.pop_back();
	//			return  1;
	//		}
	//	}
	//	return 0;
	//}

	static size_t chunk_erase(chunk_type& chunk,index_type id)
	{
		for(size_t i=0;i<chunk.size();i++)
		{
			if(chunk[i]==id)
			{
				std::swap(chunk[i],chunk.back());
				chunk.pop_back();
				return 1;
			}
		}
		return 0;
	}

	static void chunk_insert(chunk_type& chunk,index_type id)
	{
		chunk.push_back(id);
	}


	static int32_t hashcode_key(const key_type &k)
	{
		hash_t<key_type> h;
		return h(k);
	}

	static bool compare(const key_type& k,const pair_type& v)
	{
		return k==v;
	}

	static key_type& key(pair_type& v){return v;}
	static const key_type& key(const pair_type& v){return v;}

	static value_type& value(pair_type& v){return v;}
	static const value_type& value(const pair_type& v){return v;}

	static const pair_type& pair(const key_type& k){return k;}
	static const pair_type& pair(const key_type& k,const value_type&){return k;}

};


template<typename K,typename V,typename I=int,typename A=AllocatorUsePool<K,MemPoolPaging> >
class indexer_map_trait
{
public:
	typedef K key_type;
	typedef V value_type;
	typedef std::pair<K,V> pair_type;
	typedef I index_type;

	typedef arr_1t<index_type,typename A::template rebind<index_type>::other > chunk_type;
	typedef arr_1t<pair_type,typename A::template rebind<pair_type>::other > value_array;
	typedef arr_1t<chunk_type,typename A::template rebind<chunk_type>::other > chunk_array;

	static index_type chunk_find(const key_type& k,const chunk_type& chunk,const value_array& value)
	{
		for(size_t i=0;i<chunk.size();i++)
		{
			index_type id=chunk[i];
			if(compare(k,value[id]))
			{
				return  id;
			}
		}
		return index_type(-1);
	}

	static void chunk_insert(chunk_type& chunk,index_type id)
	{
		chunk.push_back(id);
	}

	//static size_t chunk_erase(const key_type& k,chunk_type& chunk,value_array& value)
	//{
	//	for(size_t i=0;i<chunk.size();i++)
	//	{
	//		index_type id=chunk[i];
	//		if(compare(k,value[id]))
	//		{
	//			std::swap(value[id],value.back());
	//			value.pop_back();
	//			std::swap(chunk[i],chunk.back());
	//			chunk.pop_back();
	//			return  1;
	//		}
	//	}
	//	return 0;
	//}

	static size_t chunk_erase(chunk_type& chunk,index_type id)
	{
		for(size_t i=0;i<chunk.size();i++)
		{
			if(chunk[i]==id)
			{
				std::swap(chunk[i],chunk.back());
				chunk.pop_back();
				return 1;
			}
		}
		return 0;
	}

	static uint32_t hashcode_key(const key_type &v)
	{
		hash_t<key_type> h;
		return h(v);
	}

	static uint32_t hashcode_key(const pair_type &v)
	{
		return hashcode_key(v.first);
	}

	static bool compare(const key_type& k,const pair_type& v)
	{
		return k==v.first;
	}

	static key_type& key(pair_type& v){return v.first;}
	static const key_type& key(const pair_type& v){return v.first;}

	static value_type& value(pair_type& v){return v.second;}
	static const value_type& value(const pair_type& v){return v.second;}

	static pair_type pair(const key_type& k,const value_type& v=value_type()){return pair_type(k,v);}

};


template<typename P>
class indexer_impl
{
public:

	typedef typename P::key_type key_type;
	typedef typename P::value_type value_type;
	typedef typename P::pair_type pair_type;
	typedef typename P::index_type index_type;

	typedef typename P::value_array value_array;
	typedef typename P::chunk_array chunk_array;
	typedef typename P::chunk_type chunk_type;

	typedef typename value_array::const_iterator const_iterator;


	indexer_impl()
	{
		m_nMaxLoadFactor=2.0;
		rehash(128);
	}

	//static index_type chunk_find(const key_type& k,const chunk_type& chunk,const value_array& value)
	//{
	//	for(size_t i=0;i<chunk.size();i++)
	//	{
	//		index_type id=chunk[i];
	//		if(P::compare(k,value[id]))
	//		{
	//			return  id;
	//		}
	//	}
	//	return index_type(-1);
	//}

	const_iterator begin() const {return values.begin();}
	const_iterator end() const{return values.end();}

	index_type find(const key_type& k) const
	{
		size_t hk=P::hashcode_key(k);
		size_t cp=hk&m_nBucketMask;
		const chunk_type& chunk(buckets[cp]);
		return P::chunk_find(k,chunk,values);
	}

	pair_type& get_pair_by_idx(index_type k)
	{
		return values[k];
	}

	const pair_type& get_pair_by_idx(index_type k) const
	{
		return values[k];
	}

	size_t erase ( const key_type& k )
	{
		size_t hk1=P::hashcode_key(k);
		size_t cp1=hk1&m_nBucketMask;
		chunk_type& chunk1(buckets[cp1]);
		index_type id1=P::chunk_find(k,chunk1,values);
		if(id1==index_type(-1))
		{
			return 0;
		}

		if(id1+1==values.size())
		{
			P::chunk_erase(chunk1,id1);
			values.pop_back();
			return 1;
		}

		size_t hk2=P::hashcode_key(values.back());
		size_t cp2=hk2&m_nBucketMask;
		chunk_type& chunk2(buckets[cp2]);


		std::swap(values[id1],values.back());
		values.pop_back();

		P::chunk_erase(chunk1,id1);
		P::chunk_erase(chunk2,values.size()-1);
		P::chunk_insert(chunk2,id1);

		return 1;
	}

	index_type insert(const key_type& k)
	{
		size_t hk=P::hashcode_key(k);
		size_t cp=hk&m_nBucketMask;
		chunk_type& chunk(buckets[cp]);

		index_type id=P::chunk_find(k,chunk,values);
		if(id!=index_type(-1))
		{
			return id;
		}

		id=(index_type)values.size();
		P::chunk_insert(chunk,id);
		try
		{
			values.push_back(P::pair(k));
			check_loadfactor();
		}
		catch(...)
		{
			P::chunk_erase(chunk,id);
			throw;
		}
		return id;
	}

	index_type insert(const key_type& k,const value_type& v)
	{
		size_t hk=P::hashcode_key(k);
		size_t cp=hk&m_nBucketMask;
		chunk_type& chunk(buckets[cp]);
		index_type id=chunk_find(k,chunk,values);

		if(id!=index_type(-1))
		{
			P::value(values[id])=v;
			return id;
		}

		id=(index_type)values.size();
		P::chunk_insert(chunk,id);
		try
		{
			values.push_back(P::pair(k,v));
			check_loadfactor();
		}
		catch(...)
		{
			P::chunk_erase(chunk,id);
			throw;
		}
		return id;
	}



	pair_type& get_pair_by_key(const key_type& k)
	{
		size_t hk=P::hashcode_key(k);
		size_t cp=hk&m_nBucketMask;
		chunk_type& chunk(buckets[cp]);
		index_type id=P::chunk_find(k,chunk,values);

		if(id!=index_type(-1))
		{
			return values[id];
		}

		id=(index_type)values.size();
		P::chunk_insert(chunk,id);
		try
		{
			values.push_back(P::pair(k));
			check_loadfactor();
		}
		catch(...)
		{
			P::chunk_erase(chunk,id);
			throw;
		}

		return values.back();
	}

	const pair_type& get_pair_by_key(const key_type& k) const
	{
		size_t hk=P::hashcode_key(k);
		size_t cp=hk&m_nBucketMask;
		chunk_type& chunk(buckets[cp]);
		index_type id=chunk_find(k,chunk,values);

		if(id==index_type(-1))
		{
			Exception::XNotFound();
		}

		return values[id];
	}


	void check_loadfactor()
	{
		if(values.size()>m_nElemHint)
		{
			rehash(2.0*double(values.size())/double(m_nMaxLoadFactor));
		}
	}

	void reserve(size_t n_)
	{
		rehash((double(n_)/double(m_nMaxLoadFactor)+0.5));
	}

	void rehash(size_t s_)
	{

		size_t sz=1<<8;
		while(sz<s_) sz<<=1;
		size_t mk=sz-1;

		chunk_array cks;
		cks.resize(sz);
		for(size_t i=0;i<values.size();i++)
		{
			size_t hk=P::hashcode_key(values[i]);
			size_t cp=hk&mk;
			P::chunk_insert(cks[cp],i);
		}

		m_nBucketCount=sz;
		m_nBucketMask=mk;
		cks.swap(buckets);

		m_nElemHint=double(m_nBucketCount)*double(m_nMaxLoadFactor);

	}

	void clear()
	{
		values.clear();
		buckets.clear();
		buckets.resize(m_nBucketCount);
	}

	size_t size() const {return values.size();}

	value_array& get_values(){return values;}
	const value_array& get_values() const {return values;}

	float load_factor() const
	{
		return double(values.size())/double(m_nBucketCount);
	}

	float max_load_factor() const
	{
		return m_nMaxLoadFactor;
	}

	void max_load_factor(float z)
	{
		if(z<0.1f) z=0.1f;
		if(z>20.0f) z=20.0f;
		m_nMaxLoadFactor=z;
		m_nElemHint=double(m_nBucketCount)*double(m_nMaxLoadFactor);
	}

	size_t bucket_size (size_t n) const
	{
		wassert(n>=0&&n<m_nBucketCount);
		return buckets[n].size();
	}

	size_t bucket_count() const
	{
		return m_nBucketCount;
	}

	bool empty() const
	{
		return values.empty();
	}

	void swap(indexer_impl& o)
	{
		values.swap(o.values);
		buckets.swap(o.buckets);
		std::swap(m_nBucketCount,o.m_nBucketCount);
		std::swap(m_nBucketMask,o.m_nBucketMask);
		std::swap(m_nElemHint,o.m_nElemHint);
		std::swap(m_nMaxLoadFactor,o.m_nMaxLoadFactor);

	}

protected:

	value_array values;
	chunk_array buckets;

	size_t m_nBucketCount;
	size_t m_nBucketMask;
	size_t m_nElemHint;
	float m_nMaxLoadFactor;


};

VHWD_LEAVE
#endif
