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


template<typename T,int N>
class pod_1t
{
public:

	typedef T value_type;
	typedef T* pointer;
	typedef T& reference;
	typedef const T* const_pointer;
	typedef const T& const_reference;

	typedef size_t size_type;

	typedef rac_iterator<value_type,true,false> iterator;
	typedef rac_iterator<value_type,true,true> const_iterator;

	typedef rac_iterator<value_type,false,false> reverse_iterator;
	typedef rac_iterator<value_type,false,true> const_reverse_iterator;


	pod_1t(){_init();}
	pod_1t(const pod_1t& o);
	pod_1t& operator=(const pod_1t& o);

	~pod_1t();

	void swap(pod_1t& o)
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

	bool empty() const;

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

	void append(const T& val_)
	{
		append(&val_,1);
	}

	template<typename It>
	void append(It first_,size_type count_);

	template<typename It>
	void append(It first_,It last_)
	{
		append(first_,std::distance(first_,last_));
	}

	iterator erase(iterator position_);
	iterator erase(iterator first_,iterator last_);

	void shrink_to_fit();

	reference front();
	reference back();

	const_reference front() const;
	const_reference back() const;

	void push_back(const T& val_)
	{
		if(m_end==m_last)
		{
			_xgen(size()+1);
		}
		*m_end++=val_;
	}

	void pop_back();

	iterator begin()
	{
		return m_ptr;
	}

	iterator end()
	{
		return m_end;
	}

	const_iterator begin() const
	{
		return m_ptr;
	}
	const_iterator end() const
	{
		return m_end;
	}

	reverse_iterator rbegin()
	{
		return m_end-1;
	}
	reverse_iterator rend()
	{
		return m_ptr-1;
	}

	const_reverse_iterator rbegin() const
	{
		return m_end-1;
	}
	const_reverse_iterator rend() const
	{
		return m_ptr-1;
	}

	reference at (size_type n)
	{
		if(n>=size()) Exception::XInvalidIndex();
		return m_ptr[n];
	}
	const_reference at (size_type n) const
	{
		if(n>=size()) Exception::XInvalidIndex();
		return m_ptr[n];
	}

	pointer data()
	{
		return m_ptr;
	}

	const_pointer data() const
	{
		return m_ptr;
	}

	T& operator[](size_type n)
	{
		return m_ptr[n];
	}

	const T& operator[](size_type n) const
	{
		return m_ptr[n];
	}

protected:

	void _xgen(size_t sz1)
	{
		size_t sz2=sz_helper::gen(sz1);
		reserve(sz2);
	}

	T* _alloc(size_t n)
	{
		size_t sz=sizeof(T)*(n+N);
		if(sz<n)
		{
			Exception::XBadAlloc();
		}
		T* _tmp=(T*)tc_alloc(sizeof(T)*n);
		if(!_tmp)
		{
			Exception::XBadAlloc();
		}
		return _tmp;
	}

	void _dealloc(void* p)
	{
		tc_free(p);
	}

	void _init()
	{
		m_ptr=m_end=m_last=NULL;
	}

	void _check_not_empty()
	{
		if(m_ptr==m_end)
		{
			Exception::XError("pod_1t is empty()",false);
		}
	}

	pointer m_ptr;
	pointer m_end;
	pointer m_last;

};

template<typename T,int N1,int N2>
bool operator==(const pod_1t<T,N1>& lhs,const pod_1t<T,N2>& rhs)
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

template<typename T,int N>
bool operator!=(const pod_1t<T,N>& lhs,const pod_1t<T,N>& rhs)
{
	return !(lhs==rhs);
}


template<typename T,int N>
pod_1t<T,N>::pod_1t(const pod_1t& o)
{
	_init();
	size_t sz=o.size();
	if(sz>0)
	{
		m_ptr=(T*)_alloc(sz);
		m_end=m_last=m_ptr+sz;
		memcpy(m_ptr,o.m_ptr,sizeof(T)*sz);
	}
}

template<typename T,int N>
pod_1t<T,N>& pod_1t<T,N>::operator=(const pod_1t& o)
{
	if(this==&o) return *this;
	size_t sz=o.size();

	if(size()>=sz)
	{
		memcpy(m_ptr,o.m_ptr,sizeof(T)*sz);
		m_end=m_ptr+sz;
	}
	else
	{
		T* _tmp=_alloc(sz);
		memcpy(_tmp,o.m_ptr,sizeof(T)*sz);

		_dealloc(m_ptr);
		m_ptr=_tmp;
		m_end=m_last=m_ptr+sz;
	}

	return *this;
}


