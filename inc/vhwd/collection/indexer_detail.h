#ifndef __H_VHWD_INDEXER_DETAIL__
#define __H_VHWD_INDEXER_DETAIL__

#include "vhwd/collection/collection_base.h"
#include "vhwd/collection/array.h"
#include "vhwd/basic/exception.h"


VHWD_ENTER


template<typename K,typename I=int32_t,typename A=std::allocator<int> >
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

	static int hashcode_key(const key_type &k)
	{
		hash_t<key_type> h;
		return h(k);
	}

	static bool compare(const key_type& k,const pair_type& v)
	{
		return k==v;
	}

	static value_type& value(pair_type& v){return v;}
	static const value_type& value(const pair_type& v){return v;}

	static const pair_type& pair(const key_type& k){return k;}
	static const pair_type& pair(const key_type& k,const value_type&){return k;}

};


template<typename K,typename V,typename I=int,typename A=std::allocator<int> >
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

	static int hashcode_key(const key_type &v)
	{
		hash_t<key_type> h;
		return h(v);
	}

	static int hashcode_key(const pair_type &v)
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
class indexer
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


	indexer()
	{
		set_capacity(1024);
	}

	const_iterator begin() const {return values.begin();}
	const_iterator end() const{return values.end();}

	index_type get_idx_by_key(const key_type& k) const
	{
		size_t hk=P::hashcode_key(k);
		size_t cp=hk&m_nChunkMask;
		const chunk_type& chunk(chunks[cp]);
		for(size_t i=0;i<chunk.size();i++)
		{
			index_type p=chunk[i];
			if(P::compare(k,values[p]))
			{
				return p;
			}
		}
		return -1;
	}

	index_type insert(const key_type& k)
	{
		size_t hk=P::hashcode_key(k);
		size_t cp=hk&m_nChunkMask;
		chunk_type& chunk(chunks[cp]);
		for(size_t i=0;i<chunk.size();i++)
		{
			index_type p=chunk[i];
			if(P::compare(k,values[p]))
			{
				return p;
			}
		}
		index_type p=(index_type)values.size();
		chunk.push_back(p);
		try
		{
			values.push_back(P::pair(k));
		}
		catch(...)
		{
			chunk.pop_back();
			throw;
		}
		return p;
	}

	index_type insert(const key_type& k,const value_type& v)
	{
		size_t hk=P::hashcode_key(k);
		size_t cp=hk&m_nChunkMask;
		chunk_type& chunk(chunks[cp]);
		for(size_t i=0;i<chunk.size();i++)
		{
			index_type p=chunk[i];
			if(P::compare(k,values[p]))
			{
				return P::value(values[p]);
			}
		}
		index_type p=(index_type)values.size();
		chunk.push_back(p);
		try
		{
			values.push_back(P::pair(k,v));
		}
		catch(...)
		{
			chunk.pop_back();
			throw;
		}
		return p;
	}

	const value_type& get_val_by_idx(index_type k) const
	{
		return P::value(values[k]);
	}

	value_type& get_val_by_idx(index_type k)
	{
		return P::value(values[k]);
	}

	const key_type& get_key_by_idx(index_type k) const
	{
		return P::key(values[k]);
	}

	key_type& get_key_by_idx(index_type k)
	{
		return P::key(values[k]);
	}

	pair_type& get_pair_by_idx(index_type k)
	{
		return values[k];
	}

	const pair_type& get_pair_by_idx(index_type k) const
	{
		return values[k];
	}


	value_type& get_val_by_key(const key_type& k)
	{
		size_t hk=P::hashcode_key(k);
		size_t cp=hk&m_nChunkMask;
		chunk_type& chunk(chunks[cp]);
		for(size_t i=0;i<chunk.size();i++)
		{
			index_type p=chunk[i];
			if(P::compare(k,values[p]))
			{
				return P::value(values[p]);
			}
		}
		index_type sz=(index_type)values.size();
		chunk.push_back(sz);
		try
		{
			values.push_back(P::pair(k));
		}
		catch(...)
		{
			chunk.pop_back();
			throw;
		}
		return P::value(values.back());
	}

	const value_type& get_val_by_key(const key_type& k) const
	{
		size_t hk=P::hashcode_key(k);
		size_t cp=hk&m_nChunkMask;
		const chunk_type& chunk(chunks[cp]);
		for(size_t i=0;i<chunk.size();i++)
		{
			index_type p=chunk[i];
			if(P::compare(k,values[p]))
			{
				return P::value(values[p]);
			}
		}
		Exception::XNotFound();
	}


	void set_capacity(size_t s_)
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
			cks[cp].push_back(i);
		}
		m_nChunkSize=sz;
		m_nChunkMask=mk;
		cks.swap(chunks);
	}

	void clear()
	{
		values.clear();
		chunks.clear();
		chunks.resize(m_nChunkSize);
	}

	size_t size() const {return values.size();}

	value_array& get_values(){return values;}
	const value_array& get_values() const {return values;}

protected:
	value_array values;
	chunk_array chunks;

	size_t m_nChunkSize;
	size_t m_nChunkMask;

};

VHWD_LEAVE
#endif
