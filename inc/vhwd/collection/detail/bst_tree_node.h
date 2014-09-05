
#ifndef __H_VHWD_COLLECTION_BST_TREE_NODE__
#define __H_VHWD_COLLECTION_BST_TREE_NODE__

#include "vhwd/collection/detail/collection_base.h"

VHWD_ENTER



template<typename K,typename V,typename E>
class bst_node
{
public:

	typedef bst_node<K,V,E> node_type;

	typedef K key_type;
	typedef V mapped_type;
	typedef E extra_type;
	typedef typename kv_trait<K,V>::value_type value_type;

	node_type* parent;
	node_type* child1;
	node_type* child2;
	value_type value;
	extra_type extra;

	bst_node()
	{
		parent=child1=child2=NULL;
	}


	bst_node(const value_type& v):value(v)
	{
		parent=child1=child2=NULL;
	}

	bst_node(const bst_node& n):value(n.value),extra(n.extra)
	{
		parent=child1=child2=NULL;
	}

#ifdef VHWD_C11

	bst_node(value_type&& v):value(std::move(v))
	{
		parent=child1=child2=NULL;
	}

	bst_node(bst_node&& n):value(n.value),extra(n.extra)
	{
		parent=child1=child2=NULL;
	}
#endif

};

template<typename K,typename V,typename C,typename E>
class bst_trait_base
{
public:

	typedef bst_node<K,V,E> node_type;

	typedef K key_type;
	typedef V mapped_type;
	typedef typename node_type::value_type value_type;
	typedef const key_type& const_key_reference;

	typedef C key_compare;

	static size_t nd_count(node_type* p)
	{
		if(!p) return 0;
		return 1+nd_count(p->child1)+nd_count(p->child2);
	}

	static node_type* nd_min(node_type* n)
	{
		if(!n) return NULL;
		while(n->child1) n=n->child1;
		return n;
	}
	static node_type* nd_max(node_type* n)
	{
		if(!n) return NULL;
		while(n->child2) n=n->child2;
		return n;
	}

	static node_type* nd_inc(node_type* n)
	{
		wassert(n!=NULL);
		if(n->child2)
		{
			n=n->child2;
			while(n->child1) n=n->child1;
			return n;
		}

		for(;;)
		{
			node_type* p=n->parent;
			if(!p)
			{
				return NULL;
			}
			if(p->child1==n)
			{
				return p;
			}
			n=p;
		}
	}

	static node_type* nd_dec(node_type* n)
	{
		wassert(n!=NULL);
		if(n->child1)
		{
			n=n->child1;
			while(n->child2) n=n->child2;
			return n;
		}

		for(;;)
		{
			node_type* p=n->parent;
			if(!p)
			{
				return NULL;
			}
			if(p->child2==n)
			{
				return p;
			}
			n=p;
		}
	}

};

template<typename K,typename V,typename C,typename E>
class bst_trait : public bst_trait_base<K,V,C,E>
{
public:
	typedef bst_trait_base<K,V,C,E> basetype;
	typedef typename basetype::key_type key_type;
	typedef typename basetype::value_type value_type;
	typedef typename basetype::node_type node_type;

	static const key_type& key(const key_type& v)
	{
		return v;
	}
	static const key_type& key(const value_type& v)
	{
		return v.first;
	}
	static const key_type& key(node_type* node)
	{
		wassert(node!=NULL);
		return key(node->value);
	}

};

template<typename K,typename C,typename E>
class bst_trait<K,void,C,E> : public bst_trait_base<K,void,C,E>
{
public:
	typedef bst_trait_base<K,void,C,E> basetype;
	typedef typename basetype::key_type key_type;
	typedef typename basetype::value_type value_type;
	typedef typename basetype::node_type node_type;

	static const key_type& key(const key_type& v)
	{
		return v;
	}
	static const key_type& key(node_type* node)
	{
		wassert(node!=NULL);
		return node->value;
	}
};


VHWD_LEAVE

#endif