template<typename T,int N>
pod_1t<T,N>::~pod_1t()
{
	clear();
}

template<typename T,int N>
bool pod_1t<T,N>::empty() const
{
	return m_ptr==m_end;
}


template<typename T,int N>
void pod_1t<T,N>::shrink_to_fit()
{
	if(m_end==m_last) return;
	pod_1t tmp(*this);
	swap(tmp);
}

template<typename T,int N>
void pod_1t<T,N>::pop_back()
{
	_check_not_empty();
	--m_end;
}

template<typename T,int N>
typename pod_1t<T,N>::reference  pod_1t<T,N>::back()
{
	_check_not_empty();
	return *(m_end-1);
}

template<typename T,int N>
typename pod_1t<T,N>::reference pod_1t<T,N>::front()
{
	_check_not_empty();
	return *m_ptr;
}

template<typename T,int N>
typename pod_1t<T,N>::const_reference pod_1t<T,N>::back() const
{
	_check_not_empty();
	return *(m_end-1);
}

template<typename T,int N>
typename pod_1t<T,N>::const_reference pod_1t<T,N>::front() const
{
	_check_not_empty();
	return *m_ptr;
}

template<typename T,int N>
void pod_1t<T,N>::clear()
{
	_dealloc(m_ptr);
	m_ptr=m_end=m_last=NULL;
}


template<typename T,int N>
void pod_1t<T,N>::reserve(size_type capacity_)
{
	if(capacity()>=capacity_)
	{
		return;
	}

	T* _tmp=_alloc(capacity_);
	size_t sz=size();
	memcpy(_tmp,m_ptr,sizeof(T)*sz);

	_dealloc(m_ptr);

	m_ptr=_tmp;
	m_end=_tmp+sz;
	m_last=_tmp+capacity_;

}

template<typename T,int N>
void pod_1t<T,N>::resize(size_type newsize_,const T& val_)
{
	size_t sz=size();
	if(sz>=newsize_)
	{
		m_end=m_ptr+newsize_;
	}
	else
	{
		reserve(newsize_);

		T* _tmp=m_ptr+newsize_;
		while(m_end<_tmp)
		{
			*m_end++=val_;
		}
	}
}

template<typename T,int N>
template<typename It>
void pod_1t<T,N>::append(It first_,size_type count_)
{
	_xgen(size()+count_);

	T* _tmp=m_end+count_;
	while(m_end<_tmp)
	{
		*m_end++=*first_++;
	}
}

template<typename T,int N>
template<typename It>
typename pod_1t<T,N>::iterator pod_1t<T,N>::insert(const_iterator where_,It first_,size_type count_)
{
	size_t p=where_-begin();

	reserve(size()+count_);

	T* p0=m_ptr+p;
	T* p1=p0-1;
	T* p2=m_end-1;
	for(T* p=p2;p>p1;--p)
	{
		*(p+count_)=*p;
	}

	T* p3=p0+count_;
	while(p0<p3)
	{
		*p0++=*first_++;
	}

	m_end+=count_;
	return p3;
}

template<typename T,int N>
void pod_1t<T,N>::assign(size_type count_,const T& val_)
{
	reserve(count_);
	for(size_t i=0;i<count_;i++)
	{
		m_ptr[i]=val_;
	}
	m_end=m_ptr+count_;
}

template<typename T,int N>
template<typename It>
void pod_1t<T,N>::assign(It first_,size_type count_)
{
	reserve(count_);
	m_end=m_ptr+count_;
	T* p=m_ptr;
	while(p<m_end)
	{
		*p++=*first_++;
	}
}

template<typename T,int N>
typename pod_1t<T,N>::iterator  pod_1t<T,N>::erase(iterator position_)
{
	return erase(position_,position_+1);
}

template<typename T,int N>
typename pod_1t<T,N>::iterator pod_1t<T,N>::erase(iterator first_,iterator last_)
{
	iterator it2=m_end-(last_-first_);

	iterator it=first_;
	while(it<it2)
	{
		*it=*last_++;
	}

	m_end=&(*it2);
	return first_;
}

template<typename T,int N> class hash_t<pod_1t<T,N> >
{
public:
	uint32_t operator()(const pod_1t<T,N>& o)
	{
		return hash_array<T>::hash(o.data(),o.size());
	}
};

VHWD_LEAVE
#endif
