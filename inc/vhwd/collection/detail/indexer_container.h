#ifndef __H_VHWD_COLLECTION_INDEXER_CONTAINER__
#define __H_VHWD_COLLECTION_INDEXER_CONTAINER__

#include "vhwd/collection/detail/indexer_base.h"
#include "vhwd/memory.h"

VHWD_ENTER


template<typename P,typename A=def_allocator>
class indexer_container : public container_base
{
public:
	typedef indexer_base<P,A> impl_type;
	impl_type impl;

	typedef typename impl_type::key_type key_type;
	typedef typename impl_type::value_type value_type;
	typedef typename impl_type::index_type index_type;
	typedef typename impl_type::size_type size_type;

	indexer_container(){}
	indexer_container(const indexer_container& o):impl(o.impl){}

#ifdef VHWD_C11
	indexer_container(indexer_container&& o):impl(o.impl){}
#endif

	void swap(indexer_container& o)
	{
		impl.swap(o.impl);
	}

	index_type find(const key_type& v) const
	{
		return impl.find(v);
	}

	index_type find2(const key_type& v)
	{
		return impl.find2(v);
	}

	index_type insert(const value_type& v)
	{
		return impl.insert(v);
	}

	size_type erase(const key_type& v)
	{
		return impl.erase(v);
	}

	void clear()
	{
		impl.clear();
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

	float max_load_factor() const
	{
		return impl.max_load_factor();
	}

	void max_load_factor(float z)
	{
		impl.max_load_factor(z);
	}

	typename impl_type::value_reference get(index_type n)
	{
		return impl.get_by_id(n);
	}

	const value_type& get(index_type n) const
	{
		return impl.get_by_id(n);
	}

	bool empty() const
	{
		return impl.empty();
	}

	size_t size() const {return impl.size();}

};




VHWD_LEAVE
#endif

