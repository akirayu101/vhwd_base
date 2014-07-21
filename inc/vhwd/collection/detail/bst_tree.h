
#ifndef __H_VHWD_COLLECTION_BST_TREE__
#define __H_VHWD_COLLECTION_BST_TREE__


#include "vhwd/collection/detail/bst_tree_node.h"
#include "vhwd/collection/detail/bst_tree_iterator.h"

VHWD_ENTER


#pragma push_macro("new")
#undef new

template<typename P,typename A>
class bst_tree : public containerK<typename P::key_compare,typename A::template rebind<typename P::node_type>::other >
{
public:
	typedef containerK<typename P::key_compare,typename A::template rebind<typename P::node_type>::other > basetype;

	typedef typename P::node_type node_type;
	typedef typename P::key_type key_type;
	typedef typename P::mapped_type mapped_type;
	typedef typename P::value_type value_type;
	typedef typename P::const_key_reference const_key_reference;

	typedef typename P::key_compare key_compare;
	typedef typename basetype::size_type size_type;
	typedef typename basetype::allocator_type allocator_type;

private:
	node_type* m_pRoot;
	size_t m_nSize;

public:

	typedef bst_iterator<P,true,false> iterator;
	typedef bst_iterator<P,false,false> reverse_iterator;
	typedef bst_iterator<P,true,true> const_iterator;
	typedef bst_iterator<P,false,true> const_reverse_iterator;

	iterator begin(){return gen_iterator<iterator>(P::nd_min(m_pRoot));}
	iterator end(){return gen_iterator<iterator>(NULL);}
	reverse_iterator rbegin(){return gen_iterator<reverse_iterator>(P::nd_max(m_pRoot));}
	reverse_iterator rend(){return gen_iterator<reverse_iterator>(NULL);}


	bst_tree(){m_pRoot=NULL;m_nSize=0;}
	bst_tree(const key_compare& kc,const allocator_type& al):basetype(kc,al){m_pRoot=NULL;m_nSize=0;}

	bst_tree(const bst_tree& o):basetype(o){m_pRoot=bst_copy_recursive(o.m_pRoot);m_nSize=o.m_nSize;}
	bst_tree& operator=(const bst_tree& o);

#ifdef VHWD_C11
	bst_tree(bst_tree&& o){m_pRoot=NULL;m_nSize=0;swap(o);}
	bst_tree& operator=(bst_tree&& o);
#endif

	~bst_tree(){clear();}

	void swap(bst_tree& o);
	void clear();

	size_type size() const{return m_nSize;}

	template<typename G>
	typename G::ret_type handle_key(const key_type& k)
	{
		G g;return handle_real<G,key_type,false>(g,k);
	}

	template<typename G>
	typename G::ret_type handle_value(const value_type& k)
	{
		G g;return handle_real<G,value_type,false>(g,k);
	}

	template<typename G>
	typename G::ret_type handle_multi_key(const key_type& k)
	{
		G g;return handle_real<G,key_type,true>(g,k);
	}

	template<typename G>
	typename G::ret_type handle_multi_value(const value_type& k)
	{
		G g;return handle_real<G,value_type,true>(g,k);
	}

	template<typename G,typename K,bool M>
	typename G::ret_type handle_real(G& g,const K& v);

	template<typename R>
	class fp_base
	{
	public:
		typedef R ret_type;
		static ret_type handle_empty(bst_tree& t,const_key_reference){return R();}
		static ret_type handle_node1(bst_tree& t,const_key_reference,node_type*){return R();}
		static ret_type handle_node2(bst_tree& t,const_key_reference,node_type*){return R();}
		static ret_type handle_equal(bst_tree& t,const_key_reference,node_type*){return R();}
		static ret_type handle_multi(bst_tree& t,const_key_reference,node_type*){return R();}
	};


	class fp_base_iterator
	{
	public:
		typedef iterator ret_type;
		static ret_type handle_empty(bst_tree& t,const_key_reference){return t.gen_iterator<iterator>(NULL);}
		static ret_type handle_node1(bst_tree& t,const_key_reference,node_type*){return t.gen_iterator<iterator>(NULL);}
		static ret_type handle_node2(bst_tree& t,const_key_reference,node_type*){return t.gen_iterator<iterator>(NULL);}
		static ret_type handle_equal(bst_tree& t,const_key_reference,node_type*){return t.gen_iterator<iterator>(NULL);}
		static ret_type handle_multi(bst_tree& t,const_key_reference,node_type*){return t.gen_iterator<iterator>(NULL);}
	};

