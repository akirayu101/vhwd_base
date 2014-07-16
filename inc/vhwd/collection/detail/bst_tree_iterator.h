
#ifndef __H_VHWD_COLLECTION_BST_TREE_ITERATOR__
#define __H_VHWD_COLLECTION_BST_TREE_ITERATOR__

#include "vhwd/collection/detail/bst_tree_node.h"

VHWD_ENTER


template<typename P>
class bst_iterator_base : public std::iterator<std::bidirectional_iterator_tag,typename P::value_type>
{
public:

	typedef typename P::nodetype nodetype;
	typedef typename P::value_type value_type;

	bst_iterator_base(nodetype* p,const nodetype** r):_pItem(p),_pRoot(r){}

	void inc()
	{
		if(_pItem<P::_val())
		{
			if(_pItem==P::_beg())
			{
				_pItem=P::nd_min(get_root());
			}
			return;
		}

		if(_pItem->child2)
		{
			_pItem=_pItem->child2;
			while(_pItem->child1) _pItem=_pItem->child1;
			return;
		}

		for(;;)
		{
			nodetype* p=_pItem->parent;
			if(!p)
			{
				_pItem=P::_end();
				return;
			}
			if(p->child1==_pItem)
			{
				_pItem=p;
				return;
			}
			_pItem=p;
		}
	}

	void dec()
	{
		if(_pItem<P::_val())
		{
			if(_pItem==P::_end())
			{
				_pItem=P::nd_max(get_root());
			}
			return;
		}

		if(_pItem->child1)
		{
			_pItem=_pItem->child1;
			while(_pItem->child2) _pItem=_pItem->child2;
			return;
		}

		for(;;)
		{
			nodetype* p=_pItem->parent;
			if(!p)
			{
				_pItem=P::_beg();
				return;
			}
			if(p->child2==_pItem)
			{
				_pItem=p;
				return;
			}
			_pItem=p;
		}
	}

	template<bool D>
	void fwd()
	{
		if(D) inc(); else dec();
	}

	template<bool D>
	void bwd()
	{
		if(D) dec(); else inc();
	}

	value_type* ptr(){return (value_type*)&_pItem->value;}

	bool operator==(const bst_iterator_base& rhs) {return _pItem==rhs._pItem;}
	bool operator!=(const bst_iterator_base& rhs) {return _pItem!=rhs._pItem;}

	nodetype* get_root(){return *(nodetype**)_pRoot;}
	nodetype* get_item(){return _pItem;}

protected:
	nodetype* _pItem;
	const nodetype** _pRoot;
};


template<typename P,bool D,bool C>
class bst_iterator;


template<typename P,bool D>
class bst_iterator<P,D,true> : public bst_iterator_base<P>
{
public:
	typedef bst_iterator_base<P> basetype;
	typedef typename P::nodetype nodetype;
	typedef typename P::value_type value_type;

	bst_iterator():basetype(NULL,NULL){}
	bst_iterator(nodetype* p,const nodetype** r):basetype(p,r){}

	bst_iterator& operator++(){this->template fwd<D>();return *this;}
	bst_iterator operator++(int){bst_iterator tmp(*this);return ++tmp;}
	bst_iterator& operator--(){this->template bwd<D>();return *this;}
	bst_iterator operator--(int){bst_iterator tmp(*this);return --tmp;}

	const value_type& operator*(){return *basetype::ptr();}
	const value_type* operator->(){return basetype::ptr();}

};


template<typename P,bool D>
class bst_iterator<P,D,false> : public bst_iterator<P,D,true>
{
public:
	typedef bst_iterator<P,D,true> basetype;
	typedef typename P::nodetype nodetype;
	typedef typename P::value_type value_type;

	bst_iterator(){}
	bst_iterator(nodetype* p,const nodetype** r):basetype(p,r){}

	bst_iterator& operator++(){this->template fwd<D>();return *this;}
	bst_iterator operator++(int){bst_iterator tmp(*this);return ++tmp;}
	bst_iterator& operator--(){this->template bwd<D>();return *this;}
	bst_iterator operator--(int){bst_iterator tmp(*this);return --tmp;}

	value_type& operator*(){return *basetype::ptr();}
	value_type* operator->(){return basetype::ptr();}

};



VHWD_LEAVE

#endif
