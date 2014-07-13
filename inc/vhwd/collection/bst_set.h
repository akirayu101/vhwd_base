
#ifndef __H_VHWD_COLLECTION_BST_SET__
#define __H_VHWD_COLLECTION_BST_SET__


#include "vhwd/collection/detail/bst_container.h"


VHWD_ENTER



template<typename T,typename C=std::less<T>,typename A=def_allocator>
class bst_set : public bst_container<bst_trait<T,void,C>,A>
{
public:
	typedef bst_container<bst_trait<T,void,C>,A> basetype;
	typedef typename basetype::key_compare key_compare;
	typedef typename basetype::allocator_type allocator_type;

	using basetype::impl;

	bst_set(){}
	bst_set(const bst_set& o):basetype(o){}
	bst_set(const key_compare& kc,const A& al=A()):basetype(kc,al){}

	bst_set& operator=(const bst_set& o){impl=o.impl;return *this;}

#ifdef VHWD_C11
	bst_set(bst_set&& o):basetype(o){}
	bst_set& operator=(bst_set&& o){this->swap(o);return *this;}
#endif

};



VHWD_LEAVE

#endif
