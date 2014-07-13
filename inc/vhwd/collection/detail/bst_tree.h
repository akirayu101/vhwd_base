
#ifndef __H_VHWD_COLLECTION_BST_TREE__
#define __H_VHWD_COLLECTION_BST_TREE__


#include "vhwd/collection/detail/bst_tree_node.h"
#include "vhwd/collection/detail/bst_tree_iterator.h"

VHWD_ENTER


#pragma push_macro("new")
#undef new

template<typename P,typename A>
class bst_tree : public containerK<typename P::key_compare,typename A::template rebind<typename P::nodetype>::other >
{
public:

	typedef containerK<typename P::key_compare,typename A::template rebind<typename P::nodetype>::other > basetype;

	typedef typename P::nodetype nodetype;
	typedef typename P::key_type key_type;
	typedef typename P::mapped_type mapped_type;
	typedef typename P::value_type value_type;
	typedef typename P::key_compare key_compare;
	typedef typename basetype::size_type size_type;
	typedef typename basetype::allocator_type allocator_type;

private:
	nodetype* m_pRoot;
	size_t m_nSize;
public:

	typedef bst_iterator<P,true,false> iterator;
	typedef bst_iterator<P,false,false> reverse_iterator;
	typedef bst_iterator<P,true,true> const_iterator;
	typedef bst_iterator<P,false,true> const_reverse_iterator;

	iterator begin(){return gen_iterator<iterator>(P::_min(m_pRoot));}
	iterator end(){return gen_iterator<iterator>(P::_end());}
	reverse_iterator rbegin(){return gen_iterator<reverse_iterator>(P::_max(m_pRoot));}
	reverse_iterator rend(){return gen_iterator<reverse_iterator>(P::_beg());}

	const_iterator begin() const {return gen_iterator<const_iterator>(P::_min(m_pRoot));}
	const_iterator end()const {return gen_iterator<const_iterator>(P::_end());}
	const_reverse_iterator rbegin() const {return gen_iterator<const_reverse_iterator>(P::_max(m_pRoot));}
	const_reverse_iterator rend() const {return gen_iterator<const_reverse_iterator>(P::_beg());}

	bst_tree()
	{
		m_pRoot=NULL;m_nSize=0;
	}

	bst_tree(const key_compare& kc,const allocator_type& al):basetype(kc,al)
	{
		m_pRoot=NULL;m_nSize=0;
	}

	bst_tree(const bst_tree& o):basetype(o)
	{
		m_pRoot=bst_copy_recursive(o.m_pRoot);
		m_nSize=o.m_nSize;
	}

#ifdef VHWD_C11
	bst_tree(bst_tree&& o)
	{
		m_pRoot=NULL;m_nSize=0;
		swap(o);
	}
#endif

	~bst_tree()
	{
		clear();
	}

	void swap(bst_tree& o)
	{
		std::swap(m_pRoot,o.m_pRoot);
		std::swap(m_nSize,o.m_nSize);
	}

	void clear()
	{
		bst_destroy_recursive(m_pRoot);
		m_pRoot=NULL;
		m_nSize=0;
	}

	bst_tree& operator=(const bst_tree& o)
	{
		if(this==&o) return;
		bst_tree tmp(o);
		tmp.swap(*this);
		return *this;
	}

	size_type size() const
	{
		return m_nSize;
	}

	template<typename G>
	typename G::ret_type handle(typename G::key_reference v)
	{
		G g;return findx<G>(v,g);
	}

	template<typename G>
	typename G::ret_type findx(typename G::key_reference v,G& g)
	{
		if(m_pRoot==NULL)
		{
			return g.handle_root(*this,v);
		}

		nodetype* node=m_pRoot;
		for(;;)
		{
			if(this->key_comp()(v,node->value))
			{
				if(node->child1==NULL)
				{
					return g.handle_add1(*this,v,node);
				}
				else
				{
					node=node->child1;
				}
			}
			else if(this->key_comp()(node->value,v))
			{
				if(node->child2==NULL)
				{
					return g.handle_add2(*this,v,node);
				}
				else
				{
					node=node->child2;
				}
			}
			else
			{
				return g.handle_node(*this,v,node);
			}
		}
	}


	class fp_return_iterator
	{
	public:
		typedef iterator ret_type;
		typedef const key_type& key_reference;

