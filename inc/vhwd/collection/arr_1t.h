// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_COLLECTION_ARR_1t__
#define __H_VHWD_COLLECTION_ARR_1t__


#include "vhwd/collection/detail/collection_base.h"
#include "vhwd/collection/detail/arr_container.h"
VHWD_ENTER

#pragma push_macro("new")
#undef new

template<typename T,typename A>
class arr_1t : public containerB< arr_container<T,typename A::template rebind<T>::other,tl::is_pod<T>::value> >
{
protected:

	typedef containerB< arr_container<T,typename A::template rebind<T>::other,tl::is_pod<T>::value> > basetype;
	using basetype::impl;

public:

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


	arr_1t() {}
	arr_1t(const A& al):basetype(al){}
	arr_1t(const arr_1t& o):basetype(o){}
	arr_1t& operator=(const arr_1t& o){impl=o.impl;return *this;}

#if defined(VHWD_C11)
	arr_1t(arr_1t&& p)
	{
		swap(p);
	}
	arr_1t& operator=(arr_1t&& p)
	{
		swap(p);
		return *this;
	}
#endif


	size_type capacity() const{return impl.capacity();}

	void reserve(size_type capacity_){impl.reserve(capacity_);}

	void resize(size_type newsize_,const T& value_=T()){impl.resize(newsize_,value_);}

	void assign(size_type count_,const T& val_){impl.assign(count_,val_);}

	template<typename It>
	void assign(It first_,size_type count_){impl.assign(first_,count_);}

	template<typename It>
	void assign(It first_,It last_){impl.assign(first_,last_);}

	iterator insert(const_iterator where_,const T& val_)
	{
		return impl.insert(where_,val_);
	}

	template<typename It>
	iterator insert(const_iterator where_,It first_,size_type count_)
	{
		return impl.insert(where_,first_,count_);
	}

	template<typename It>
	iterator insert(const_iterator where_,It first_,It last_)
	{
		return impl.insert(where_,first_,last_);		
	}

	iterator append(const T& val_)
	{
		return impl.append(val_);
	}

	template<typename It>
	iterator append(It first_,size_type count_)
	{
		return impl.append(first_,count_);
	}

	template<typename It>
	iterator append(It first_,It last_)
	{
		return impl.append(first_,last_);
	}

	iterator erase(iterator position_)
	{
		return impl.erase(position_);
	}

	iterator erase(iterator first_,iterator last_)
	{
		return impl.erase(first_,last_);
	}

	void shrink_to_fit()
	{
		impl.shrink_to_fit();
	}

	reference front(){return impl.front();}
	reference back(){return impl.back();}

	const_reference front() const{return impl.front();}
	const_reference back() const{return impl.back();}

	void push_back(const T& val_){impl.append(val_);}

	void pop_back(){impl.pop_back();}

	reference at (size_type n)
	{
		return impl.at(n);
	}

	const_reference at (size_type n) const
	{
		return impl.at(n);
	}

	pointer data()
	{
		return impl.data();
	}

	const_pointer data() const
	{
		return impl.data();
	}

	T& operator[](size_type n){return impl[n];}
	const T& operator[](size_type n) const {return impl[n];}

};

