
#ifndef __H_VHWD_COLLECTION_INDEXER_BASE__
#define __H_VHWD_COLLECTION_INDEXER_BASE__

#include "vhwd/collection/detail/kv_trait.h"
#include "vhwd/collection/arr_1t.h"

#include "math.h"

VHWD_ENTER



template<typename K,typename V,typename I>
class indexer_trait : public kv_trait<K,V>
{
public:
	typedef I index_type;
	typedef kv_trait<K,V> basetype;
	typedef typename basetype::key_type key_type;


	static const index_type invalid_pos=index_type(-1);

	class chunk_node : public SmallObject
	{
	public:
		chunk_node* pnext;
		index_type index;
	};

	static int32_t hashcode_key(const key_type &k)
	{
		hash_t<key_type> h;
		return h(k);
	}

	static bool equal(const key_type& k1,const key_type& k2)
	{
		return k1==k2;
	}
};


template<typename P,typename A>
class indexer_base : public containerA<A>
{
public:

	typedef typename P::key_type key_type;
	typedef typename P::value_type value_type;
	typedef typename P::mapped_type mapped_type;
	typedef typename P::index_type index_type;
	typedef typename P::value_proxy value_proxy;
	typedef typename P::chunk_node chunk_node;

	static const size_t g_nInitialBucketCount=32;

	indexer_base()
	{
		m_nMaxLoadFactor=2.0;
		m_nBucketMask=0;
		m_nBucketCount=0;
	}

	indexer_base(const indexer_base& o):values(o.values)
	{
		m_nMaxLoadFactor=o.m_nMaxLoadFactor;
		m_nBucketMask=0;
		m_nBucketCount=0;
		if(!values.empty())
		{
			rehash(0);
		}
	}

	indexer_base& operator=(const indexer_base& o)
	{
		if(this==&o) return *this;
		indexer_base(o).swap(*this);
		return *this;
	}

#ifdef VHWD_C11

	indexer_base(indexer_base&& o)
	{
		swap(o);
	}

	indexer_base& operator=(indexer_base&& o)
	{
		if(this!=&o) swap(o);
		return *this;
	}

#endif

	class chunk_type
	{
	public:
		chunk_node* phead;
		chunk_type()
		{
			phead=NULL;
		}
		chunk_type(const chunk_type& o)
		{
			phead=copy_recursive(o.phead);
		}

		chunk_type& operator=(const chunk_type& o)
		{
			phead=copy_recursive(o.phead);
			return *this;
		}

#ifdef VHWD_C11
		chunk_type(chunk_type&& o)
		{
			if(this==&o) return;
			phead=o.phead;
			o.phead=NULL;
		}

		chunk_type& operator=(chunk_type&& o)
		{
			if(this==&o) return;
			phead=o.phead;
			o.phead=NULL;
			return *this;
		}
#endif

		static chunk_node* copy_recursive(chunk_node* p)
		{
			chunk_node* h=NULL;
			try
			{
				while(p)
				{
					chunk_node* t=create();
					t->pnext=h;
					t->index=p->index;
					h=t;
					p=p->pnext;
				}
			}
			catch(...)
			{
				destroy_recursive(h);
				throw;
			}
			return h;
		}

		static void destroy_recursive(chunk_node* p)
		{
			while(p)
			{
				chunk_node* t=p;
				p=p->pnext;
				destroy(t);
			}
		}

		static void destroy(chunk_node* p)
		{
			MemPoolPaging::current().deallocate(p);
		}

		static chunk_node* create()
		{
			return (chunk_node*)MemPoolPaging::current().allocate(sizeof(chunk_node));
		}

		~chunk_type()
		{
			destroy_recursive(phead);
		}
	};

	typedef typename A::template rebind<chunk_node>::other chunk_allocator;

	typedef arr_1t<value_type,typename A::template rebind<value_type>::other > value_array;
	typedef arr_1t<value_proxy,typename A::template rebind<value_type>::other > proxy_array;
	typedef arr_1t<chunk_type,typename A::template rebind<chunk_type>::other > chunk_array;

	typedef typename value_array::iterator iterator;
	typedef typename value_array::const_iterator const_iterator;
	typedef typename value_array::reverse_iterator reverse_iterator;
	typedef typename value_array::const_reverse_iterator const_reverse_iterator;


	const_iterator begin() const
	{
		return values.begin();
	}

	const_iterator end() const
	{
		return values.end();
	}

	iterator begin()
	{
		return values.begin();
	}

	iterator end()
	{
		return values.end();
	}

	const_reverse_iterator rbegin() const
	{
		return values.rbegin();
	}

	const_reverse_iterator rend() const
	{
		return values.rend();
	}

	reverse_iterator rbegin()
	{
		return values.rbegin();
	}

	reverse_iterator rend()
	{
		return values.rend();
	}

	static const index_type invalid_pos=index_type(-1);

	inline index_type chunk_find(const chunk_type& chunk,const key_type& k) const
	{
		for(chunk_node* p=chunk.phead; p!=NULL; p=p->pnext)
		{
			if(P::equal(P::key(values[p->index]),k)) return p->index;
		}
		return invalid_pos;
	}

	static inline void chunk_insert(chunk_type& chunk,index_type k)
	{
		chunk_node* p=chunk_type::create();
		p->index=k;
		p->pnext=chunk.phead;
		chunk.phead=p;
	}

	static inline void chunk_replace(chunk_type& chunk,index_type id1,index_type id2)
	{
		for(chunk_node* n=chunk.phead; n; n=n->pnext)
		{
			if(n->index==id1)
			{
				n->index=id2;
				return;
			}
		}
	}

