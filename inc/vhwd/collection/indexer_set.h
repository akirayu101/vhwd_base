// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_INDEXER_SET__
#define __H_VHWD_INDEXER_SET__


#include "vhwd/collection/detail/indexer_container.h"

VHWD_ENTER


template<typename K,typename A=def_allocator,typename P=indexer_trait<K,void,int> >
class indexer_set : public indexer_container<P,A>
{
protected:
	typedef indexer_container<P,A> basetype;
	typedef typename basetype::impl_type impl_type;
	using basetype::impl;

public:

	typedef typename impl_type::key_type key_type;
	typedef typename impl_type::index_type index_type;
	typedef typename impl_type::value_type value_type;
	typedef typename impl_type::size_type size_type;
	typedef typename impl_type::const_iterator iterator;


	indexer_set() {}
	indexer_set(const indexer_set& o):basetype(o) {}

#ifdef VHWD_C11
	indexer_set(indexer_set&& o):basetype(o) {}
#endif

};

VHWD_LEAVE
#endif
