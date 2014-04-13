#ifndef __H_VHWD_INDEXER_MAP__
#define __H_VHWD_INDEXER_MAP__

#include "vhwd/config.h"
#include "vhwd/collection/indexer_detail.h"

VHWD_ENTER

template<typename K,typename V,typename P=indexer_map_trait<K,V> >
class indexer_map
{
public:

	typedef typename P::index_type index_type;
	typedef typename P::pair_type pair_type;
	typedef typename P::key_type key_type;
	typedef typename P::value_type value_type;

	value_type& operator[](const key_type& k)
	{
		return impl.get_val_by_key(k);
	}

	const value_type& operator[](const key_type& k) const
	{
		return impl.get_val_by_key(k);
	}

	index_type insert(const key_type& k,const value_type& v)
	{
		return impl.insert(k,v);
	}

	void set_capacity(size_t n)
	{
		impl.set_capacity(n);
	}

	void clear()
	{
		impl.clear();
	}

	index_type find(const key_type& v) const
	{
		return impl.get_idx_by_key(v);
	}

	const pair_type& get(index_type n) const
	{
		return impl.get_pair_by_idx(n);
	}

	pair_type& get(index_type n)
	{
		return impl.get_pair_by_idx(n);
	}

	size_t size() const {return impl.size();}

private:
	indexer<P> impl;
};


VHWD_LEAVE
#endif