	class fp_return_iterator : public fp_base_iterator
	{
	public:
		typedef iterator ret_type;
		static ret_type handle_equal(bst_tree& t,const_key_reference,node_type* n){return t.gen_iterator<ret_type>(n);}
		static ret_type handle_multi(bst_tree& t,const_key_reference,node_type* n){return t.gen_iterator<ret_type>(n);}
	};

	class fp_return_node : public fp_base<node_type*>
	{
	public:
		typedef node_type* ret_type;
		static ret_type handle_equal(bst_tree& t,const_key_reference,node_type* n){return n;}
		static ret_type handle_multi(bst_tree& t,const_key_reference,node_type* n){return n;}
	};

	class fp_lower_bound;
	class fp_upper_bound;
	class fp_equal_range;
	class fp_insert_node;
	class fp_insert_pair;
	class fp_erase;

	static bool check_node(node_type* n);

	void do_erase_swap_node0(node_type* n,node_type* x);
	void do_erase_swap_node1(node_type* n,node_type* x);
	void do_erase_swap_node2(node_type* n,node_type* x);
	void do_erase_swap_node(node_type* n,node_type* x);

	void do_erase(node_type* n);
	iterator erase(const_iterator position);
	iterator erase(const_iterator p1,const_iterator p2);

	template<typename ITERATOR>
	ITERATOR gen_iterator(node_type* p)
	{
		return ITERATOR(p,&m_pRoot);
	}

protected:
	// new_root should be old_root->child1 or old_root->child2;
	void rotate(node_type* old_root,node_type* new_root);
	void rotate_right(node_type* node);
	void rotate_left(node_type* node);

#ifdef VHWD_C11
	template<typename X>
	node_type* bst_construct(X&& o);
#endif

	template<typename X>
	node_type* bst_construct(const X& o);
	node_type* bst_copy_recursive(node_type* r);

	void bst_destroy(node_type* n);
	void bst_destroy_recursive(node_type* n);

	bool bst_validate(){return bst_depth()>=0;}

	int bst_depth();
	int bst_depth_real(node_type* n);

	size_t depth() const{return depth_real(m_pRoot);}
	size_t depth_real(node_type* n)
	{
		if(n==NULL) return 0;
		return 1+std::max(depth_real(n->child1),depth_real(n->child2));
	}

	static node_type* bst_sibling(node_type* n);

	void adjust_insert_case1(node_type* n);
	void adjust_insert_case2(node_type* p,node_type* n);
	void adjust_insert_case3(node_type* p,node_type* n);
	void adjust_insert_case4(node_type* p,node_type* n);
	void adjust_insert_case5(node_type* n);

	void delete_one_child(node_type *n);
    void adjust_delete_case1(node_type *n);
    void adjust_delete_case2(node_type* p,node_type *s);
    void adjust_delete_case3(node_type* p,node_type *s);
    void adjust_delete_case5(node_type* p,node_type *s);
    void adjust_delete_case6(node_type* p,node_type *s);

	static void set_child1(node_type* node,node_type* child)
	{
		node->child1=child;
		if(child) child->parent=node;
	}

	static void set_child2(node_type* node,node_type* child)
	{
		node->child2=child;
		if(child) child->parent=node;
	}
};

template<typename P,typename A>
bst_tree<P,A>& bst_tree<P,A>::operator=(const bst_tree& o)
{
	if(this==&o) return *this;
	bst_tree(o).swap(*this);
	return *this;
}

#ifdef VHWD_C11
template<typename P,typename A>
template<typename X>
typename bst_tree<P,A>::node_type* bst_tree<P,A>::bst_construct(X&& o)
{
	node_type* p=get_allocator().allocate(1);
	try
	{
		new(p) node_type(o);
	}
	catch(...)
	{
		get_allocator().deallocate(p,1);
		throw;
	}
	return p;
}

template<typename P,typename A>
bst_tree<P,A>& bst_tree<P,A>::operator=(bst_tree&& o)
{
	if(this==&o) return *this;
	o.swap(*this);
	return *this;
}
#endif

