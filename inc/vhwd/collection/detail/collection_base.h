// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_COLLECTION_BASE__
#define __H_VHWD_COLLECTION_BASE__


#include "vhwd/config.h"
#include "vhwd/memory.h"

#include <memory>

VHWD_ENTER


class sz_helper
{
public:

	// adjunst alignment
	static int adj(int sz,int al)
	{
		return (sz+al-1)&~(al-1);
	}

	// adjunst alignment
	static void* adj(void* sz,int al)
	{
		return (void*)(((ptrdiff_t)sz+al-1)&~(al-1));
	}

	// adjust size to 2^n
	static int n2p(int sz)
	{
		int k=2;
		while(k<sz) k=k<<1;
		return k;
	}
};


template<typename T,typename A=def_allocator >
class arr_xt;

template<typename T,typename A=def_allocator >
class arr_1t;

class container_base
{
public:

};

template<typename A,bool V>
class alloc_base : public container_base
{
public:
	typedef A allocator_type;

	alloc_base(){}
	alloc_base(const alloc_base&){}
	alloc_base(const allocator_type&){}

	allocator_type& get_allocator() const {return *(allocator_type*)(NULL);}
};

template<typename A>
class alloc_base<A,false> : public container_base
{
public:
	typedef A allocator_type;

	alloc_base(){}
	alloc_base(const alloc_base& o):_al(o._al){}
	alloc_base(const allocator_type& al):_al(al){}

	allocator_type& get_allocator() const {return _al;}

protected:
	mutable allocator_type _al;
};



template<typename K,bool V,typename A>
class keycomp_base : public alloc_base<A,tl::is_empty_type<A>::value>
{
public:
	typedef K key_compare;
	typedef alloc_base<A,tl::is_empty_type<A>::value> basetype;
	typedef typename basetype::allocator_type allocator_type;

	keycomp_base(){}
	keycomp_base(const keycomp_base& o):basetype(o){}
	keycomp_base(const key_compare&,const allocator_type& al):basetype(al){}


	key_compare& key_comp() const {return *(key_compare*)(NULL);}
};

template<typename K,typename A>
class keycomp_base<K,false,A> : public alloc_base<A,tl::is_empty_type<A>::value>
{
public:
	typedef K key_compare;
	typedef alloc_base<A,tl::is_empty_type<A>::value> basetype;
	typedef typename basetype::allocator_type allocator_type;

	keycomp_base(){}
	keycomp_base(const keycomp_base& o):basetype(o),_kc(o._kc){}
	keycomp_base(const key_compare& kc,const allocator_type& al):basetype(al),_kc(kc){}

	key_compare& key_comp() const {return _kc;}

protected:
	mutable key_compare _kc;
};

template<typename B>
class containerB : public container_base
{
protected:
	typedef B impl_type;
public:

	typedef typename impl_type::allocator_type allocator_type;

	typedef typename allocator_type::value_type value_type;
	typedef typename allocator_type::size_type size_type;
	typedef typename allocator_type::difference_type difference_type;

	typedef typename allocator_type::pointer pointer;
	typedef typename allocator_type::const_pointer const_pointer;

	typedef typename allocator_type::reference reference;
	typedef typename allocator_type::const_reference const_reference;

	typedef typename impl_type::iterator iterator;
	typedef typename impl_type::reverse_iterator reverse_iterator;
	typedef typename impl_type::const_iterator const_iterator;
	typedef typename impl_type::const_reverse_iterator const_reverse_iterator;

	containerB(){}

	template<typename T1,typename T2>
	containerB(const T1& kc,const T2& al):impl(kc,al){}

	containerB(const containerB& o):impl(o.impl){}

#ifdef VHWD_C11
	containerB(containerB&& o):impl(o.impl){}
#endif

	iterator begin(){return impl.begin();}
	iterator end(){return impl.end();}
	reverse_iterator rbegin(){return impl.rbegin();}
	reverse_iterator rend(){return impl.rend();}

