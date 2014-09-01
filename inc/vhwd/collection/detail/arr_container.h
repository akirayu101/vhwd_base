// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_COLLECTION_POD_1t__
#define __H_VHWD_COLLECTION_POD_1t__


#include "vhwd/collection/detail/collection_base.h"

VHWD_ENTER

#pragma push_macro("new")
#undef new

template<typename T,typename A,bool pod>
class arr_container : public containerS<A>
{
public:

	typedef containerS<A> basetype;

	typedef T value_type;
	typedef T* pointer;
	typedef T& reference;

	typedef size_t size_type;

	typedef typename basetype::iterator iterator;
	typedef typename basetype::reverse_iterator reverse_iterator;
	typedef typename basetype::const_iterator const_iterator;
	typedef typename basetype::const_reverse_iterator const_reverse_iterator;

	arr_container();
	arr_container(const arr_container& o);
	arr_container& operator=(const arr_container& o);

	~arr_container();

	void swap(arr_container& o)
	{
		std::swap(m_ptr,o.m_ptr);
		std::swap(m_end,o.m_end);
		std::swap(m_last,o.m_last);
	}

	void clear();

	size_type size() const
	{
		return m_end-m_ptr;
	}

	size_type capacity() const
	{
		return m_last-m_ptr;
	}

	bool empty() const
	{
		return m_ptr==m_end;
	}

	void reserve(size_type capacity_);
	void resize(size_type newsize_,const T& value_=T());

	void assign(size_type count_,const T& val_);

	template<typename It>
	void assign(It first_,size_type count_);

	template<typename It>
	void assign(It first_,It last_)
	{
		assign(first_,std::distance(first_,last_));
	}

	iterator insert(const_iterator where_,const T& val_)
	{
		return insert(where_,&val_,1);
	}

	template<typename It>
	iterator insert(const_iterator where_,It first_,size_type count_);

	template<typename It>
	iterator insert(const_iterator where_,It first_,It last_)
	{
		return insert(where_,first_,std::distance(first_,last_));
	}

	iterator append(const T& val_);

	template<typename It>
	iterator append(It first_,size_type count_);

	template<typename It>
	iterator append(It first_,It last_)
	{
		return append(first_,std::distance(first_,last_));
	}

	iterator erase(iterator position_);
	iterator erase(iterator first_,iterator last_);

	void shrink_to_fit();

	reference front();
	reference back();

	void push_back(const T& val_){append(val_);}
	void pop_back();

	iterator begin(){return m_ptr;}
	iterator end(){return m_end;}
	reverse_iterator rbegin(){return m_end-1;}
	reverse_iterator rend(){return m_ptr-1;}

	reference at (size_type n)
	{
		T* p=m_ptr+n;
		if(p>=m_end) Exception::XInvalidIndex();
		return *p;
	}

	pointer data(){return m_ptr;}
	T& operator[](size_type n){return m_ptr[n];}

protected:

	//void _xgen(size_t sz1)
	//{
	//	size_t sz2=sz_helper::gen(sz1);
	//	reserve(sz2);
	//}

	T* _alloc(size_t n)
	{
		return this->get_allocator().allocate(n);
	}

	T* _alloc_copy(size_t n,const T& v,size_t s)
	{
		T* _tmp=_alloc(n);

		if(pod)
		{
			for(size_t i=0;i<s;i++) _tmp[i]=v;
		}
		else
		{
			try
			{
				xmem<T>::uninitialized_fill_n(_tmp,s,v);
			}
			catch(...)
			{
				_dealloc(_tmp,n);
				throw;
			}
		}
		return _tmp;
	}

	T* _alloc_copy(size_t n,T* b1,size_t s)
	{
		T* _tmp=_alloc(n);

		if(s==0) return _tmp;

		if(pod)
		{
			memcpy(_tmp,b1,sizeof(T)*s);
		}
		else
		{
			try
			{
				xmem<T>::uninitialized_copy_n(b1,s,_tmp);
			}
			catch(...)
			{
				_dealloc(_tmp,n);
				throw;
			}
		}
		return _tmp;
	}
	T* _alloc_copy(size_t n,T* b1,T* b2)
	{
		T* _tmp=_alloc(n);

		if(b1==b2) return _tmp;

		if(pod)
		{
			memcpy(_tmp,b1,sizeof(T)*(b2-b1));
		}
		else
		{
			try
			{
				xmem<T>::uninitialized_copy(b1,b2,_tmp);
			}
			catch(...)
			{
				_dealloc(_tmp,n);
				throw;
			}
		}
		return _tmp;
	}

	void _dealloc(T* p,size_t n)
	{
		this->get_allocator().deallocate(p,n);
	}

	void _init()
	{
		m_ptr=m_end=m_last=NULL;
	}

