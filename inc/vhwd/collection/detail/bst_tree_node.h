
#ifndef __H_VHWD_COLLECTION_BST_TREE_NODE__
#define __H_VHWD_COLLECTION_BST_TREE_NODE__

//#include "vhwd/memory/mempool.h"
#include "vhwd/collection/detail/collection_base.h"

VHWD_ENTER


class bst_base
{
public:
	typedef char color_type;
	enum
	{
		COLOR_RED,		// red is default
		COLOR_BLACK,
	};
};

template<typename K,typename V>
class bst_node : public bst_base
{
public:
	typedef bst_node node_type;
	typedef K key_type;
	typedef V mapped_type;
	typedef std::pair<const key_type,mapped_type> value_type;

	node_type* parent;
	node_type* child1;
	node_type* child2;
	key_type value;
	mapped_type extra;
	color_type color;

	bst_node(){parent=child1=child2=NULL;color=COLOR_RED;}
	bst_node(const key_type& k):value(k){parent=child1=child2=NULL;color=COLOR_RED;}
	bst_node(const value_type& p):value(p.first),extra(p.second){parent=child1=child2=NULL;color=COLOR_RED;}
	bst_node(const bst_node& n):value(n.value),extra(n.extra),color(n.color){parent=child1=child2=NULL;}

#ifdef VHWD_C11
	bst_node(key_type&& k):value(k){parent=child1=child2=NULL;color=COLOR_RED;}
	bst_node(value_type&& v):value(v.first),extra(v.second){parent=child1=child2=NULL;color=COLOR_RED;}
#endif

};

template<typename K>
class bst_node<K,void> : public bst_base
{
public:
	typedef bst_node node_type;
	typedef K key_type;
	typedef K mapped_type;
	typedef const K value_type;

	node_type* parent;
	node_type* child1;
	node_type* child2;
	key_type value;
	color_type color;

	bst_node(){parent=child1=child2=NULL;color=COLOR_RED;}
	bst_node(const key_type& k):value(k){parent=child1=child2=NULL;color=COLOR_RED;}
	bst_node(const bst_node& n):value(n.value),color(n.color){parent=child1=child2=NULL;}

#ifdef VHWD_C11
	bst_node(key_type&& k):value(k){parent=child1=child2=NULL;color=COLOR_RED;}
#endif

};



template<typename K,typename V,typename C>
class bst_trait_base : public bst_base
{
public:

	typedef bst_node<K,V> node_type;

	typedef K key_type;
	typedef V mapped_type;
	typedef typename node_type::value_type value_type;
	typedef const key_type& const_key_reference;

	typedef C key_compare;

	static bool is_red(node_type* node)
	{
		return node&&node->color==node_type::COLOR_RED;
	}

	static bool is_black(node_type* node)
	{
		return !node||node->color==COLOR_BLACK;
	}

	static void mark_red(node_type* node)
	{
		node->color=COLOR_RED;
	}

	static void mark_black(node_type* node)
	{
		if(node) node->color=COLOR_BLACK;
	}

	static void mark(node_type* node,node_type* pref)
	{
		node->color=pref->color;
	}

	static void mark_inv(node_type* node)
	{
		node->color=node->color==COLOR_RED?COLOR_BLACK:COLOR_RED;
	}

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

template<typename K,typename V,typename C>
class bst_trait : public bst_trait_base<K,V,C>
{
public:
	typedef bst_trait_base<K,V,C> basetype;
	typedef typename basetype::key_type key_type;
	typedef typename basetype::value_type value_type;
	typedef typename basetype::node_type node_type;

	static const key_type& key(const key_type& v){return v;}
	static const key_type& key(const value_type& v){return v.first;}
	static const key_type& key(node_type* node){wassert(node!=NULL);return node->value;}

};

template<typename K,typename C>
class bst_trait<K,void,C> : public bst_trait_base<K,void,C>
{
public:
	typedef bst_trait_base<K,void,C> basetype;
	typedef typename basetype::key_type key_type;
	typedef typename basetype::value_type value_type;
	typedef typename basetype::node_type node_type;

	static const key_type& key(const key_type& v){return v;}
	static const key_type& key(node_type* node){wassert(node!=NULL);return node->value;}
};


VHWD_LEAVE

#endif