	const_iterator begin() const {return impl.begin();}
	const_iterator end() const {return impl.end();}
	const_reverse_iterator rbegin() const {return impl.rbegin();}
	const_reverse_iterator rend() const {return impl.rend();}

	const_iterator cbegin() const {return impl.begin();}
	const_iterator cend() const {return impl.end();}
	const_reverse_iterator crbegin() const {return impl.rbegin();}
	const_reverse_iterator crend() const {return impl.rend();}

	bool empty() const {return impl.empty();}
	size_type size() const {return impl.size();}
	size_type max_size() const{return impl.get_allocator().max_size();}
	void clear(){impl.clear();}

	void swap(containerB& o){impl.swap(o.impl);}

	allocator_type& get_allocator(){return impl.get_allocator();}

protected:
	mutable impl_type impl;
};

template<typename A>
class containerA : public alloc_base<A,tl::is_empty_type<A>::value>
{
public:
	typedef alloc_base<A,tl::is_empty_type<A>::value> basetype;
	typedef A allocator_type;

	typedef typename allocator_type::value_type value_type;
	typedef typename allocator_type::size_type size_type;
	typedef typename allocator_type::difference_type difference_type;

	typedef typename allocator_type::pointer pointer;
	typedef typename allocator_type::const_pointer const_pointer;

	typedef typename allocator_type::reference reference;
	typedef typename allocator_type::const_reference const_reference;

	containerA(){}
	containerA(const containerA& o):basetype(o){}
	containerA(const allocator_type& al):basetype(al){}

};

template<typename K,typename A>
class containerK : public keycomp_base<K,tl::is_empty_type<K>::value,A>
{
public:

	typedef keycomp_base<K,tl::is_empty_type<K>::value,A> basetype;

	typedef A allocator_type;

	typedef typename allocator_type::value_type value_type;
	typedef typename allocator_type::size_type size_type;
	typedef typename allocator_type::difference_type difference_type;

	typedef typename allocator_type::pointer pointer;
	typedef typename allocator_type::const_pointer const_pointer;

	typedef typename allocator_type::reference reference;
	typedef typename allocator_type::const_reference const_reference;

	typedef typename basetype::key_compare key_compare;

	containerK(){}
	containerK(const containerK& o):basetype(o){}
	containerK(const key_compare& kc,const allocator_type& al):basetype(kc,al){}

};



template<typename T,bool D>
class IteratorDirection
{
public:
	static inline T* add(T* p,ptrdiff_t n){return p+n;}
	static inline T* sub(T* p,ptrdiff_t n){return p-n;}
	static inline ptrdiff_t sub(const T* p1,const T* p2){return p1-p2;}
};

template<typename T>
class IteratorDirection<T,false>
{
public:
	static inline T* add(T* p,ptrdiff_t n){return p-n;}
	static inline T* sub(T* p,ptrdiff_t n){return p+n;}
	static inline ptrdiff_t sub(const T* p1,const T* p2){return p2-p1;}
};

// D iterator direction
// C const_iterator?
template<typename T,bool D,bool C>
class rac_iterator;


template<typename T,bool D>
class rac_iterator<T,D,true>	: public std::iterator<std::random_access_iterator_tag,T>
{
public:
	typedef T* pointer;

	rac_iterator(const pointer x=pointer()) :_pAddress(x) {}
	rac_iterator& operator++() {_pAddress=IteratorDirection<T,D>::add(_pAddress,1);return *this;}
	rac_iterator operator++(int) {rac_iterator tmp(*this); _pAddress=IteratorDirection<T,D>::add(_pAddress,1); return tmp;}
	rac_iterator& operator--() {_pAddress=IteratorDirection<T,D>::sub(_pAddress,1);return *this;}
	rac_iterator operator--(int) {rac_iterator tmp(*this); _pAddress=IteratorDirection<T,D>::sub(_pAddress,1); return tmp;}

