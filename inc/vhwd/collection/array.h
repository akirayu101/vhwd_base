// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_COLLECTION_ARRAY__
#define __H_VHWD_COLLECTION_ARRAY__


#include "vhwd/collection/detail/collection_base.h"

VHWD_ENTER

template<typename T,typename A>
class arr_1t : public arr_base<T,A,extra_vec_1t>
{
public:
	typedef arr_base<T,A,extra_vec_1t> basetype;

    typedef typename basetype::size_type size_type;
	typedef typename basetype::value_type value_type;
	typedef typename basetype::pointer pointer;
	typedef typename basetype::const_pointer const_pointer;
	typedef typename basetype::reference reference;
	typedef typename basetype::const_reference const_reference;
	typedef typename basetype::iterator iterator;
	typedef typename basetype::const_iterator const_iterator;
	typedef typename basetype::reverse_iterator reverse_iterator;
	typedef typename basetype::const_reverse_iterator const_reverse_iterator;

	using basetype::swap;


	arr_1t(){}
	arr_1t(const A& al);
	arr_1t(const arr_1t& o);
	arr_1t& operator=(const arr_1t& o);

	~arr_1t();

#if defined(VHWD_C11)
	arr_1t(arr_1t&& p){swap(p);}
	arr_1t& operator=(arr_1t&& p){swap(p);return *this;}
#endif

	void clear();

	size_type size() const;
	size_type capacity() const;
	bool empty() const;

	void reserve(size_type capacity_);
	void resize(size_type newsize_,const T& value_=T());

	void assign(size_type count_,const T& val_);

	template<typename It>
	void assign(It first_,size_type count_);

	template<typename It>
	void assign(It first_,It last_){assign(first_,std::distance(first_,last_));}


	iterator insert(const_iterator where_,const T& val_){return insert(where_,&val_,1);}

	template<typename It>
	iterator insert(const_iterator where_,It first_,size_type count_);

	template<typename It>
	iterator insert(const_iterator where_,It first_,It last_){insert(where_,first_,std::distance(first_,last_));}

	void append(const T& val_){append(&val_,1);}

	template<typename It>
	void append(It first_,size_type count_);

	template<typename It>
	void append(It first_,It last_){append(first_,std::distance(first_,last_));}

	iterator erase(iterator position_);
	iterator erase(iterator first_,iterator last_);

	void shrink_to_fit();

	reference front();
	reference back();

	const_reference front() const;
	const_reference back() const;

	void push_back(const T& val_){append(&val_,1);}
	void pop_back();

	iterator begin(){return m_ptr;}
	iterator end(){return m_ptr+extra().size;}

	const_iterator begin() const {return m_ptr;}
	const_iterator end() const {return m_ptr+extra().size;}

	reverse_iterator rbegin(){return m_ptr+extra().size-1;}
	reverse_iterator rend(){return m_ptr-1;}

	const_reverse_iterator rbegin() const {return m_ptr+extra().size-1;}
	const_reverse_iterator rend() const {return m_ptr-1;}

	reference at (size_type n){if(n>=size()) Exception::XInvalidIndex();return m_ptr[n];}
	const_reference at (size_type n) const{if(n>=size()) Exception::XInvalidIndex();return m_ptr[n];}

	pointer data() {return m_ptr;}
	const_pointer data() const {return m_ptr;}

protected:

	using basetype::m_ptr;
	using basetype::extra;

};

template<typename T,typename A1,typename A2>
bool operator==(const arr_1t<T,A1>& lhs,const arr_1t<T,A2>& rhs)
{
	if(lhs.size()!=rhs.size()) return false;
	for(size_t i=0;i<lhs.size();i++)
	{
		if(lhs[i]!=rhs[i]) return false;
	}
	return true;
}

template<typename T,typename A1,typename A2>
bool operator!=(const arr_1t<T,A1>& lhs,const arr_1t<T,A2>& rhs)
{
	return !(lhs==rhs);
}


template<typename T,typename A>
arr_1t<T,A>::arr_1t(const arr_1t& o):basetype(o.get_allocator())
{
	size_type _newsize=o.size();
	this->_xset(_newsize);

	if(_newsize==0) return;
	try
	{
		xmem<T>::uninitialized_copy_n(o.m_ptr,_newsize,m_ptr);
		extra().size=_newsize;
	}
	catch(...)
	{
		this->_xset(0);
		throw;
	}
}