	void _check_not_empty()
	{
		if(m_ptr==m_end)
		{
			Exception::XError("arr_container is empty()",false);
		}
	}

	pointer m_ptr;
	pointer m_end;
	pointer m_last;

};




template<typename T,typename A,bool pod>
arr_container<T,A,pod>::arr_container():m_ptr(NULL),m_end(NULL),m_last(NULL)
{

}

template<typename T,typename A,bool pod>
arr_container<T,A,pod>::arr_container(const arr_container& o):m_ptr(NULL),m_end(NULL),m_last(NULL)
{
	size_t sz=o.size();
	if(sz>0)
	{
		m_ptr=(T*)_alloc_copy(sz,o.m_ptr,o.m_end);
		m_end=m_last=m_ptr+sz;
	}
}

template<typename T,typename A,bool pod>
arr_container<T,A,pod>& arr_container<T,A,pod>::operator=(const arr_container& o)
{
	if(this==&o) return *this;
	arr_container(o).swap(*this);
	return *this;
}


template<typename T,typename A,bool pod>
arr_container<T,A,pod>::~arr_container()
{
	clear();
}


template<typename T,typename A,bool pod>
void arr_container<T,A,pod>::shrink_to_fit()
{
	if(m_end==m_last) return;
	arr_container tmp(*this);
	swap(tmp);
}

template<typename T,typename A,bool pod>
void arr_container<T,A,pod>::pop_back()
{
	_check_not_empty();
	--m_end;
	m_end->~T();
}

template<typename T,typename A,bool pod>
typename arr_container<T,A,pod>::reference  arr_container<T,A,pod>::back()
{
	_check_not_empty();
	return *(m_end-1);
}

template<typename T,typename A,bool pod>
typename arr_container<T,A,pod>::reference arr_container<T,A,pod>::front()
{
	_check_not_empty();
	return *m_ptr;
}

template<typename T,typename A,bool pod>
void arr_container<T,A,pod>::clear()
{
	if(!m_ptr) return;
	xmem<T>::destroy(m_ptr,m_end);
	_dealloc(m_ptr,m_last-m_ptr);
	m_ptr=m_end=m_last=NULL;
}


template<typename T,typename A,bool pod>
void arr_container<T,A,pod>::reserve(size_type capacity_)
{
	if(capacity()>=capacity_)
	{
		return;
	}

	arr_container tmp;
	tmp.m_ptr=_alloc_copy(capacity_,m_ptr,m_end);
	tmp.m_end=tmp.m_ptr+size();
	tmp.m_last=tmp.m_ptr+capacity_;

	swap(tmp);
}

template<typename T,typename A,bool pod>
void arr_container<T,A,pod>::resize(size_type newsize_,const T& val_)
{
	size_t n0=size();
	if(n0>=newsize_)
	{
		T* _end2=m_ptr+newsize_;
		xmem<T>::destroy(_end2,m_end);
		m_end=_end2;
	}
	else if(capacity()>=newsize_)
	{
		T* _end2=m_ptr+newsize_;
		xmem<T>::uninitialized_fill(m_end,_end2,val_);
		m_end=_end2;
	}
	else
	{
		arr_container tmp;
		tmp.m_ptr=_alloc_copy(newsize_,m_ptr,m_end);
		tmp.m_end=tmp.m_ptr+n0;
		tmp.m_last=tmp.m_ptr+newsize_;
		xmem<T>::uninitialized_fill(tmp.m_end,tmp.m_last,val_);
		tmp.m_end=tmp.m_last;
		swap(tmp);
	}
}



template<typename T,typename A,bool pod>
typename arr_container<T,A,pod>::iterator arr_container<T,A,pod>::append(const T& val_)
{
	if(m_end==m_last)
	{
		arr_container tmp;
		size_t n0=size();
		size_t sz=sz_helper::gen(n0+1);
		tmp.m_ptr=_alloc_copy(sz,m_ptr,m_end);
		tmp.m_last=tmp.m_ptr+sz;
		tmp.m_end=tmp.m_ptr+n0;
		new(tmp.m_end) T(val_);
		++tmp.m_end;
		swap(tmp);

	}
	else
	{
		new(m_end) T(val_);
		++m_end;
	}

	return m_end;

}

template<typename T,typename A,bool pod>
template<typename It>
typename arr_container<T,A,pod>::iterator arr_container<T,A,pod>::append(It first_,size_type count_)
{
	T* _end2=m_end+count_;
	if(_end2>m_last)
	{
		arr_container tmp;
		size_t n1=size();
		size_t sz=sz_helper::gen(n1+count_);

		tmp.m_ptr=_alloc_copy(sz,m_ptr,m_end);
		tmp.m_last=tmp.m_ptr+sz;
		tmp.m_end=tmp.m_ptr+n1;

		xmem<T>::uninitialized_copy_n(first_,count_,tmp.m_end);
		tmp.m_end+=count_;

		swap(tmp);
	}
	else
	{
		xmem<T>::uninitialized_copy_n(first_,count_,m_end);
		m_end+=count_;
	}
	return m_end;
}

