// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

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
		return P::value(impl.get_pair_by_key(k));
	}

	const value_type& operator[](const key_type& k) const
	{
		return P::value(impl.get_pair_by_key(k));
	}

	index_type insert(const key_type& k,const value_type& v)
	{
		return impl.insert(k,v);
	}

	void rehash(size_t n)
	{
		impl.rehash(n);
	}

	void reserve(size_t n)
	{
		impl.reserve(n);
	}

	float load_factor() const
	{
		return impl.load_factor();
	}


	void clear()
	{
		impl.clear();
	}

	index_type find(const key_type& v) const
	{
		return impl.find(v);
	}

	const pair_type& get(index_type n) const
	{
		return impl.get_pair_by_idx(n);
	}

	pair_type& get(index_type n)
	{
		return impl.get_pair_by_idx(n);
	}

	bool empty() const
	{
		return impl.empty();
	}

	void swap(indexer_map& o)
	{
		impl.swap(o.impl);
	}

	size_t erase(const key_type& v)
	{
		return impl.erase(v);
	}

	size_t size() const {return impl.size();}

private:
	indexer_impl<P> impl;
};


VHWD_LEAVE
#endif