template<typename P,typename A>
template<typename X>
typename bst_tree<P,A>::node_type* bst_tree<P,A>::bst_construct(const X& o)
{
	node_type* p=this->get_allocator().allocate(1);
	try
	{
		new(p) node_type(o);
	}
	catch(...)
	{
		this->get_allocator().deallocate(p,1);
		throw;
	}
	return p;
}

template<typename P,typename A>
void bst_tree<P,A>::swap(bst_tree& o)
{
	std::swap(m_pRoot,o.m_pRoot);
	std::swap(m_nSize,o.m_nSize);
}

template<typename P,typename A>
void bst_tree<P,A>::clear()
{
	bst_destroy_recursive(m_pRoot);
	m_pRoot=NULL;m_nSize=0;
}

template<typename P,typename A>
void bst_tree<P,A>::bst_destroy(node_type* n)
{
	n->~node_type();
	this->get_allocator().deallocate(n,1);
}

template<typename P,typename A>
void bst_tree<P,A>::bst_destroy_recursive(node_type* n)
{
	if(!n) return;
	bst_destroy_recursive(n->child1);
	bst_destroy_recursive(n->child2);
	bst_destroy(n);
}

template<typename P,typename A>
template<typename G,typename K,bool M>
typename G::ret_type bst_tree<P,A>::handle_real(G& g,const K& v)
{
	if(m_pRoot==NULL)
	{
		return g.handle_empty(*this,v);
	}

	node_type* node=m_pRoot;
	for(;;)
	{
		if(this->key_comp()(P::key(v),P::key(node)))
		{
			if(node->child1==NULL)
			{
				return g.handle_node1(*this,v,node);
			}
			else
			{
				node=node->child1;
			}
		}
		else if(this->key_comp()(P::key(node),P::key(v)))
		{
			if(node->child2==NULL)
			{
				return g.handle_node2(*this,v,node);
			}
			else
			{
				node=node->child2;
			}
		}
		else
		{
			if(M)
			{
				return g.handle_multi(*this,v,node);
			}
			else
			{
				return g.handle_equal(*this,v,node);
			}
		}
	}
}

template<typename P,typename A>
typename bst_tree<P,A>::node_type* bst_tree<P,A>::bst_sibling(node_type* n)
{
	wassert(n!=NULL&&n->parent!=NULL);

	node_type* p=n->parent;
	if(n==p->child1)
	{
		return p->child2;
	}
	else
	{
		return p->child1;
	}
}

template<typename P,typename A>
class bst_tree<P,A>::fp_erase : public fp_base<size_type>
{
public:
	typedef size_type ret_type;
	typedef const key_type& const_key_reference;

	static ret_type handle_equal(bst_tree& t,const_key_reference,node_type* n){t.do_erase(n);return 1;}
	static ret_type handle_multi(bst_tree& t,const_key_reference v,node_type* n)
	{
		ret_type num=1;		
		while(1)
		{
			node_type* x=P::nd_inc(n);
			if(x==NULL||t.key_comp()(v,P::key(x)))
			{
				break;
			}
			++num;t.do_erase(x);
		}

		while(1)
		{
			node_type* x=P::nd_dec(n);
			if(x==NULL||t.key_comp()(P::key(x),v))
			{
				break;
			}
			++num;t.do_erase(x);
		}

		t.do_erase(n);
		return num;
	}

};

template<typename P,typename A>
class bst_tree<P,A>::fp_lower_bound : public fp_base_iterator
{
public:
	typedef iterator ret_type;
	static ret_type handle_node1(bst_tree& t,const_key_reference,node_type* n){return t.gen_iterator<iterator>(n);}
	static ret_type handle_node2(bst_tree& t,const_key_reference,node_type* n){return ++t.gen_iterator<iterator>(n);}
	static ret_type handle_equal(bst_tree& t,const_key_reference,node_type* n){return t.gen_iterator<iterator>(n);}

	static ret_type handle_multi(bst_tree& t,const_key_reference v,node_type* n)
	{
		if(!n->child1) return t.gen_iterator<iterator>(n);
		for(;;)
		{
			if(t.key_comp()(P::key(n),v))
			{
				if(n->child2)
				{
					n=n->child2;
				}
				else
				{
					return ++t.gen_iterator<iterator>(n);
				}
			}
			else
			{
				if(!n->child1)
				{
					return t.gen_iterator<iterator>(n);
				}
				else
				{
					n=n->child1;
				}
			}
		}
	}


};