template<typename T,typename A,bool pod>
template<typename It>
typename arr_container<T,A,pod>::iterator arr_container<T,A,pod>::insert(const_iterator where_,It first_,size_type count_)
{
	if(where_==end())
	{
		return append(first_,count_);
	}

	arr_container tmp;
	size_t sz=sz_helper::gen(size()+count_);

	T* p1=(T*)&(*where_);
	tmp.reserve(sz);
	tmp.append(m_ptr,p1);
	tmp.append(first_,count_);
	tmp.append(p1,m_end);

	T* p2=tmp.m_ptr+((p1-m_ptr)+count_);
	swap(tmp);

	return p2;


	//size_t p=where_-begin();
	//_xgen(size()+count_);

	//T* p0=m_ptr+p;
	//T* p1=p0-1;
	//T* p2=m_end-1;

	//for(T* p=p2;p>p1;--p)
	//{
	//	*(p+count_)=*p;
	//}

	//T* p3=p0+count_;
	//while(p0<p3)
	//{
	//	*p0++=*first_++;
	//}

	//m_end+=count_;
	//return p3;
}

template<typename T,typename A,bool pod>
void arr_container<T,A,pod>::assign(size_type newsize_,const T& val_)
{
	if(size()>=newsize_)
	{
		T* _end2=m_ptr+newsize_;
		xmem<T>::destroy(_end2,m_end);
		m_end=_end2;

		xmem<T>::fill_n(m_ptr,newsize_,val_);
	}
	else if(capacity()>=newsize_)
	{
		xmem<T>::fill(m_ptr,m_end,val_);

		T* _end2=m_ptr+newsize_;
		xmem<T>::uninitialized_fill(m_end,_end2,val_);
		m_end=_end2;

	}
	else
	{
		arr_container tmp;
		tmp.m_ptr=_alloc_copy(newsize_,val_,newsize_);
		tmp.m_end=tmp.m_ptr+newsize_;
		tmp.m_last=tmp.m_ptr+newsize_;

		swap(tmp);
	}
}

template<typename T,typename A,bool pod>
template<typename It>
void arr_container<T,A,pod>::assign(It first_,size_type newsize_)
{
	if(size()>=newsize_)
	{
		T* _end2=m_ptr+newsize_;
		xmem<T>::destroy(_end2,m_end);
		m_end=_end2;

		xmem<T>::copy_n(first_,newsize_,m_ptr);
	}
	else if(capacity()>=newsize_)
	{
		T* p0=m_ptr;
		T* p1=m_end;
		T* p2=p0+newsize_;

		while(p0<p1)
		{
			*p0++=*first_++;
		}

		xmem<T>::uninitialized_copy_n(first_,p2-p1,p1);
		m_end=p2;
	}
	else
	{
		arr_container tmp;
		tmp.m_ptr=_alloc_copy(newsize_,(T*)&(*first_),newsize_);
		tmp.m_end=tmp.m_ptr+newsize_;
		tmp.m_last=tmp.m_ptr+newsize_;

		swap(tmp);
	}
}

template<typename T,typename A,bool pod>
typename arr_container<T,A,pod>::iterator  arr_container<T,A,pod>::erase(iterator position_)
{
	return erase(position_,position_+1);
}

template<typename T,typename A,bool pod>
typename arr_container<T,A,pod>::iterator arr_container<T,A,pod>::erase(iterator first_,iterator last_)
{
	T* _end2=m_end-(last_-first_);
	xmem<T>::copy(last_,end(),first_);
	xmem<T>::destroy(_end2,m_end);
	m_end=_end2;
	return first_;
}


template<typename T,typename A,bool pod>
bool operator==(const arr_container<T,A,pod>& lhs,const arr_container<T,A,pod>& rhs)
{
	if(lhs.size()!=rhs.size())
	{
		return false;
	}
	for(size_t i=0; i<lhs.size(); i++)
	{
		if(lhs[i]!=rhs[i]) return false;
	}
	return true;
}

template<typename T,typename A,bool pod>
bool operator!=(const arr_container<T,A,pod>& lhs,const arr_container<T,A,pod>& rhs)
{
	return !(lhs==rhs);
}

template<typename T,typename A,bool pod> class hash_t<arr_container<T,A,pod> >
{
public:
	uint32_t operator()(const arr_container<T,A,pod>& o)
	{
		return hash_array<T>::hash(o.data(),o.size());
	}
};

#pragma pop_macro("new")

VHWD_LEAVE

#endif