		static ret_type handle_root(const bst_tree& t,key_reference){return t.gen_iterator<iterator>(P::_end());}
		static ret_type handle_add1(const bst_tree& t,key_reference,nodetype*){return t.gen_iterator<iterator>(P::_end());}
		static ret_type handle_add2(const bst_tree& t,key_reference,nodetype*){return t.gen_iterator<iterator>(P::_end());}
		static ret_type handle_node(const bst_tree& t,key_reference,nodetype* n){return t.gen_iterator<ret_type>(n);}
	};


	class fp_lower_bound
	{
	public:
		typedef iterator ret_type;
		typedef const key_type& key_reference;
		static ret_type handle_root(const bst_tree& t,key_reference){return t.gen_iterator<ret_type>(P::_end());}
		static ret_type handle_add1(const bst_tree& t,key_reference,nodetype* n){return t.gen_iterator<ret_type>(n);}
		static ret_type handle_add2(const bst_tree& t,key_reference,nodetype* n){return ++t.gen_iterator<ret_type>(n);}
		static ret_type handle_node(const bst_tree& t,key_reference,nodetype* n){return t.gen_iterator<ret_type>(n);}
	};

	class fp_upper_bound
	{
	public:
		typedef iterator ret_type;
		typedef const key_type& key_reference;
		static ret_type handle_root(const bst_tree& t,key_reference){return t.gen_iterator<ret_type>(P::_end());}
		static ret_type handle_add1(const bst_tree& t,key_reference,nodetype* n){return t.gen_iterator<ret_type>(n);}
		static ret_type handle_add2(const bst_tree& t,key_reference,nodetype* n){return ++t.gen_iterator<ret_type>(n);}
		static ret_type handle_node(const bst_tree& t,key_reference,nodetype* n){return ++t.gen_iterator<ret_type>(n);}
	};

	class fp_equal_range
	{
	public:
		typedef std::pair<iterator,iterator> ret_type;
		typedef const key_type& key_reference;

		static ret_type ge(iterator it){return ret_type(it,it);}
		static ret_type g2(iterator it){return ret_type(it,++iterator(it));}

		static ret_type handle_root(const bst_tree& t,key_reference){return ge(t.gen_iterator<iterator>(P::_end()));}
		static ret_type handle_add1(const bst_tree& t,key_reference,nodetype* n){return ge(t.gen_iterator<iterator>(n));}
		static ret_type handle_add2(const bst_tree& t,key_reference,nodetype* n){return ge(++t.gen_iterator<iterator>(n));}
		static ret_type handle_node(const bst_tree& t,key_reference,nodetype* n){return g2(t.gen_iterator<iterator>(n));}
	};

	class fp_return_nodetype
	{
	public:
		typedef nodetype* ret_type;
		typedef const key_type& key_reference;
		static ret_type handle_root(const bst_tree&,key_reference){return NULL;}
		static ret_type handle_add1(const bst_tree&,key_reference,nodetype*){return NULL;}
		static ret_type handle_add2(const bst_tree&,key_reference,nodetype*){return NULL;}
		static ret_type handle_node(const bst_tree&,key_reference,nodetype* n){return n;}
	};

	class fp_insert_pair
	{
	public:

		typedef std::pair<iterator,bool> ret_type;
		typedef const key_type& key_reference;

		ret_type handle_root(bst_tree& t,key_reference v)
		{
			t.m_pRoot=t.bst_construct(v);
			P::mark_black(t.m_pRoot);
			t.m_nSize=1;
			return ret_type(t.gen_iterator<iterator>(t.m_pRoot),true);
		}

		ret_type handle_add1(bst_tree& t,key_reference v,nodetype* p)
		{
			nodetype* n=t.bst_construct(v);
			set_child1(p,n);
			return adjust_insert(t,n);
		}

		ret_type handle_add2(bst_tree& t,key_reference v,nodetype* p)
		{
			nodetype* n=t.bst_construct(v);
			set_child2(p,n);
			return adjust_insert(t,n);
		}

		ret_type adjust_insert(bst_tree& t,nodetype* n)
		{
			t.adjust_insert_case1(n);
			t.m_nSize++;
			return ret_type(t.gen_iterator<iterator>(n),true);
		}

		ret_type handle_node(bst_tree& t,key_reference,nodetype* n)
		{
			return ret_type(t.gen_iterator<iterator>(n),false);
		}
	};

	class fp_insert_node
	{
	public:

