#ifndef __H_VHWD_COLLECTION_INDEXER_CONTAINER__
#define __H_VHWD_COLLECTION_INDEXER_CONTAINER__

#include "vhwd/collection/detail/indexer_base.h"
#include "vhwd/memory/mempool.h"
#include "vhwd/memory/allocator.h"

VHWD_ENTER


template<typename P,typename A=def_allocator>
class indexer_container : public container_base
{
protected:
	typedef indexer_base<P,A> impl_type;
	mutable impl_type impl;

	typedef typename impl_type::value_proxy value_proxy;

public:
	typedef typename impl_type::key_type key_type;
	typedef typename impl_type::value_type value_type;
	typedef typename impl_type::index_type index_type;
	typedef typename impl_type::size_type size_type;
	typedef typename impl_type::value_array value_array;

	indexer_container() {}
	indexer_container(const indexer_container& o):impl(o.impl) {}

	typedef typename impl_type::proxy_array::iterator iterator;
	typedef typename impl_type::proxy_array::iterator const_iterator;

	iterator begin(){return impl.get_proxys().begin();}
	iterator end(){return impl.get_proxys().end();}
	const_iterator begin() const {return impl.get_proxys().begin();}
	const_iterator end() const {return impl.get_proxys().end();}
	const_iterator cbegin() const {return impl.get_proxys().begin();}
	const_iterator cend() const {return impl.get_proxys().end();}

	void swap(indexer_container& o){impl.swap(o.impl);}

	void swap(value_array& o)
	{
		impl.get_values().swap(o);
		impl.rehash(0);
	}

	inline index_type find(const key_type& v) const
	{
		return impl.find(v);
	}

	inline index_type find2(const key_type& v)
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

	value_type& get(index_type n)
	{
		return (value_type&)impl.get_by_id(n);
	}

	const value_type& get(index_type n) const
	{
		return impl.get_by_id(n);
	}

	bool empty() const
	{
		return impl.empty();
	}

	size_t size() const
	{
		return impl.size();
	}

};




VHWD_LEAVE
#endif