template<typename P,typename A>
class bst_tree<P,A>::fp_upper_bound : public fp_base_iterator
{
public:
	typedef typename fp_base_iterator::ret_type ret_type;
	static ret_type handle_node1(bst_tree& t,const_key_reference,node_type* n){return t.gen_iterator<iterator>(n);}
	static ret_type handle_node2(bst_tree& t,const_key_reference,node_type* n){return ++t.gen_iterator<iterator>(n);}
	static ret_type handle_equal(bst_tree& t,const_key_reference,node_type* n){return ++t.gen_iterator<iterator>(n);}
	static ret_type handle_multi(bst_tree& t,const_key_reference v,node_type* n)
	{
		if(!n->child2) return ++t.gen_iterator<iterator>(n);
		for(;;)
		{
			if(t.key_comp()(v,P::key(n)))
			{
				if(n->child1)
				{
					n=n->child1;
				}
				else
				{
					return t.gen_iterator<iterator>(n);
				}
			}
			else
			{
				if(!n->child2)
				{
					return ++t.gen_iterator<iterator>(n);
				}
				else
				{
					n=n->child2;
				}
			}
		}
	}
};

template<typename P,typename A>
class bst_tree<P,A>::fp_equal_range
{
public:
	typedef std::pair<iterator,iterator> ret_type;
	static ret_type handle_empty(bst_tree& t,const_key_reference){iterator it=t.gen_iterator<iterator>(NULL);return ret_type(it,it);}
	static ret_type handle_node1(bst_tree& t,const_key_reference,node_type* n){iterator it=t.gen_iterator<iterator>(n);return ret_type(it,it);}
	static ret_type handle_node2(bst_tree& t,const_key_reference,node_type* n){iterator it=++t.gen_iterator<iterator>(n);return ret_type(it,it);}
	static ret_type handle_equal(bst_tree& t,const_key_reference,node_type* n){iterator it=t.gen_iterator<iterator>(n);return ret_type(it,++iterator(it));}
	static ret_type handle_multi(bst_tree& t,const_key_reference v,node_type* n)
	{
		return ret_type(fp_lower_bound::handle_multi(t,v,n),fp_upper_bound::handle_multi(t,v,n));
	}
};



template<typename P,typename A>
class bst_tree<P,A>::fp_insert_pair
{
public:
	typedef std::pair<iterator,bool> ret_type;

	ret_type handle_empty(bst_tree& t,const value_type& v)
	{
		t.m_pRoot=t.bst_construct(v);
		P::mark_black(t.m_pRoot);
		t.m_nSize=1;
		return ret_type(t.gen_iterator<iterator>(t.m_pRoot),true);
	}

	ret_type handle_node1(bst_tree& t,const value_type& v,node_type* p)
	{
		node_type* n=t.bst_construct(v);
		set_child1(p,n);
		return adjust_insert(t,n);
	}

	ret_type handle_node2(bst_tree& t,const value_type& v,node_type* p)
	{
		node_type* n=t.bst_construct(v);
		set_child2(p,n);
		return adjust_insert(t,n);
	}

	ret_type adjust_insert(bst_tree& t,node_type* n)
	{
		t.adjust_insert_case1(n);
		t.m_nSize++;
		return ret_type(t.gen_iterator<iterator>(n),true);
	}

	ret_type handle_equal(bst_tree& t,const value_type&,node_type* n)
	{
		return ret_type(t.gen_iterator<iterator>(n),false);
	}

	ret_type handle_multi(bst_tree& t,const value_type& v,node_type* n)
	{
		for(;;)
		{
			if(t.key_comp()(P::key(v),P::key(n)))
			{
				if(n->child1==NULL)
				{
					return handle_node1(t,v,n);
				}
				else
				{
					n=n->child1;
				}
			}
			else
			{
				if(n->child2==NULL)
				{
					return handle_node2(t,v,n);
				}
				else
				{
					n=n->child2;
				}
			}
		}
	}
};



template<typename P,typename A>
class bst_tree<P,A>::fp_insert_node
{
public:
	typedef node_type* ret_type;

	ret_type handle_empty(bst_tree& t,const_key_reference v)
	{
		t.m_pRoot=t.bst_construct(v);
		P::mark_black(t.m_pRoot);
		t.m_nSize=1;
		return t.m_pRoot;
	}