		typedef nodetype* ret_type;
		typedef const key_type& key_reference;

		ret_type handle_root(bst_tree& t,key_reference v)
		{
			t.m_pRoot=t.bst_construct(v);
			P::mark_black(t.m_pRoot);
			t.m_nSize=1;
			return t.m_pRoot;
		}

		ret_type handle_add1(bst_tree& t,key_reference v,nodetype* p)
		{
			nodetype* n=t.bst_construct(v);
			set_child1(p,n);
			return adjust_insert(t,n);
		}

		ret_type handle_add2(bst_tree& t,key_reference v,nodetype* p)
		{
			nodetype* n=t.bst_construct(v);
			set_child2(p,n);
			return adjust_insert(t,n);
		}

		ret_type adjust_insert(bst_tree& t,nodetype* n)
		{
			t.adjust_insert_case1(n);
			t.m_nSize++;
			return n;
		}

		ret_type handle_node(bst_tree&,key_reference,nodetype* n)
		{
			return n;
		}
	};


	void swap_node1(nodetype* n,nodetype* x)
	{
		if(n->parent)
		{
			if(n->parent->child1==n)
			{
				n->parent->child1=x;
			}
			else
			{
				n->parent->child2=x;
			}
		}
		else
		{
			m_pRoot=x;
		}

		if(x->parent->child1==x)
		{
			x->parent->child1=n;
		}
		else
		{
			x->parent->child2=n;
		}

		n->child1->parent=x;
		n->child2->parent=x;
		if(x->child1) x->child1->parent=n;
		if(x->child2) x->child2->parent=n;

		std::swap(n->parent,x->parent);
		std::swap(n->child1,x->child1);
		std::swap(n->child2,x->child2);		
		std::swap(n->color,x->color);

	}

