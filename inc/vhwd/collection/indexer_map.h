// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_INDEXER_MAP__
#define __H_VHWD_INDEXER_MAP__

#include "vhwd/collection/detail/indexer_container.h"


VHWD_ENTER


template<typename K,typename V,typename A=def_allocator,typename P=indexer_trait<K,V,int> >
class indexer_map : public indexer_container<P,A>
{
protected:
	typedef indexer_container<P,A> basetype;
	typedef typename basetype::impl_type impl_type;
	using basetype::impl;

public:

	typedef typename impl_type::mapped_type mapped_type;
	typedef typename impl_type::key_type key_type;
	typedef typename impl_type::index_type index_type;
	typedef typename impl_type::value_type value_type;
	typedef typename impl_type::size_type size_type;
	typedef typename impl_type::value_proxy value_proxy;

	indexer_map() {}
	indexer_map(const indexer_map& o):basetype(o) {}

#ifdef VHWD_C11
	indexer_map(indexer_map&& o):basetype(o) {}
#endif

	mapped_type& operator[](const key_type& k)
	{
		index_type id=impl.find2(k);
		return this->get(id).second;
	}

	const mapped_type& operator[](const key_type& k) const
	{
		index_type id=impl.find(k);
		if(id==impl_type::invalid_pos) Exception::XNotFound();
		return this->get(id).second;
	}

};

VHWD_LEAVE
#endif