	ret_type handle_node1(bst_tree& t,const_key_reference v,node_type* p)
	{
		node_type* n=t.bst_construct(v);
		set_child1(p,n);
		return adjust_insert(t,n);
	}

	ret_type handle_node2(bst_tree& t,const_key_reference v,node_type* p)
	{
		node_type* n=t.bst_construct(v);
		set_child2(p,n);
		return adjust_insert(t,n);
	}

	ret_type adjust_insert(bst_tree& t,node_type* n)
	{
		t.adjust_insert_case1(n);
		t.m_nSize++;
		return n;
	}

	ret_type handle_equal(bst_tree&,const_key_reference,node_type* n)
	{
		return n;
	}

	ret_type handle_multi(bst_tree&,const_key_reference,node_type* n)
	{
		return n;
	}

};


template<typename P,typename A>
void bst_tree<P,A>::rotate(node_type* old_root,node_type* new_root)
{
	if(new_root==old_root->child1)
	{
		rotate_right(old_root);
	}
	else
	{
		rotate_left(old_root);
	}
}

template<typename P,typename A>
void bst_tree<P,A>::rotate_right(node_type* node)
{

	wassert(node->child1!=NULL);

	node_type* oldr=node->parent;
	node_type* newr=node->child1;
	set_child1(node,newr->child2);
	set_child2(newr,node);

	if(oldr)
	{
		if(node==oldr->child1)
		{
			set_child1(oldr,newr);
		}
		else
		{
			set_child2(oldr,newr);
		}
	}
	else
	{
		m_pRoot=newr;
		newr->parent=NULL;
	}
}

template<typename P,typename A>
void bst_tree<P,A>::rotate_left(node_type* node)
{
	wassert(node->child2!=NULL);

	node_type* oldr=node->parent;
	node_type* newr=node->child2;
	set_child2(node,newr->child1);
	set_child1(newr,node);

	if(oldr)
	{
		if(node==oldr->child1)
		{
			set_child1(oldr,newr);
		}
		else
		{
			set_child2(oldr,newr);
		}
	}
	else
	{
		m_pRoot=newr;
		newr->parent=NULL;
	}
}

template<typename P,typename A>
void bst_tree<P,A>::do_erase_swap_node(node_type* n,node_type* x)
{
	std::swap(n->color,x->color);
	if(n->child2==x)
	{
		do_erase_swap_node2(n,x);
	}
	else if(n->child1==x)
	{
		do_erase_swap_node1(n,x);
	}
	else
	{
		do_erase_swap_node0(n,x);
	}
}

template<typename P,typename A>
void bst_tree<P,A>::do_erase_swap_node0(node_type* n,node_type* x)
{
	node_type* p=n->parent;
	if(p)
	{
		if(p->child1==n)
		{
			p->child1=x;
		}
		else
		{
			p->child2=x;
		}
	}
	else
	{
		m_pRoot=x;
	}

	p=x->parent;
	if(p->child1==x)
	{
		p->child1=n;
	}
	else
	{
		p->child2=n;
	}

	n->child1->parent=x;
	n->child2->parent=x;
	if(x->child1) x->child1->parent=n;
	if(x->child2) x->child2->parent=n;

	std::swap(n->parent,x->parent);
	std::swap(n->child1,x->child1);
	std::swap(n->child2,x->child2);

	wassert(check_node(n));
	wassert(check_node(x));
}

template<typename P,typename A>
void bst_tree<P,A>::do_erase_swap_node1(node_type* n,node_type* x)
{
	wassert(n->child1==x);
	wassert(n->child2!=NULL);
	wassert(x->child2==NULL);

	// parent
	node_type* p=n->parent;
	x->parent=p;
	if(p)
	{
		if(n==p->child1) p->child1=x;
		else p->child2=x;
	}
	else
	{
		m_pRoot=x;
	}

	// xchild2
	x->child2=n->child2;
	x->child2->parent=x;
	n->child2=NULL;

	// child1
	n->child1=x->child1;
	if(n->child1) n->child1->parent=n;
	x->child1=n;
	n->parent=x;

	wassert(check_node(n));
	wassert(check_node(x));
}