	void swap_node2(nodetype* n,nodetype* x)
	{
		// n->child2==x;
		nodetype* p=n->parent;
		if(p)
		{
			if(n==p->child1)
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

		n->child1->parent=x;
		n->parent=x;
		
		if(x->child2)
		{
			x->child2->parent=n;
		}
		if(x->child1)
		{
			x->child1->parent=n;
		}
		n->child2=x->child2;
		x->child2=n;

		std::swap(n->child1,x->child1);
		std::swap(n->color,x->color);
		x->parent=p;

	}

	void swap_node(nodetype* n,nodetype* x)
	{
		//if(m_pRoot==n)
		//{
		//	m_pRoot=x;
		//}

		if(n->child2!=x)
		{
			swap_node1(n,x);
		}
		else
		{
			swap_node2(n,x);
		}		
	}
		
	

	void do_erase(nodetype* n)
	{
		if(n->child1!=NULL&&n->child2!=NULL)
		{
			nodetype* x=P::nd_min(n->child2);
			swap_node(n,x);
		}
		delete_one_child(n);
		m_nSize--;
	}

	class fp_erase
	{
	public:
		typedef size_type ret_type;
		typedef const key_type& key_reference;
		static ret_type handle_root(bst_tree&,key_reference){return 0;}
		static ret_type handle_add1(bst_tree&,key_reference,nodetype*){return 0;}
		static ret_type handle_add2(bst_tree&,key_reference,nodetype*){return 0;}
		static ret_type handle_node(bst_tree& t,key_reference,nodetype* n){t.do_erase(n);return 1;}
	};

	size_type erase(const key_type& v)
	{
		fp_erase fp;
		return findx<fp_erase>(v,fp);
	}

	iterator erase(const_iterator position)
	{
		wassert(position.get_root()==m_pRoot);
		nodetype* n=position.get_item();
		if(n<P::_val())
		{
			return end();
		}
		++position;
		do_erase(n);

		return gen_iterator<iterator>(position.get_item());
	}

	iterator erase(const_iterator p1,const_iterator p2)
	{
		wassert(p1.get_root()==m_pRoot);
		wassert(p2.get_root()==m_pRoot);
		while(p1!=p2)
		{
			nodetype* n=p1.get_item();
			++p1;
			do_erase(n);
		}
		return gen_iterator<iterator>(p2.get_item());
	}


	template<typename ITERATOR>
	ITERATOR gen_iterator(nodetype* p)
	{
		return ITERATOR(p,(const nodetype**)&m_pRoot);
	}

	template<typename ITERATOR>
	ITERATOR gen_iterator(nodetype* p) const
	{
		return ITERATOR(p,(const nodetype**)&m_pRoot);
	}

protected:

#ifdef VHWD_C11
	template<typename X>
	nodetype* bst_construct(X&& o)
	{
		nodetype* p=get_allocator().allocate(1);
		try
		{
			new(p) nodetype(o);
		}
		catch(...)
		{
			get_allocator().deallocate(p,1);
			throw;
		}
		return p;
	}
#endif

	template<typename X>
	nodetype* bst_construct(const X& o)
	{
		nodetype* p=this->get_allocator().allocate(1);
		try
		{
			new(p) nodetype(o);
		}
		catch(...)
		{
			this->get_allocator().deallocate(p,1);
			throw;
		}
		return p;
	}

	void bst_destroy(nodetype* n)
	{
		n->~nodetype();
		this->get_allocator().deallocate(n,1);
	}

	nodetype* bst_copy_recursive(nodetype* r)
	{
		if(!r) return NULL;

		nodetype* p=bst_construct(*r);
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

	void bst_destroy_recursive(nodetype* n)
	{
		if(!n) return;
		bst_destroy_recursive(n->child1);
		bst_destroy_recursive(n->child2);
		bst_destroy(n);
	}

	bool bst_validate() const
	{
		return bst_depth()>=0;
	}

	int bst_depth() const
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

	int bst_depth_real(nodetype* n) const
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


	size_t depth() const
	{
		return depth_real(m_pRoot);
	}

	size_t depth_real(nodetype* n) const
	{
		if(n==NULL) return 0;
		return 1+std::max(depth_real(n->child1),depth_real(n->child2));
	}

	static nodetype* bst_sibling(nodetype* n)
	{
		nodetype* p=n->parent;
		wassert(p!=NULL);
		if(n==p->child1)
		{
			return p->child2;
		}
		else
		{
			return p->child1;
		}
	}


	void adjust_insert_case1(nodetype* n)
	{
		nodetype* p=n->parent;
		if(!p)
		{
			m_pRoot=n;
			P::mark_black(n);
			return;
		}

		adjust_insert_case2(p,n);

	}

	void adjust_insert_case2(nodetype* p,nodetype* n)
	{
		if(P::is_black(p))
		{
			return;
		}
		adjust_insert_case3(p,n);
	}


	void adjust_insert_case3(nodetype* p,nodetype* n)
	{
		nodetype* g=p->parent;
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

	void adjust_insert_case4(nodetype* p,nodetype* n)
	{
		nodetype* g=p->parent;
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

	void adjust_insert_case5(nodetype* n)
	{
		nodetype* p=n->parent;
		nodetype* g=p->parent;
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


	void delete_one_child(nodetype *n)
	{

        nodetype *child=n->child1!=NULL?n->child1:n->child2;
		nodetype* p=n->parent;

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
				nodetype* s=p->child1;
				if(s==NULL) s=p->child2;
				adjust_delete_case2(p,s);
			}
		}

        bst_destroy(n);
    }

    void adjust_delete_case1(nodetype *n)
	{
		nodetype* p=n->parent;
		if(!p)
		{
			wassert(P::is_black(n));
			return;
		}
		adjust_delete_case2(p,p->child1==n?p->child2:p->child1);
	}

    void adjust_delete_case2(nodetype* p,nodetype *s)
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

    void adjust_delete_case3(nodetype* p,nodetype *s) // case3 and case4
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

    void adjust_delete_case5(nodetype* p,nodetype *s)
	{
		wassert(P::is_black(s)) //trival
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
    void adjust_delete_case6(nodetype* p,nodetype *s)
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

	static void set_child1(nodetype* node,nodetype* child)
	{
		node->child1=child;
		if(child) child->parent=node;
	}
	static void set_child2(nodetype* node,nodetype* child)
	{
		node->child2=child;
		if(child) child->parent=node;
	}

	void rotate(nodetype* old_root,nodetype* new_root)
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

	void rotate_right(nodetype* node)
	{

		wassert(node->child1!=NULL);

		nodetype* oldr=node->parent;
		nodetype* newr=node->child1;
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

	void rotate_left(nodetype* node)
	{
		wassert(node->child2!=NULL);

		nodetype* oldr=node->parent;
		nodetype* newr=node->child2;
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

};
#pragma pop_macro("new")


VHWD_LEAVE

#endif