template<typename T,typename A>
arr_1t<T,A>& arr_1t<T,A>::operator=(const arr_1t& o)
{
	if(this==&o) return *this;

	size_type _newsize=o.size();
	size_type _capacity=extra().capacity;
	if(_capacity<_newsize||_capacity>(_newsize*2))
	{
		arr_1t(o).swap(*this);
	}
	else
	{
		size_type _oldsize=extra().size;
		if(_newsize>_oldsize)
		{
			xmem<T>::copy_n(o.m_ptr,_oldsize,m_ptr);
			xmem<T>::uninitialized_copy_n(o.m_ptr+_oldsize,_newsize-_oldsize,m_ptr+_oldsize);
			extra().size=_newsize;
		}
		else
		{
			xmem<T>::copy_n(o.m_ptr,_newsize,m_ptr);
			xmem<T>::destroy(o.m_ptr+_newsize,_oldsize-_newsize);
			extra().size=_newsize;
		}
	}

	return *this;
}

template<typename T,typename A>
arr_1t<T,A>::arr_1t(const A& al):basetype(al)
{

}

template<typename T,typename A>
arr_1t<T,A>::~arr_1t()
{
	clear();
}

template<typename T,typename A>
void arr_1t<T,A>::shrink_to_fit()
{
	if(size()==capacity()) return;
	arr_1t tmp(*this);
	swap(tmp);
}

template<typename T,typename A>
void arr_1t<T,A>::pop_back()
{
	if(extra().size==0)
	{
		Exception::XNotFound();
	}
	this->get_allocator().destroy(m_ptr+extra().size-1);
	extra().size--;
}

template<typename T,typename A>
typename arr_1t<T,A>::reference  arr_1t<T,A>::back()
{
	if(extra().size==0)
	{
		Exception::XNotFound();
	}
	return m_ptr[extra().size-1];
}

template<typename T,typename A>
typename arr_1t<T,A>::reference arr_1t<T,A>::front()
{
	if(extra().size==0)
	{
		Exception::XNotFound();
	}
	return m_ptr[0];
}

template<typename T,typename A>
typename arr_1t<T,A>::const_reference arr_1t<T,A>::back() const
{
	if(extra().size==0)
	{
		Exception::XNotFound();
	}
	return m_ptr[extra().size-1];
}

template<typename T,typename A>
typename arr_1t<T,A>::const_reference arr_1t<T,A>::front() const
{
	if(extra().size==0)
	{
		Exception::XNotFound();
	}
	return m_ptr[0];
}

template<typename T,typename A>
void arr_1t<T,A>::clear()
{
	xmem<T>::destroy(begin(),end());
	extra().size=0;
}

template<typename T,typename A>
typename  arr_1t<T,A>::size_type arr_1t<T,A>::size() const
{
	return extra().size;
}

template<typename T,typename A>
typename arr_1t<T,A>::size_type arr_1t<T,A>::capacity() const
{
	return extra().capacity;
}

template<typename T,typename A>
bool arr_1t<T,A>::empty() const
{
	return extra().size==0;
}


template<typename T,typename A>
void arr_1t<T,A>::reserve(size_type capacity_)
{
	if(capacity_<extra().capacity)
	{
		return;
	}

	arr_1t tmp;
	tmp._xset(capacity_);
	size_type _oldsize=extra().size;
	xmem<T>::uninitialized_copy_n(m_ptr,_oldsize,tmp.m_ptr);
	tmp.extra().size=_oldsize;

	swap(tmp);
}

template<typename T,typename A>
void arr_1t<T,A>::resize(size_type newsize_,const T& val_)
{
	size_type _capacity=extra().capacity;
	if(newsize_<=_capacity)
	{
		size_type _oldsize=extra().size;
		if(newsize_>=_oldsize)
		{
			xmem<T>::uninitialized_fill(m_ptr+_oldsize,m_ptr+newsize_,val_);
			extra().size=newsize_;
		}
		else
		{
			xmem<T>::destroy(m_ptr+newsize_,m_ptr+_oldsize);
			extra().size=newsize_;
		}
	}
	else
	{
		size_type _oldsize=extra().size;
		arr_1t tmp;
		tmp._xset(newsize_);
		xmem<T>::uninitialized_copy_n(m_ptr,_oldsize,tmp.m_ptr);
		try
		{
			xmem<T>::uninitialized_fill(tmp.m_ptr+_oldsize,tmp.m_ptr+newsize_,val_);
		}
		catch(...)
		{
			xmem<T>::destroy(tmp.m_ptr,_oldsize);
			throw;
		}
		tmp.extra().size=newsize_;
		swap(tmp);
	}
}

template<typename T,typename A>
template<typename It>
void arr_1t<T,A>::append(It first_,size_type count_)
{
	size_type _oldsize=extra().size;
	size_type _newsize=_oldsize+count_;

	if(_newsize<=extra().capacity)
	{
		xmem<T>::uninitialized_copy_n(first_,count_,m_ptr+_oldsize);
		extra().size=_newsize;
	}
	else
	{
		arr_1t tmp;
		tmp._xgen(_newsize);

		xmem<T>::uninitialized_copy_n(m_ptr,_oldsize,tmp.m_ptr);
		try
		{
			xmem<T>::uninitialized_copy_n(first_,count_,tmp.m_ptr+_oldsize);
		}
		catch(...)
		{
			xmem<T>::destroy(tmp.m_ptr,_oldsize);
			throw;
		}

		tmp.extra().size=_newsize;
		swap(tmp);
	}
}