	index_type chunk_find_delete(chunk_type& chunk,const key_type& k)
	{
		if(!chunk.phead) return invalid_pos;
		chunk_node* p=chunk.phead;
		index_type id=p->index;
		if(P::equal(P::key(values[id]),k))
		{
			index_type id=p->index;
			chunk.phead=chunk.phead->pnext;
			chunk_type::destroy(p);
			return id;
		}

		for(;;)
		{
			chunk_node* n=p->pnext;
			if(!n) return invalid_pos;
			id=n->index;
			if(P::equal(P::key(values[id]),k))
			{
				p->pnext=n->pnext;
				chunk_type::destroy(n);
				return id;
			}
			p=n;
		}

	}

	size_t chunk_erase(chunk_type& chunk,index_type id)
	{
		if(!chunk.phead) return 0;
		chunk_node* p=chunk.phead;
		if(p->index==id)
		{
			chunk.phead=chunk.phead->pnext;
			chunk_type::destroy(p);
			return 1;
		}

		for(;;)
		{
			chunk_node* n=p->pnext;
			if(!n) return 0;
			if(n->index==id)
			{
				p->pnext=n->pnext;
				chunk_type::destroy(n);
				return 1;
			}
			p=n;
		}
	}

	inline index_type find(const key_type& k) const
	{
		if(m_nBucketMask==0)
		{
			return invalid_pos;
		}

		size_t hk=P::hashcode_key(k);
		size_t cp=hk&m_nBucketMask;
		const chunk_type& chunk(buckets[cp]);
		return chunk_find(chunk,k);
	}


	inline index_type insert(const value_type& v)
	{
		const key_type& k(P::key(v));
		size_t hk=P::hashcode_key(k);

		if(m_nBucketMask==0)
		{
			rehash(g_nInitialBucketCount);
		}

		size_t cp=hk&m_nBucketMask;
		chunk_type& chunk(buckets[cp]);
		index_type id=chunk_find(chunk,k);

		if(id==invalid_pos)
		{
			id=(index_type)values.size();
			chunk_push(chunk,id,v);
		}

		return id;
	}


	void chunk_push(chunk_type& chunk,index_type id,const value_type& v)
	{
		chunk_node* p=chunk_type::create();

		try
		{
			values.push_back(v);
			check_loadfactor();
		}
		catch(...)
		{
			chunk_type::destroy(p);
			throw;
		}

		p->index=id;
		p->pnext=chunk.phead;
		chunk.phead=p;

	}

	inline index_type find2(const key_type& k)
	{

		if(m_nBucketMask==0)
		{
			rehash(g_nInitialBucketCount);
		}

		size_t hk=P::hashcode_key(k);
		size_t cp=hk&m_nBucketMask;
		chunk_type& chunk(buckets[cp]);
		index_type id=chunk_find(chunk,k);

		if(id==invalid_pos)
		{
			id=(index_type)values.size();
			chunk_push(chunk,id,P::pair(k));
		}
		return id;
	}

	inline size_t erase(const key_type& k)
	{
		if(m_nBucketMask==0)
		{
			return 0;
		}

		size_t hk1=P::hashcode_key(k);
		size_t cp1=hk1&m_nBucketMask;
		chunk_type& chunk1(buckets[cp1]);
		index_type id1=chunk_find_delete(chunk1,k);

		if(id1==P::invalid_pos)
		{
			return 0;
		}

		index_type id2=(index_type)values.size()-1;
		if(id1!=id2)
		{
			std::swap(values[id1],values[id2]);
			size_t hk2=P::hashcode_key(P::key(values[id1]));
			size_t cp2=hk2&m_nBucketMask;
			chunk_type& chunk2(buckets[cp2]);
			chunk_replace(chunk2,id2,id1);
		}

		values.pop_back();
		return 1;
	}

	inline value_type& get_by_id(index_type k)
	{
		return (value_type&)values[k];
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
		values.reserve(n_);
		size_t bc=::ceil(double(n_)/double(m_nMaxLoadFactor));
		if(bc>m_nBucketCount)
		{
			rehash(bc);
		}
	}

	size_t min_bucket_count() const
	{
		return (double(values.size())/double(m_nMaxLoadFactor)+0.5);
	}

	void rehash(size_t s_)
	{
		if(s_==0&&values.empty())
		{
			buckets.clear();
			m_nBucketCount=0;
			m_nBucketMask=0;
			return;
		}

		size_t bc=min_bucket_count();
		size_t sz=sz_helper::n2p(std::max(s_,bc));
		size_t mk=sz-1;

		chunk_array cks;
		cks.resize(sz);
		size_t n=values.size();
		for(size_t i=0; i<n; i++)
		{
			size_t hk=P::hashcode_key(P::key(values[i]));
			size_t cp=hk&mk;
			chunk_insert(cks[cp],i);
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

	size_t size() const
	{
		return values.size();
	}

	value_array& get_values()
	{
		return (value_array&)values;
	}

	proxy_array& get_proxys()
	{
		return (proxy_array&)values;
	}

	const value_array& get_values() const
	{
		return values;
	}

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

	void swap(indexer_base& o)
	{
		values.swap(o.values);
		buckets.swap(o.buckets);
		std::swap(m_nBucketCount,o.m_nBucketCount);
		std::swap(m_nBucketMask,o.m_nBucketMask);
		std::swap(m_nElemHint,o.m_nElemHint);
		std::swap(m_nMaxLoadFactor,o.m_nMaxLoadFactor);
	}

protected:

	proxy_array values;
	chunk_array buckets;

	size_t m_nBucketCount;
	size_t m_nBucketMask;
	size_t m_nElemHint;
	float m_nMaxLoadFactor;


};

VHWD_LEAVE

#endif