template<typename P,typename A>
void bst_tree<P,A>::do_erase_swap_node2(node_type* n,node_type* x)
{
	wassert(n->child2==x);
	wassert(n->child1!=NULL);
	wassert(x->child1==NULL);

	// parent
	node_type* p=n->parent;
	x->parent=p;
	if(p)
	{
		if(n==p->child1) p->child1=x;
		else p->child2=x;
	}
	else
	{
		m_pRoot=x;
	}

	// xchild2
	x->child1=n->child1;
	x->child1->parent=x;
	n->child1=NULL;

	// child1
	n->child2=x->child2;
	if(n->child2) n->child2->parent=n;
	x->child2=n;
	n->parent=x;

	wassert(check_node(n));
	wassert(check_node(x));

}

template<typename P,typename A>
void bst_tree<P,A>::delete_one_child(node_type *n)
{

    node_type *child=n->child1!=NULL?n->child1:n->child2;
	node_type* p=n->parent;

	if (p == NULL)
	{
		if(child!=NULL)
		{
			m_pRoot=child;
			m_pRoot->parent=NULL;
			P::mark_black(m_pRoot);
		}
		else
		{
			m_pRoot =NULL;
		}
    }
	else
	{
		if (p->child1 == n)
		{
			p->child1 = child;
		}
		else
		{
			p->child2 = child;
		}

		if(child!=NULL)
		{
			child->parent=p;

			if (P::is_black(n))
			{
				if(P::is_red(child))
				{
					P::mark_black(child);
				}
				else
				{
					adjust_delete_case1(child);
				}
			}
		}
		else if(P::is_black(n))
		{
			node_type* s=p->child1;
			if(s==NULL) s=p->child2;
			adjust_delete_case2(p,s);
		}
	}

    bst_destroy(n);
}


template<typename P,typename A>
void bst_tree<P,A>::adjust_delete_case1(node_type *n)
{
	node_type* p=n->parent;
	if(!p)
	{
		wassert(P::is_black(n));
		return;
	}
	adjust_delete_case2(p,p->child1==n?p->child2:p->child1);
}
template<typename P,typename A>
void bst_tree<P,A>::adjust_delete_case2(node_type* p,node_type *s)
{
	if(P::is_red(s))
	{
		P::mark_black(s);
		P::mark_red(p);

		if(s==p->child2)
		{
			rotate_left(p);
			s=p->child2;
		}
		else
		{
			rotate_right(p);
			s=p->child1;
		}
	}

	adjust_delete_case3(p,s);
}
template<typename P,typename A>
void bst_tree<P,A>::adjust_delete_case3(node_type* p,node_type *s) // case3 and case4
{
	wassert(P::is_black(s));

	if(P::is_black(s->child1) && P::is_black(s->child2))
	{
		if(P::is_black(p))
		{
			P::mark_red(s);
			adjust_delete_case1(p);
		}
		else
		{
			P::mark_red(s);
			P::mark_black(p);
		}
	}
	else
	{
		adjust_delete_case5(p,s);
	}

}
template<typename P,typename A>
void bst_tree<P,A>::adjust_delete_case5(node_type* p,node_type *s)
{
	wassert(P::is_black(s));
	if(s==p->child2 && P::is_black(s->child2) && P::is_red(s->child1))
	{
		P::mark_red(s);
		P::mark_black(s->child1);
		rotate_right(s);
		s=p->child2;
	}
	else if(s==p->child1 && P::is_black(s->child1) && P::is_red(s->child2))
	{
		P::mark_red(s);
		P::mark_black(s->child2);
		rotate_left(s);
		s=p->child1;
	}

	adjust_delete_case6(p,s);

}
template<typename P,typename A>
void bst_tree<P,A>::adjust_delete_case6(node_type* p,node_type *s)
{
	P::mark(s,p);
	P::mark_black(p);
	if(s==p->child2)
	{
		P::mark_black(s->child2);
		rotate_left(p);
	}
	else
	{
		P::mark_black(s->child1);
		rotate_right(p);
	}
}


template<typename P,typename A>
void bst_tree<P,A>::adjust_insert_case1(node_type* n)
{
	node_type* p=n->parent;
	if(!p)
	{
		m_pRoot=n;
		P::mark_black(n);
		return;
	}
	adjust_insert_case2(p,n);
}

template<typename P,typename A>
void bst_tree<P,A>::adjust_insert_case2(node_type* p,node_type* n)
{
	if(P::is_black(p))
	{
		return;
	}
	adjust_insert_case3(p,n);
}

