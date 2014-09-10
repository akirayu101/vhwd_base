// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_COLLECTION_ARR_XT__
#define __H_VHWD_COLLECTION_ARR_XT__


#include "vhwd/collection/arr_1t.h"

VHWD_ENTER

template<typename T,typename A=def_allocator>
class arr_xt : public containerB< arr_container<T,typename A::template rebind<T>::other,tl::is_pod<T>::value> >
{
protected:
	typedef containerB< arr_container<T,typename A::template rebind<T>::other,tl::is_pod<T>::value> > basetype;

public:
	static const size_t MAX_DIM=arr_xt_dims::MAX_DIM;

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

	arr_xt(){}
	arr_xt(const A& al):basetype(al){}
	arr_xt(const arr_xt& o):basetype(o),dims(o.dims){}


	arr_xt& operator=(const arr_xt& o)
	{
		impl=o.impl;
		dims=o.dims;
		return *this;
	}

	void swap(arr_xt& o)
	{
		impl.swap(o.impl);
		std::swap(dims,o.dims);
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

	void resize(size_type k0,size_type k1=1,size_type k2=1,size_type k3=1,size_type k4=1,size_type k5=1);
	void clear()
	{
		impl.clear();
		dims.reset();
	}

	inline T& operator()(size_type k0)
	{
		return impl[k0];
	}
	inline T& operator()(size_type k0,size_type k1)
	{
		return impl[k0+dims[0]*k1];
	}
	inline T& operator()(size_type k0,size_type k1,size_type k2)
	{
		return impl[k0+dims[0]*(k1+dims[1]*k2)];
	}
	inline T& operator()(size_type k0,size_type k1,size_type k2,size_type k3)
	{
		return impl[k0+dims[0]*(k1+dims[1]*(k2+dims[2]*k3))];
	}
	inline T& operator()(size_type k0,size_type k1,size_type k2,size_type k3,size_type k4)
	{
		return impl[k0+dims[0]*(k1+dims[1]*(k2+dims[2]*(k3+dims[3]*k4)))];
	}
	inline T& operator()(size_type k0,size_type k1,size_type k2,size_type k3,size_type k4,size_type k5)
	{
		return impl[k0+dims[0]*(k1+dims[1]*(k2+dims[2]*(k3+dims[3]*(k4+dims[4]*k5))))];
	}

	inline const T& operator()(size_type k0) const
	{
		return impl[k0];
	}
	inline const T& operator()(size_type k0,size_type k1) const
	{
		return impl[k0+dims[0]*k1];
	}
	inline const T& operator()(size_type k0,size_type k1,size_type k2) const
	{
		return impl[k0+dims[0]*(k1+dims[1]*k2)];
	}
	inline const T& operator()(size_type k0,size_type k1,size_type k2,size_type k3) const
	{
		return impl[k0+dims[0]*(k1+dims[1]*(k2+dims[2]*k3))];
	}
	inline const T& operator()(size_type k0,size_type k1,size_type k2,size_type k3,size_type k4) const
	{
		return impl[k0+dims[0]*(k1+dims[1]*(k2+dims[2]*(k3+dims[3]*k4)))];
	}
	inline const T& operator()(size_type k0,size_type k1,size_type k2,size_type k3,size_type k4,size_type k5) const
	{
		return impl[k0+dims[0]*(k1+dims[1]*(k2+dims[2]*(k3+dims[3]*(k4+dims[4]*k5))))];
	}

	inline size_type size() const
	{
		return impl.size();
	}
	inline size_type size(int n) const
	{
		return dims[n];
	}

	const size_type* size_ptr() const
	{
		return &dims[0];
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

protected:	
	using basetype::impl;
	arr_xt_dims dims;
};


template<typename T,typename A>
void arr_xt<T,A>::resize(size_type k0,size_type k1,size_type k2,size_type k3,size_type k4,size_type k5)
{

	size_t _newsize=k0*k1*k2*k3*k4*k5;
	if( (_newsize>0 && k0!=_newsize/k1/k2/k3/k4/k5)||_newsize>this->get_allocator().max_size())
	{
		Exception::XBadAlloc();
	}

	arr_xt tmp;
	tmp.impl.resize(_newsize);
	tmp.dims[0]=k0;
	tmp.dims[1]=k1;
	tmp.dims[2]=k2;
	tmp.dims[3]=k3;
	tmp.dims[4]=k4;
	tmp.dims[5]=k5;

	if(_newsize==0)
	{
		swap(tmp);
		return;
	}

	arr_xt& _Eold(*this);
	arr_xt& _Enew(tmp);

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


template<typename T,typename A> class hash_t<arr_xt<T,A> >
{
public:
	typedef arr_xt<T,A> type;
	uint32_t operator()(const arr_xt<T,A>& o)
	{
		return hash_array<T>::hash(o.data(),o.size())^hash_array<typename type::size_type>::hash(o.size_ptr(),type::MAX_DIM);
	}
};

VHWD_LEAVE
#endif