template<typename T,typename A1,typename A2>
bool operator==(const arr_1t<T,A1>& lhs,const arr_1t<T,A2>& rhs)
{
	if(lhs.size()!=rhs.size()) return false;
	for(size_t i=0; i<lhs.size(); i++)
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

//
//template<typename T,typename A>
//arr_1t<T,A>::arr_1t(const arr_1t& o):basetype(o.get_allocator())
//{
//	size_t sz=o.size();
//	m_ptr=_alloc_copy(o.size(),o.m_ptr,o.extra().m_end);
//	extra().m_end=extra().m_last=m_ptr+sz;
//}
//
//template<typename T,typename A>
//arr_1t<T,A>& arr_1t<T,A>::operator=(const arr_1t& o)
//{
//	if(this==&o) return *this;
//	arr_1t(o).swap(*this);
//	return *this;
//}
//
//template<typename T,typename A>
//arr_1t<T,A>::arr_1t(const A& al):basetype(al)
//{
//
//}
//
//template<typename T,typename A>
//arr_1t<T,A>::~arr_1t()
//{
//	xmem<T>::destroy(begin(),end());
//	_dealloc(m_ptr,extra().m_last-m_ptr);
//}
//
//template<typename T,typename A>
//void arr_1t<T,A>::shrink_to_fit()
//{
//	if(extra().m_end==extra().m_last) return;
//	arr_1t(o).swap(*this);
//}
//
//template<typename T,typename A>
//void arr_1t<T,A>::pop_back()
//{
//	_check_not_empty();	
//	extra().m_end->~T();
//	extra().m_end--;
//}
//
//template<typename T,typename A>
//void arr_1t<T,A>::push_back(const T& val_)
//{
//	if(extra().m_end==extra().m_last)
//	{
//		append(&val_,1);		
//	}
//	else
//	{
//		new(extra().m_end) T(val_);
//		++extra().m_end;
//	}
//}
//
//
//
//template<typename T,typename A>
//typename arr_1t<T,A>::reference  arr_1t<T,A>::back()
//{
//	_check_not_empty();
//	return *(extra().m_end-1);
//}
//
//template<typename T,typename A>
//typename arr_1t<T,A>::reference arr_1t<T,A>::front()
//{
//	_check_not_empty();	
//	return m_ptr[0];
//}
//
//template<typename T,typename A>
//typename arr_1t<T,A>::const_reference arr_1t<T,A>::back() const
//{
//	_check_not_empty();
//	return *(extra().m_end-1);
//}
//
//template<typename T,typename A>
//typename arr_1t<T,A>::const_reference arr_1t<T,A>::front() const
//{
//	_check_not_empty();
//	return m_ptr[0];
//}
//
//template<typename T,typename A>
//void arr_1t<T,A>::clear()
//{
//	xmem<T>::destroy(begin(),end());
//	extra().m_end=m_ptr;
//}
//
//template<typename T,typename A>
//typename  arr_1t<T,A>::size_type arr_1t<T,A>::size() const
//{
//	return extra().size;
//}
//
//template<typename T,typename A>
//typename arr_1t<T,A>::size_type arr_1t<T,A>::capacity() const
//{
//	return extra().capacity;
//}
//
//template<typename T,typename A>
//bool arr_1t<T,A>::empty() const
//{
//	return extra().size==0;
//}
//
//
//template<typename T,typename A>
//void arr_1t<T,A>::reserve(size_type capacity_)
//{
//	if(capacity_<=capacity())
//	{
//		return;
//	}
//	size_t sz=size();
//	m_ptr=_alloc_copy(capacity_,m_ptr,sz);
//	extra().m_end=m_ptr+sz;
//	extra().m_last=m_ptr+capacity_;
//	
//
//}
//
//template<typename T,typename A>
//void arr_1t<T,A>::resize(size_type newsize_,const T& val_)
//{
//	size_type _capacity=capacity();
//	if(_capacity<newsize_)
//	{
//		reserve(newsize_);
//	}
//
//	size_type _oldsize=size();
//	if(newsize_>=_oldsize)
//	{
//		xmem<T>::uninitialized_fill(m_ptr+_oldsize,m_ptr+newsize_,val_);
//		extra().m_end=m_ptr+newsize_;
//	}
//	else
//	{
//		xmem<T>::destroy(m_ptr+newsize_,m_ptr+_oldsize);
//		extra().m_end=m_ptr+newsize_;
//	}
//
//}
//
//template<typename T,typename A>
//template<typename It>
//void arr_1t<T,A>::append(It first_,size_type count_)
//{
//	if(count_==0) return;
//
//	size_type _oldsize=size();
//	size_type _newsize=_oldsize+count_;
//
//	if(_newsize>capacity())
//	{
//		reserve(sz_helper::gen(_newsize));
//	}
//
//	xmem<T>::uninitialized_copy_n(first_,count_,extra().m_end);
//	extra().m_end=m_ptr+_newsize;
//	
//}
//
//template<typename T,typename A>
//template<typename It>
//typename arr_1t<T,A>::iterator arr_1t<T,A>::insert(const_iterator where_,It first_,size_type count_)
//{
//	size_type _oldsize=size();
//	size_type _newsize=_oldsize+count_;
//	size_type _tmpsize=where_-begin();
//
//	if(_newsize<=capacity() && (size_t)(&(*first_)-m_ptr)>_oldsize)
//	{
//		iterator _itbeg((T*)&(*where_));
//		iterator _itend(end());
//
//		size_type _datsize=_oldsize-_tmpsize;
//		if(_datsize>=count_)
//		{
//			iterator _itpos=_itend-count_;
//			xmem<T>::uninitialized_copy_n(_itpos,count_,_itend);
//			extra().size=_newsize;
//			xmem<T>::copy_backward(_itbeg,_itpos,_itend);
//			xmem<T>::copy_n(first_,count_,_itbeg);
//		}
//		else
//		{
//			xmem<T>::uninitialized_copy(_itbeg,_itend,_itend);
//			extra().size=_newsize;
//			xmem<T>::copy_n(first_,_datsize,_itbeg);
//			xmem<T>::uninitialized_copy_n(first_+_datsize,count_-_datsize,_itbeg+_datsize);
//		}
//
//		return begin()+_tmpsize+count_;
//
//	}
//	else
//	{
//
//		arr_1t<T,A> tmp;
//
//		size_t sz=sz_helper::gen(_newsize);
//		T* _tmp=_alloc_copy(sz,m_ptr,_tmpsize);
//		T* _szt=_tmp+_tmpsize;
//
//		try
//		{
//			xmem<T>::uninitialized_copy_n(first_,count_,_tmp+_tmpsize);
//		}
//		catch(...)
//		{
//			xmem<T>::destroy(_tmp,_tmpsize);
//			throw;
//		}
//		_szt+=count_;
//		try
//		{
//			xmem<T>::uninitialized_copy_n(m_ptr+_tmpsize,_oldsize-_tmpsize,_szt);
//		}
//		catch(...)
//		{
//			xmem<T>::destroy(tmp.m_ptr,_tmpsize+count_);
//			throw;
//		}
//
//		_szt+=_oldsize-_tmpsize;
//
//		clear();
//		_dealloc(m_ptr,capacity());
//		m_ptr=_tmp;
//		extra().m_last=m_ptr+sz;
//		extra().m_end=_szt;
//
//		return m_ptr+_tmpsize+count_;
//	}
//}
//
//template<typename T,typename A>
//void arr_1t<T,A>::assign(size_type count_,const T& val_)
//{
//	arr_1t tmp;
//	tmp.resize(count_,val_);
//	swap(tmp);
//}
//
//template<typename T,typename A>
//template<typename It>
//void arr_1t<T,A>::assign(It first_,size_type count_)
//{
//	arr_1t tmp;
//	tmp.reserve(count_);
//	xmem<T>::uninitialized_copy_n(first_,count_,tmp.m_ptr);
//	tmp.extra().m_end=tmp.m_ptr+count_;
//	swap(tmp);
//}
//
//template<typename T,typename A>
//typename arr_1t<T,A>::iterator  arr_1t<T,A>::erase(iterator position_)
//{
//	return erase(position_,position_+1);
//}
//
//template<typename T,typename A>
//typename arr_1t<T,A>::iterator arr_1t<T,A>::erase(iterator first_,iterator last_)
//{
//	if(first_==last_) return first_;
//	iterator tmp(first_);
//	iterator it=xmem<T>::copy(last_,end(),first_);
//	xmem<T>::destroy(it,end());
//	extra().m_end-=last_-first_;
//
//	return first_;
//}

template<typename T,typename A> class hash_t<arr_1t<T,A> >
{
public:
	uint32_t operator()(const arr_1t<T,A>& o)
	{
		return hash_array<T>::hash(o.data(),o.size());
	}
};

#pragma pop_macro("new")

VHWD_LEAVE
#endif
