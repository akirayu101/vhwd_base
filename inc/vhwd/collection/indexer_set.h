// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_INDEXER_SET__
#define __H_VHWD_INDEXER_SET__

#include "vhwd/config.h"
#include "vhwd/collection/indexer_detail.h"
#include "vhwd/collection/array.h"
VHWD_ENTER

template<typename K,typename P=indexer_set_trait<K> >
class indexer_set
{
public:

	typedef typename P::index_type index_type;
	typedef typename P::key_type key_type;


	index_type insert(const key_type& v)
	{
		return impl.insert(v);
	}

	index_type find(const key_type& v)
	{
		return impl.find(v);
	}

	void clear()
	{
		impl.clear();
	}

	void set_capacity(size_t n)
	{
		impl.set_capacity(n);
	}

	const key_type& get(index_type n)
	{
		return impl.get_pair_by_idx(n);
	}

	size_t size() const {return impl.size();}

private:
	indexer<P> impl;
};


VHWD_LEAVE
#endif
