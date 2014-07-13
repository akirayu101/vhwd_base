
#ifndef __H_VHWD_COLLECTION_BST_CONTAINER__
#define __H_VHWD_COLLECTION_BST_CONTAINER__

#include "vhwd/collection/detail/bst_tree.h"

VHWD_ENTER

template<typename T,typename A>
class bst_container : public containerB<bst_tree<T,A> >
{
protected:
	typedef containerB<bst_tree<T,A> > basetype;
	typedef typename basetype::impl_type impl_type;
	using basetype::impl;

public:

	typedef typename basetype::iterator iterator;
	typedef typename basetype::const_iterator const_iterator;
	typedef typename basetype::size_type size_type;
	typedef typename impl_type::key_type key_type;
	typedef typename impl_type::value_type value_type;
	typedef typename impl_type::key_compare key_compare;
	typedef typename basetype::allocator_type allocator_type;

	bst_container(){}
	bst_container(const bst_container& o):basetype(o){}
	bst_container(const key_compare& kc,const A& al):basetype(kc,allocator_type(al)){}

#ifdef VHWD_C11
	bst_container(bst_container&& o):basetype(o){}
#endif

	iterator find(const key_type& v)
	{
		return impl.handle<impl_type::fp_return_iterator>(v);
	}

	const_iterator find(const key_type& v) const
	{
		return impl.handle<impl_type::fp_return_iterator>(v);
	}

	size_type count(const key_type& v) const
	{
		return impl.handle<impl_type::fp_return_node_c>(v)?1:0;
	}

	std::pair<iterator,bool> insert(const value_type& v)
	{
		return impl.template handle<typename impl_type::fp_insert_pair>(v);
	}

	template <class InputIterator>
	void insert (InputIterator first, InputIterator last)
	{
		while(first!=last) insert(*first++);
	}

	iterator insert(const_iterator, const value_type& v)
	{
		return impl.handle<impl_type::fp_insert_pair>(v).first;
	}

	iterator lower_bound(const key_type& v)
	{
		return impl.handle<impl_type::fp_lower_bound>(v);
	}

	const_iterator lower_bound(const key_type& v) const
	{
		return impl.handle<impl_type::fp_lower_bound>(v);
	}

	iterator upper_bound(const key_type& v)
	{
		return impl.handle<impl_type::fp_upper_bound>(v);
	}

	const_iterator upper_bound(const key_type& v) const
	{
		return impl.handle<impl_type::fp_upper_bound>(v);
	}

	std::pair<const_iterator,const_iterator> equal_range(const key_type& v) const
	{
		return impl.handle<impl_type::fp_equal_range>(v);
	}

	std::pair<iterator,iterator> equal_range (const value_type& v)
	{
		return impl.handle<impl_type::fp_equal_range>(v);
	}

	size_type erase(const key_type& v){return impl.erase(v);}
	iterator erase(const_iterator p){return impl.erase(p);}
	iterator erase(const_iterator p1,const_iterator p2){return impl.erase(p1,p2);}

	typename impl_type::_Keycomp_return key_comp() const
	{
		return impl.key_comp;
	}

	class value_compare
	{
	public:
		value_compare(typename impl_type::_Keycomp_return kc_):kc(kc_){}
		typename impl_type::_Keycomp_return kc;
		bool operator()(const value_type& lhs,const value_type& rhs)
		{
			return kc(T::key(lhs),T::key(rhs));
		}
	};

	value_compare value_comp() const
	{
		return value_compare(impl.key_comp());
	}
};


VHWD_LEAVE

#endif
