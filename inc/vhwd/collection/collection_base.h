#ifndef __H_VHWD_COLLECTION_BASE__
#define __H_VHWD_COLLECTION_BASE__


#include "vhwd/config.h"
#include "vhwd/memory/allocator.h"


VHWD_ENTER

template<typename T,typename A=std::allocator<T> >
class arr_xt;

template<typename T,typename A=std::allocator<T> >
class arr_1t;


class container_base
{
public:

};

template<typename A,bool V>
class alloc_base : public container_base
{
public:
	typedef A allocater_type;
	typedef A _Alloc_return;

	_Alloc_return get_allocator() const {return allocater_type();}
};

template<typename A>
class alloc_base<A,false> : public container_base
{
public:
	typedef A allocater_type;
	typedef A& _Alloc_return;
	_Alloc_return get_allocator() const {return _al;}

protected:
	mutable allocater_type _al;
};


template<typename A>
class container : public alloc_base<A,tl::is_empty_type<A>::value>
{
public:

	typedef A allocater_type;

	typedef typename allocater_type::value_type value_type;
	typedef typename allocater_type::size_type size_type;
	typedef typename allocater_type::difference_type difference_type;

	typedef typename allocater_type::pointer pointer;
	typedef typename allocater_type::const_pointer const_pointer;

	typedef typename allocater_type::reference reference;
	typedef typename allocater_type::const_reference const_reference;

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


template<typename T,bool D,bool C>
class RandomAccessIterator;


template<typename T,bool D>
class RandomAccessIterator<T,D,true>	: public std::iterator<std::random_access_iterator_tag,T>
{
public:
	typedef T* pointer;

	RandomAccessIterator(const pointer x=pointer()) :_pAddress(x) {}
	RandomAccessIterator& operator++() {_pAddress=IteratorDirection<T,D>::add(_pAddress,1);return *this;}
	RandomAccessIterator operator++(int) {RandomAccessIterator tmp(*this); _pAddress=IteratorDirection<T,D>::add(_pAddress,1); return tmp;}
	RandomAccessIterator& operator--() {_pAddress=IteratorDirection<T,D>::sub(_pAddress,1);return *this;}
	RandomAccessIterator operator--(int) {RandomAccessIterator tmp(*this); _pAddress=IteratorDirection<T,D>::sub(_pAddress,1); return tmp;}

	bool operator==(const RandomAccessIterator& rhs) {return _pAddress==rhs._pAddress;}
	bool operator!=(const RandomAccessIterator& rhs) {return _pAddress!=rhs._pAddress;}
	bool operator<(const RandomAccessIterator& rhs) {return IteratorDirection<T,D>::sub(_pAddress,rhs._pAddress)<0;}
	bool operator<=(const RandomAccessIterator& rhs) {return IteratorDirection<T,D>::sub(_pAddress,rhs._pAddress)<=0;}
	bool operator>(const RandomAccessIterator& rhs) {return IteratorDirection<T,D>::sub(_pAddress,rhs._pAddress)>0;}
	bool operator>=(const RandomAccessIterator& rhs) {return IteratorDirection<T,D>::sub(_pAddress,rhs._pAddress)>=0;}

	const T& operator*(){return *_pAddress;}
	T* get(){return _pAddress;}
	const T* operator->(){return _pAddress;}

protected:
	pointer _pAddress;
};

template<typename T,bool D>
class RandomAccessIterator<T,D,false> : public RandomAccessIterator<T,D,true>
{
protected:
	using RandomAccessIterator<T,D,true>::_pAddress;
public:
	typedef T* pointer;

	RandomAccessIterator(pointer x=pointer()):RandomAccessIterator<T,D,true>(x){}

	RandomAccessIterator& operator++() {_pAddress=IteratorDirection<T,D>::add(_pAddress,1);return *this;}
	RandomAccessIterator operator++(int) {RandomAccessIterator tmp(*this); _pAddress=IteratorDirection<T,D>::add(_pAddress,1); return tmp;}
	RandomAccessIterator& operator--() {_pAddress=IteratorDirection<T,D>::sub(_pAddress,1);return *this;}
	RandomAccessIterator operator--(int) {RandomAccessIterator tmp(*this); _pAddress=IteratorDirection<T,D>::sub(_pAddress,1); return tmp;}

	T& operator*(){return *_pAddress;}
	T* get(){return _pAddress;}
	T* operator->(){return _pAddress;}
};


template<typename T,bool D,bool C>
RandomAccessIterator<T,D,C> operator+(RandomAccessIterator<T,D,C> it1,ptrdiff_t n)
{
	return IteratorDirection<T,D>::add(it1.get(),n);
}

template<typename T,bool C,bool D>
RandomAccessIterator<T,D,C> operator-(RandomAccessIterator<T,D,C> it1,ptrdiff_t n)
{
	return IteratorDirection<T,D>::sub(it1.get(),n);
}

template<typename T,bool D,bool C1,bool C2>
ptrdiff_t operator-(RandomAccessIterator<T,D,C1> it1,RandomAccessIterator<T,D,C2> it2)
{
	return IteratorDirection<T,D>::sub(it1.get(),it2.get());
}


template<typename A>
class container_seq : public container<A>
{
public:
    typedef container<A> basetype;
    typedef typename basetype::value_type value_type;
	typedef RandomAccessIterator<value_type,true,false> iterator;
	typedef RandomAccessIterator<value_type,true,true> const_iterator;

	typedef RandomAccessIterator<value_type,false,false> reverse_iterator;
	typedef RandomAccessIterator<value_type,false,true> const_reverse_iterator;

	typedef typename container<A>::size_type size_type;

	size_type max_size() const
	{
		return this->get_allocator().max_size();
	}

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
	static void uninitialized_copy(It first_,It last_,Ot dest_)
	{
		std::uninitialized_copy(first_,last_,dest_);
	}

	template<typename It,typename Ot>
	static void uninitialized_copy_n(It first_,size_t count_,Ot dest_)
	{
		//std::uninitialized_copy_n(first_,count_,dest_);		//C++11
		std::uninitialized_copy(first_,first_+count_,dest_);
	}

	template<typename It,typename Ot>
	static void copy(It first_,It last_,Ot dest_)
	{
		std::copy(first_,last_,dest_);
	}

	template<typename It,typename Ot>
	static void copy_n(It first_,size_t count_,Ot dest_)
	{
		//std::copy_n(first_,count_,dest_);
		std::copy(first_,first_+count_,dest_);
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
class arr_base : public container_seq< AllocatorExtra<T,E,A> >
{
	arr_base(const arr_base&);
	const arr_base& operator=(const arr_base&);
public:

	typedef typename container_seq<A>::iterator iterator;
	typedef typename container_seq<A>::const_iterator const_iterator;
	typedef typename container_seq<A>::size_type size_type;

	arr_base():m_ptr(NULL){}

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

	E& extra(){return AllocatorExtra<T,E,A>::extra(m_ptr);}
	const E& extra() const {return AllocatorExtra<T,E,A>::extra(m_ptr);}

	void swap(arr_base& o){std::swap(m_ptr,o.m_ptr);}

	T& operator[](size_type n){return m_ptr[n];}
	const T& operator[](size_type n) const {return m_ptr[n];}

protected:
	T* m_ptr;

};


VHWD_LEAVE


#endif