template<typename P,typename A>
void bst_tree<P,A>::adjust_insert_case3(node_type* p,node_type* n)
{
	node_type* g=p->parent;
	if( P::is_red(g->child1) && P::is_red(g->child2))
	{
		P::mark_black(g->child1);
		P::mark_black(g->child2);
		P::mark_red(g);
		adjust_insert_case1(g);
	}
	else
	{
		adjust_insert_case4(p,n);
	}
}

template<typename P,typename A>
void bst_tree<P,A>::adjust_insert_case4(node_type* p,node_type* n)
{
	node_type* g=p->parent;
	if(n==p->child2 && p==g->child1)
	{
		rotate_left(p);
		n=n->child1;
	}
	else if(n==p->child1 && p==g->child2)
	{
		rotate_right(p);
		n=n->child2;
	}
	adjust_insert_case5(n);
}

template<typename P,typename A>
void bst_tree<P,A>::adjust_insert_case5(node_type* n)
{
	node_type* p=n->parent;
	node_type* g=p->parent;
	P::mark_black(p);
	P::mark_red(g);

	if(n==p->child1 && p==g->child1)
	{
		rotate_right(g);
	}
	else if(n==p->child2 && p==g->child2)
	{
		rotate_left(g);
	}
	else
	{
		System::LogWarning("bst_tree<...>::adjust_insert_case5 failed!");
	}
}


template<typename P,typename A>
void bst_tree<P,A>::do_erase(node_type* n)
{
	if(n->child1!=NULL&&n->child2!=NULL)
	{
		//node_type* x=P::nd_min(n->child2);
		node_type* x=P::nd_max(n->child1);
		do_erase_swap_node(n,x);
	}
	delete_one_child(n);
	m_nSize--;
}

template<typename P,typename A>
typename bst_tree<P,A>::iterator bst_tree<P,A>::erase(const_iterator position)
{
	wassert(position.root()==m_pRoot);
	node_type* n=position.node();
	if(n==NULL)
	{
		System::LogError("%s failed! INVALID iterator!","bst_tree<...>::erase");
		return end();
	}
	node_type* x=P::nd_inc(n);
	do_erase(n);

	return gen_iterator<iterator>(x);
}

template<typename P,typename A>
typename bst_tree<P,A>::iterator bst_tree<P,A>::erase(const_iterator p1,const_iterator p2)
{
	wassert(p1.root()==m_pRoot);
	wassert(p2.root()==m_pRoot);

	while(p1!=p2)
	{
		node_type* n=p1.node();
		if(n==NULL)
		{
			System::LogError("%s failed! INVALID iterator!","bst_tree<...>::erase");
			return end();
		}

		++p1;
		do_erase(n);
	}
	return gen_iterator<iterator>(p2.node());
}

template<typename P,typename A>
int bst_tree<P,A>::bst_depth()
{
	if(!m_pRoot) return 0;
	try
	{
		return bst_depth_real(m_pRoot);
	}
	catch(...)
	{
		return -1;
	}
}

template<typename P,typename A>
int bst_tree<P,A>::bst_depth_real(node_type* n)
{
	if(n==NULL) return 0;

	int d=P::is_black(n)?1:0;
	if(d==0)
	{
		if(P::is_red(n->child1)||P::is_red(n->child2))
		{
			throw 1;
		}
	}

	int c1=bst_depth_real(n->child1);
	int c2=bst_depth_real(n->child2);
	if(c1==c2)
	{
		return c1+d;
	}
	else
	{
		throw 1;
	}
}

template<typename P,typename A>
bool bst_tree<P,A>::check_node(node_type* n)
{
	if(n->child1 && n->child1->parent!=n)
	{
		return false;
	}
	if(n->child2 && n->child2->parent!=n)
	{
		return false;
	}
	if(n->parent && n->parent->child1!=n && n->parent->child2!=n)
	{
		return false;
	}
	return true;
}


template<typename P,typename A>
typename bst_tree<P,A>::node_type* bst_tree<P,A>::bst_copy_recursive(node_type* r)
{
	if(!r) return NULL;

	node_type* p=bst_construct(*r);
	try
	{
		p->child1=bst_copy_recursive(r->child1);
		p->child2=bst_copy_recursive(r->child2);
	}
	catch(std::bad_alloc&)
	{
		bst_destroy_recursive(p);
		throw;
	}

	return p;
}

#pragma pop_macro("new")


VHWD_LEAVE

#endif