template<typename T,typename A>
template<typename It>
typename arr_1t<T,A>::iterator arr_1t<T,A>::insert(const_iterator where_,It first_,size_type count_)
{
	size_type _oldsize=extra().size;
	size_type _newsize=_oldsize+count_;
	size_type _tmpsize=where_-begin();

	if(_newsize<=extra().capacity && (size_t)(&(*first_)-m_ptr)>_oldsize)
	{
		iterator _itbeg((T*)&(*where_));
		iterator _itend(end());

		size_type _datsize=_oldsize-_tmpsize;
		if(_datsize>=count_)
		{
			iterator _itpos=_itend-count_;
			xmem<T>::uninitialized_copy_n(_itpos,count_,_itend);
			extra().size=_newsize;
			xmem<T>::copy_backward(_itbeg,_itpos,_itend);
			xmem<T>::copy_n(first_,count_,_itbeg);
		}
		else
		{
			xmem<T>::uninitialized_copy(_itbeg,_itend,_itend);
			extra().size=_newsize;
			xmem<T>::copy_n(first_,_datsize,_itbeg);
			xmem<T>::uninitialized_copy_n(first_+_datsize,count_-_datsize,_itbeg+_datsize);
		}

		return begin()+_tmpsize+count_;

	}
	else
	{
		arr_1t<T,A> tmp;
		tmp._xgen(_newsize);

		xmem<T>::uninitialized_copy_n(begin(),_tmpsize,tmp.m_ptr);
		try
		{
			xmem<T>::uninitialized_copy_n(first_,count_,tmp.m_ptr+_tmpsize);
		}
		catch(...)
		{
			xmem<T>::destroy(tmp.m_ptr,_tmpsize);
			throw;
		}
		tmp.extra().size=_tmpsize+count_;
		try
		{
			xmem<T>::uninitialized_copy_n(m_ptr+_tmpsize,_oldsize-_tmpsize,tmp.m_ptr+tmp.extra().size);
		}
		catch(...)
		{
			xmem<T>::destroy(tmp.m_ptr,tmp.extra().size);
			throw;
		}

		tmp.extra().size=_newsize;
		swap(tmp);

		return begin()+_tmpsize+count_;
	}
}

template<typename T,typename A>
void arr_1t<T,A>::assign(size_type count_,const T& val_)
{
	if(count_>extra().capacity)
	{
		xmem<T>::destroy(begin(),end());
		extra().size=0;
		_xset(count_);
		xmem<T>::uninitialized_fill_n(begin(),count_,val_);
		extra().size=count_;
		return;
	}

	size_type _oldsize=extra().size;

	if(count_<=_oldsize)
	{
		xmem<T>::fill_n(begin(),count_,val_);
		xmem<T>::destroy(begin()+count_,end());
		extra().size=count_;
	}
	else
	{
		xmem<T>::fill_n(begin(),count_,val_);
		xmem<T>::uninitialized_fill_n(begin()+_oldsize,count_-_oldsize,val_);
		extra().size=count_;
	}
}

template<typename T,typename A>
template<typename It>
void arr_1t<T,A>::assign(It first_,size_type count_)
{
	if(count_>extra().capacity||(size_type)(&(*first_)-m_ptr)<size())
	{
		arr_1t<T,A> tmp;
		tmp._xset(count_);
		xmem<T>::uninitialized_copy_n(first_,count_,tmp.m_ptr);
		tmp.extra().size=count_;

		swap(tmp);
		return;
	}

	size_type _oldsize=extra().size;

	if(count_<=_oldsize)
	{
		xmem<T>::copy_n(first_,count_,m_ptr);
		xmem<T>::destroy(begin()+count_,end());
		extra().size=count_;
	}
	else
	{
		xmem<T>::copy_n(first_,_oldsize,m_ptr);
		xmem<T>::uninitialized_copy_n(first_+_oldsize,count_-_oldsize,m_ptr+_oldsize);
		extra().size=count_;
	}
}

template<typename T,typename A>
typename arr_1t<T,A>::iterator  arr_1t<T,A>::erase(iterator position_)
{
	return erase(position_,position_+1);
}

template<typename T,typename A>
typename arr_1t<T,A>::iterator arr_1t<T,A>::erase(iterator first_,iterator last_)
{
	if(first_==last_) return first_;
	iterator tmp(first_);
	iterator it=xmem<T>::copy(last_,end(),first_);
	xmem<T>::destroy(it,end());
	extra().size-=last_-first_;

	return first_;
}


VHWD_LEAVE
#endif
