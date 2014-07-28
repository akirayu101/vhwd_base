// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_COLLECTION_ARRAY_XT__
#define __H_VHWD_COLLECTION_ARRAY_XT__


#include "vhwd/collection/detail/collection_base.h"

VHWD_ENTER


template<typename T,typename A>
class arr_xt : public arr_base<T,A,extra_arr_xt>
{
public:
	typedef arr_base<T,A,extra_arr_xt> basetype;

private:
	using basetype::m_ptr;
	using basetype::extra;

public:

	typedef extra_arr_xt::size_type size_type;
	typedef typename basetype::iterator iterator;
	typedef typename basetype::const_iterator const_iterator;

	using basetype::swap;

	static const size_type MAX_DIM=extra_arr_xt::MAX_DIM;

	arr_xt() {}
	arr_xt(const A& al):basetype(al) {}
	arr_xt(const arr_xt& o);
	arr_xt& operator=(const arr_xt& o);

	~arr_xt()
	{
		clear();
	}

#if defined(VHWD_C11)
	arr_xt(arr_xt&& p)
	{
		swap(p);
	}
	arr_xt& operator=(arr_xt&& p)
	{
		swap(p);
		return *this;
	}
#endif

	iterator begin()
	{
		return m_ptr;
	}
	iterator end()
	{
		return m_ptr+extra().size;
	}

	const_iterator begin() const
	{
		return m_ptr;
	}
	const_iterator end() const
	{
		return m_ptr+extra().size;
	}

	void resize(size_type k0,size_type k1=1,size_type k2=1,size_type k3=1,size_type k4=1,size_type k5=1);
	void clear();

	inline T& operator()(size_type k0)
	{
		return m_ptr[k0];
	}
	inline T& operator()(size_type k0,size_type k1)
	{
		return m_ptr[k0+extra().dims[0]*k1];
	}
	inline T& operator()(size_type k0,size_type k1,size_type k2)
	{
		return m_ptr[k0+extra().dims[0]*(k1+extra().dims[1]*k2)];
	}
	inline T& operator()(size_type k0,size_type k1,size_type k2,size_type k3)
	{
		return m_ptr[k0+extra().dims[0]*(k1+extra().dims[1]*(k2+extra().dims[2]*k3))];
	}
	inline T& operator()(size_type k0,size_type k1,size_type k2,size_type k3,size_type k4)
	{
		return m_ptr[k0+extra().dims[0]*(k1+extra().dims[1]*(k2+extra().dims[2]*(k3+extra().dims[3]*k4)))];
	}
	inline T& operator()(size_type k0,size_type k1,size_type k2,size_type k3,size_type k4,size_type k5)
	{
		return m_ptr[k0+extra().dims[0]*(k1+extra().dims[1]*(k2+extra().dims[2]*(k3+extra().dims[3]*(k4+extra().dims[4]*k5))))];
	}

	inline const T& operator()(size_type k0) const
	{
		return m_ptr[k0];
	}
	inline const T& operator()(size_type k0,size_type k1) const
	{
		return m_ptr[k0+extra().dims[0]*k1];
	}
	inline const T& operator()(size_type k0,size_type k1,size_type k2) const
	{
		return m_ptr[k0+extra().dims[0]*(k1+extra().dims[1]*k2)];
	}
	inline const T& operator()(size_type k0,size_type k1,size_type k2,size_type k3) const
	{
		return m_ptr[k0+extra().dims[0]*(k1+extra().dims[1]*(k2+extra().dims[2]*k3))];
	}
	inline const T& operator()(size_type k0,size_type k1,size_type k2,size_type k3,size_type k4) const
	{
		return m_ptr[k0+extra().dims[0]*(k1+extra().dims[1]*(k2+extra().dims[2]*(k3+extra().dims[3]*k4)))];
	}
	inline const T& operator()(size_type k0,size_type k1,size_type k2,size_type k3,size_type k4,size_type k5) const
	{
		return m_ptr[k0+extra().dims[0]*(k1+extra().dims[1]*(k2+extra().dims[2]*(k3+extra().dims[3]*(k4+extra().dims[4]*k5))))];
	}

	inline size_type size() const
	{
		return extra().size;
	}
	inline size_type size(int n) const
	{
		return extra().dims[n];
	}
	const size_type* size_ptr() const
	{
		return extra().dims;
	}

