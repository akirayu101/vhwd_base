
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

	bst_iterator_base(nodetype* p,nodetype** r):m_pNode(p),m_ppRoot(r){}

	value_type* item()
	{
		wassert(this->is_valid());
		return (value_type*)&m_pNode->value;
	}

	nodetype* root()
	{
		wassert(m_ppRoot!=NULL);
		return *(nodetype**)m_ppRoot;
	}
	nodetype* node(){return m_pNode;}

	bool is_valid(){return m_pNode!=NULL;}

protected:

	template<bool D>
	void fwd()
	{
		wassert(m_ppRoot!=NULL);
		if(D)
		{
			if(m_pNode)
			{
				m_pNode=P::_inc(m_pNode);
			}
			else
			{
				System::LogWarning("increasing iterator while pNode is NULL");
			}
		}
		else
		{
			if(m_pNode)
			{
				m_pNode=P::_dec(m_pNode);
			}
			else
			{
				System::LogWarning("increasing iterator while pNode is NULL");
			}
		}
	}

	template<bool D>
	void bwd()
	{
		wassert(m_ppRoot!=NULL);
		if(D)
		{
			if(m_pNode)
			{
				m_pNode=P::_dec(m_pNode);
			}
			else
			{
				m_pNode=P::_max(root());
				wassert(m_pNode!=NULL);
			}
		}
		else
		{
			if(m_pNode)
			{
				m_pNode=P::_inc(m_pNode);

			}
			else
			{
				m_pNode=P::_min(root());
				wassert(m_pNode!=NULL);
			}
		}
	}

	nodetype* m_pNode;
	nodetype** m_ppRoot;
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
	bst_iterator(nodetype* p,nodetype** r):basetype(p,r){}

	bst_iterator& operator++(){this->template fwd<D>();return *this;}
	bst_iterator& operator--(){this->template bwd<D>();return *this;}

	bst_iterator operator++(int){bst_iterator tmp(*this);++(*this);return tmp;}
	bst_iterator operator--(int){bst_iterator tmp(*this);--(*this);return tmp;}

	bool operator==(const bst_iterator& it)
	{
		wassert(this->m_ppRoot==it.m_ppRoot);
		return this->m_pNode==it.m_pNode;
	}

	bool operator!=(const bst_iterator& it)
	{
		wassert(this->m_ppRoot==it.m_ppRoot);
		return this->m_pNode!=it.m_pNode;
	}

	const value_type& operator*(){return *this->item();}
	const value_type* operator->(){return this->item();}

};


template<typename P,bool D>
class bst_iterator<P,D,false> : public bst_iterator<P,D,true>
{
public:
	typedef bst_iterator<P,D,true> basetype;
	typedef typename P::nodetype nodetype;
	typedef typename P::value_type value_type;

	bst_iterator(){}
	bst_iterator(nodetype* p,nodetype** r):basetype(p,r){}

	bst_iterator& operator++(){this->template fwd<D>();return *this;}
	bst_iterator& operator--(){this->template bwd<D>();return *this;}

	bst_iterator operator++(int){bst_iterator tmp(*this);++(*this);return tmp;}
	bst_iterator operator--(int){bst_iterator tmp(*this);--(*this);return tmp;}

	value_type& operator*(){return *this->item();}
	value_type* operator->(){return this->item();}

};



VHWD_LEAVE

#endif