	bool operator==(const rac_iterator& rhs) {return _pAddress==rhs._pAddress;}
	bool operator!=(const rac_iterator& rhs) {return _pAddress!=rhs._pAddress;}
	bool operator<(const rac_iterator& rhs) {return IteratorDirection<T,D>::sub(_pAddress,rhs._pAddress)<0;}
	bool operator<=(const rac_iterator& rhs) {return IteratorDirection<T,D>::sub(_pAddress,rhs._pAddress)<=0;}
	bool operator>(const rac_iterator& rhs) {return IteratorDirection<T,D>::sub(_pAddress,rhs._pAddress)>0;}
	bool operator>=(const rac_iterator& rhs) {return IteratorDirection<T,D>::sub(_pAddress,rhs._pAddress)>=0;}

	const T& operator*(){return *_pAddress;}
	T* get(){return _pAddress;}
	const T* operator->(){return _pAddress;}

protected:
	pointer _pAddress;
};

template<typename T,bool D>
class rac_iterator<T,D,false> : public rac_iterator<T,D,true>
{
protected:
	using rac_iterator<T,D,true>::_pAddress;
public:
	typedef T* pointer;

	rac_iterator(pointer x=pointer()):rac_iterator<T,D,true>(x){}

	rac_iterator& operator++() {_pAddress=IteratorDirection<T,D>::add(_pAddress,1);return *this;}
	rac_iterator operator++(int) {rac_iterator tmp(*this); _pAddress=IteratorDirection<T,D>::add(_pAddress,1); return tmp;}
	rac_iterator& operator--() {_pAddress=IteratorDirection<T,D>::sub(_pAddress,1);return *this;}
	rac_iterator operator--(int) {rac_iterator tmp(*this); _pAddress=IteratorDirection<T,D>::sub(_pAddress,1); return tmp;}

	T& operator*(){return *_pAddress;}
	T* get(){return _pAddress;}
	T* operator->(){return _pAddress;}
};


template<typename T,bool D,bool C>
rac_iterator<T,D,C> operator+(rac_iterator<T,D,C> it1,ptrdiff_t n)
{
	return IteratorDirection<T,D>::add(it1.get(),n);
}

template<typename T,bool C,bool D>
rac_iterator<T,D,C> operator-(rac_iterator<T,D,C> it1,ptrdiff_t n)
{
	return IteratorDirection<T,D>::sub(it1.get(),n);
}

template<typename T,bool D,bool C1,bool C2>
ptrdiff_t operator-(rac_iterator<T,D,C1> it1,rac_iterator<T,D,C2> it2)
{
	return IteratorDirection<T,D>::sub(it1.get(),it2.get());
}


template<typename A>
class containerS : public containerA<A>
{
public:
    typedef containerA<A> basetype;
    typedef typename basetype::allocator_type allocator_type;
    typedef typename basetype::value_type value_type;
	typedef rac_iterator<value_type,true,false> iterator;
	typedef rac_iterator<value_type,true,true> const_iterator;

	typedef rac_iterator<value_type,false,false> reverse_iterator;
	typedef rac_iterator<value_type,false,true> const_reverse_iterator;

	typedef typename containerA<A>::size_type size_type;

	size_type max_size() const
	{
		return this->get_allocator().max_size();
	}

	containerS(){}
	containerS(const containerS& o):basetype(o){}
	containerS(const allocator_type& al):basetype(al){}

};


template<typename T,bool POD>
class xmem_helper_type;

template<typename T>
class xmem_helper_type<T,true>
{
public:

	static void destroy(T* first_,size_t count_)
	{
		(void)&first_;
		(void)&count_;
	}

	template<typename It>
	static void destroy(It first_,It last_)
	{
		(void)&first_;
		(void)&last_;
	}


};

template<typename T>
class xmem_helper_type<T,false>
{
public:
	static void destroy(T* first_,size_t count_)
	{
		for(size_t i=0;i<count_;i++)
		{
			first_[i].~T();
		}
	}

	template<typename It>
	static void destroy(It first_,It last_)
	{
		for(;first_<last_;first_++)
		{
			(*first_).~T();
		}
	}

};


template<typename T>
class xmem_helper : public xmem_helper_type<T,tl::is_pod<T>::value>
{
public:

	template<typename It>
	static void uninitialized_fill(It first_,It last_,const T& val_)
	{
		std::uninitialized_fill(first_,last_,val_);
	}

	template<typename It>
	static void uninitialized_fill_n(It first_,size_t count_,const T& val_)
	{
		std::uninitialized_fill_n(first_,count_,val_);
	}

	template<typename It>
	static void fill(It first_,It last_,const T& val_)
	{
		std::fill(first_,last_,val_);
	}

	template<typename It>
	static void fill_n(It first_,size_t count_,const T& val_)
	{
		std::fill_n(first_,count_,val_);
	}

	template<typename It,typename Ot>
	static Ot uninitialized_copy(It first_,It last_,Ot dest_)
	{
		return std::uninitialized_copy(first_,last_,dest_);
	}

	template<typename It,typename Ot>
	static Ot uninitialized_copy_n(It first_,size_t count_,Ot dest_)
	{
		//std::uninitialized_copy_n(first_,count_,dest_);		//C++11
		return std::uninitialized_copy(first_,first_+count_,dest_);
	}

	template<typename It,typename Ot>
	static Ot copy(It first_,It last_,Ot dest_)
	{
		return std::copy(first_,last_,dest_);
	}


	template<typename It,typename Ot>
	static Ot copy_n(It first_,size_t count_,Ot dest_)
	{
		//std::copy_n(first_,count_,dest_);
		return std::copy(first_,first_+count_,dest_);
	}

	template<typename It,typename Ot>
	static Ot copy_backward(It first_,It last_,Ot dest_)
	{
		return std::copy_backward(first_,last_,dest_);
	}
};


template<typename T>
class xmem : public xmem_helper<T>
{
public:

};


struct extra_vec_1t
{
	typedef size_t size_type;

	size_type capacity;
	size_type size;

	extra_vec_1t()
	{
		capacity=0;
		size=0;
	}
};


class extra_arr_xt
{
public:
	static const size_t MAX_DIM=6;
	typedef size_t size_type;
	size_type capacity;
	size_type size;
	size_type dims[6];
	extra_arr_xt(){capacity=size=0;dims[0]=0;dims[1]=dims[2]=dims[3]=dims[4]=dims[5]=1;}
};

template<typename T,typename A,typename E>
class arr_base : public containerS< typename AllocatorE<A,E>::template rebind<T>::other >
{
	arr_base(const arr_base&);
	const arr_base& operator=(const arr_base&);
public:

	typedef containerS< typename AllocatorE<A,E>::template rebind<T>::other > basetype;
	typedef typename basetype::iterator iterator;
	typedef typename basetype::const_iterator const_iterator;
	typedef typename basetype::size_type size_type;
	typedef typename basetype::allocator_type allocator_type;


	arr_base():m_ptr(NULL){}
	arr_base(const A& al):basetype(allocator_type(al)),m_ptr(NULL){}
	arr_base(const allocator_type& al):basetype(al),m_ptr(NULL){}

	~arr_base()
	{
		if(m_ptr!=NULL)
		{
			this->get_allocator().deallocate(m_ptr,extra().capacity);
		}
	}

	void _xgen(size_type count_)
	{
		if(count_>7)
		{
			count_=(count_+(count_>>2)+7)&~7;
		}

		_xset(count_);
	}

	void _xset(size_type count_)
	{
		if(m_ptr!=NULL)
		{
			this->get_allocator().deallocate(m_ptr,extra().capacity);
			m_ptr=NULL;
		}

		if(count_>0)
		{
			m_ptr=this->get_allocator().allocate(count_);
			extra().capacity=count_;
		}

	}

	E& extra(){return allocator_type::extra(m_ptr);}
	const E& extra() const {return allocator_type::extra(m_ptr);}

	void swap(arr_base& o){std::swap(m_ptr,o.m_ptr);}

	T& operator[](size_type n){return m_ptr[n];}
	const T& operator[](size_type n) const {return m_ptr[n];}

protected:
	T* m_ptr;

};


VHWD_LEAVE


#endif