	inline T* data()
	{
		return m_ptr;
	}
	inline const T* data() const
	{
		return m_ptr;
	}
};


template<typename T,typename A1,typename A2>
bool operator==(const arr_xt<T,A1>& lhs,const arr_xt<T,A2>& rhs)
{
	typedef size_t size_type;
	if(lhs.size()!=rhs.size()) return false;

	for(size_type i=0; i<lhs.MAX_DIM; i++)
	{
		if(lhs.size(i)!=rhs.size(i)) return false;
	}

	for(size_type i=0; i<lhs.size(); i++)
	{
		if(lhs[i]!=rhs[i]) return false;
	}

	return true;
}



template<typename T,typename A1,typename A2>
bool operator!=(const arr_xt<T,A1>& lhs,const arr_xt<T,A2>& rhs)
{
	return !(lhs==rhs);
}
template<typename T,typename A>
arr_xt<T,A>::arr_xt(const arr_xt& o):basetype(o.get_allocator())
{
	size_type _newsize=o.size();
	this->_xset(_newsize);
	if(_newsize==0) return;

	try
	{
		xmem<T>::uninitialized_copy_n(o.m_ptr,_newsize,m_ptr);
		extra()=o.extra();
		extra().capacity=_newsize;
	}
	catch(...)
	{
		this->_xset(0);
		throw;
	}

}

template<typename T,typename A>
arr_xt<T,A>& arr_xt<T,A>::operator=(const arr_xt& o)
{
	if(this==&o) return *this;
	size_type _newsize=o.size();
	if(_newsize==size())
	{
		xmem<T>::copy_n(o.m_ptr,_newsize,m_ptr);
		extra()=o.extra();
	}
	else
	{
		arr_xt(o).swap(*this);
	}

	return *this;
}

template<typename T,typename A>
void arr_xt<T,A>::resize(size_type k0,size_type k1,size_type k2,size_type k3,size_type k4,size_type k5)
{

	size_type _newsize=k0*k1*k2*k3*k4*k5;
	if( (_newsize>0 && k0!=_newsize/k1/k2/k3/k4/k5)||_newsize>this->get_allocator().max_size())
	{
		Exception::XBadAlloc();
	}

	arr_xt tmp;
	tmp._xset(_newsize);

	if(_newsize==0)
	{
		swap(tmp);
		return;
	}

	xmem<T>::uninitialized_fill_n(tmp.m_ptr,_newsize,T());

	extra_arr_xt &_Enew(tmp.extra());
	extra_arr_xt &_Eold(extra());
	_Enew.size=_newsize;
	_Enew.dims[0]=k0;
	_Enew.dims[1]=k1;
	_Enew.dims[2]=k2;
	_Enew.dims[3]=k3;
	_Enew.dims[4]=k4;
	_Enew.dims[5]=k5;

	if(_newsize>0)
	{
		for(size_type a0=0; a0<std::min(_Eold.dims[0],_Enew.dims[0]); a0++)
			for(size_type a1=0; a1<std::min(_Eold.dims[1],_Enew.dims[1]); a1++)
				for(size_type a2=0; a2<std::min(_Eold.dims[2],_Enew.dims[2]); a2++)
					for(size_type a3=0; a3<std::min(_Eold.dims[3],_Enew.dims[3]); a3++)
						for(size_type a4=0; a4<std::min(_Eold.dims[4],_Enew.dims[4]); a4++)
							for(size_type a5=0; a5<std::min(_Eold.dims[5],_Enew.dims[5]); a5++)
							{
								tmp(a0,a1,a2,a3,a4,a5)=(*this)(a0,a1,a2,a3,a4,a5);
							}
	}

	swap(tmp);
}

template<typename T,typename A>
void arr_xt<T,A>::clear()
{
	xmem<T>::destroy(m_ptr,extra().size);
	extra().size=0;
	extra().dims[0]=0;
	extra().dims[1]=1;
	extra().dims[2]=1;
	extra().dims[3]=1;
	extra().dims[4]=1;
	extra().dims[5]=1;
	this->_xset(0);
}

template<typename T,typename A> class hash_t<arr_xt<T,A> >
{
public:
	uint32_t operator()(const arr_xt<T,A>& o)
	{
		return hash_array<T>::hash(o.data(),o.size());
	}
};

VHWD_LEAVE
#endif
