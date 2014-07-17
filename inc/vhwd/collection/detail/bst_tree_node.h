
#ifndef __H_VHWD_COLLECTION_BST_TREE_NODE__
#define __H_VHWD_COLLECTION_BST_TREE_NODE__

#include "vhwd/memory/mempool.h"

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
	typedef bst_node nodetype;
	typedef K key_type;
	typedef V mapped_type;
	typedef std::pair<const key_type,mapped_type> value_type;

	nodetype* parent;
	nodetype* child1;
	nodetype* child2;
	key_type value;
	mapped_type extra;
	color_type color;

	bst_node(){parent=child1=child2=NULL;color=COLOR_RED;}
	bst_node(const key_type& k):value(k){parent=child1=child2=NULL;color=COLOR_RED;}
	bst_node(const value_type& p):value(p.first),extra(p.second){parent=child1=child2=NULL;color=COLOR_RED;}
	bst_node(const nodetype& n):value(n.value),extra(n.extra),color(n.color){parent=child1=child2=NULL;}

};

template<typename K>
class bst_node<K,void> : public bst_base
{
public:
	typedef bst_node nodetype;
	typedef K key_type;
	typedef K mapped_type;
	typedef const K value_type;

	nodetype* parent;
	nodetype* child1;
	nodetype* child2;
	key_type value;
	color_type color;

	bst_node(){parent=child1=child2=NULL;color=COLOR_RED;}
	bst_node(const key_type& k):value(k){parent=child1=child2=NULL;color=COLOR_RED;}
	bst_node(const nodetype& n):value(n.value),color(n.color){parent=child1=child2=NULL;}

};



template<typename K,typename V,typename C>
class bst_trait_base : public bst_base
{
public:

	typedef bst_node<K,V> nodetype;

	typedef typename nodetype::key_type key_type;
	typedef typename nodetype::mapped_type mapped_type;
	typedef typename nodetype::value_type value_type;
	typedef const key_type& key_reference;

	typedef C key_compare;

	static size_t nd_count(nodetype* p)
	{
		if(!p) return 0;
		return 1+nd_count(p->child1)+nd_count(p->child2);
	}


	static bool is_red(nodetype* node)
	{
		return node&&node->color==nodetype::COLOR_RED;
	}

	static bool is_black(nodetype* node)
	{
		return !node||node->color==COLOR_BLACK;
	}

	static void mark_red(nodetype* node)
	{
		node->color=COLOR_RED;
	}

	static void mark_black(nodetype* node)
	{
		if(node) node->color=COLOR_BLACK;
	}

	static void mark(nodetype* node,nodetype* pref)
	{
		node->color=pref->color;
	}

	static void mark_inv(nodetype* node)
	{
		node->color=node->color==COLOR_RED?COLOR_BLACK:COLOR_RED;
	}

	static nodetype* _min(nodetype* n)
	{
		if(!n) return NULL;
		while(n->child1) n=n->child1;
		return n;
	}
	static nodetype* _max(nodetype* n)
	{
		if(!n) return NULL;
		while(n->child2) n=n->child2;
		return n;
	}

	static nodetype* _inc(nodetype* n)
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
			nodetype* p=n->parent;			
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

	static nodetype* _dec(nodetype* n)
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
			nodetype* p=n->parent;			
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

	static const key_type& key(const key_type& v){return v;}
	static const key_type& key(const value_type& v){return v.first;}

};

template<typename K,typename C>
class bst_trait<K,void,C> : public bst_trait_base<K,void,C>
{
public:
	typedef bst_trait_base<K,void,C> basetype;
	typedef typename basetype::key_type key_type;
	typedef typename basetype::value_type value_type;

	static const key_type& key(const key_type& v){return v;}

};


VHWD_LEAVE

#endif
