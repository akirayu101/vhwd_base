#ifndef __H_COLLECTION_ARRAY_IDX__
#define __H_COLLECTION_ARRAY_IDX__

#include "vhwd/collection/arr_xt.h"

VHWD_ENTER


class arridx
{
public:

	typedef size_t size_type;

	enum
	{
		IDX_NONE,
		IDX_ALL,
		IDX_SLICE,
		IDX_ARRAY,
		IDX_PTR,
	};

	arridx(){type=IDX_NONE;}

	void reset(size_type s)
	{
		type=IDX_ALL;
		size=s;
		imin=0;
		imax=size-1;
	}

	void slice(size_type s1,size_type sw,size_type s2)
	{
		if(sw==0)
		{
			Exception::XError("slice_step_is_zero",false);;
		}
		type=IDX_SLICE;
		idxs.resize(2);
		idxs(0)=s1;
		idxs(1)=sw;
		size=1+(s2-s1)/sw;
		if(sw>0)
		{
			imin=s1;
			imax=s1+(size-1)*sw;
		}
		else
		{
			imax=s1;
			imin=s1+(size-1)*sw;
		}
	}

	void arrptr(int64_t *p,size_type s)
	{
		type=IDX_PTR;
		size=s;
		xptr=p;
		imin=imax=xptr[0];
		for(size_type i=1; i<size; i++)
		{
			if(xptr[i]>imax)
			{
				imax=xptr[i];
			}
			if(xptr[i]<imin)
			{
				imin=xptr[i];
			}
		}
	}


	void arrobj(arr_xt<int64_t> &idx)
	{
		type=IDX_ARRAY;
		size=idx.size();
		idxs.swap(idx);
		xptr=idxs.data();
		imin=imax=xptr[0];
		for(size_type i=1; i<size; i++)
		{
			if(xptr[i]>imax)
			{
				imax=xptr[i];
			}
			if(xptr[i]<imin)
			{
				imin=xptr[i];
			}
		}
	}

	bool test(int v)
	{
		return imax<v && imin>=0;
	}

	size_type operator[](size_type i)
	{
		switch(type)
		{
		case IDX_ALL:
			return i;
		case IDX_SLICE:
			return idxs(0)+i*idxs(1);
		case IDX_ARRAY:
			return idxs(i);
		case IDX_PTR:
			return xptr[i];
		default:
			throw -1;
		};
	}
	size_type operator()(size_type i)
	{
		switch(type)
		{
		case IDX_ALL:
			return i;
		case IDX_SLICE:
			return idxs(0)+i*idxs(1);
		case IDX_ARRAY:
			return idxs(i);
		case IDX_PTR:
			return xptr[i];
		default:
			throw -1;
		};
	}

	int type;
	int imin;
	int imax;
	size_type size;
	arr_xt<int64_t> idxs;
	int64_t *xptr;
};


template<typename T>
class sub_xt
{
	const sub_xt& operator=(const sub_xt&);
	sub_xt(const sub_xt&);
public:
	typedef arr_xt<T> arrtype;
	typedef typename arrtype::size_type size_type;

	sub_xt(arrtype &a):arr(a)
	{
		for(size_t i=0; i<arrtype::MAX_DIM; i++)
		{
			idx[i].reset(arr.size(i));
		}
	}


	T &operator()(size_type s0)
	{
		return arr(idx[0](s0));
	}

	T &operator()(size_type s0,size_type s1)
	{
		return arr(idx[0](s0),idx[1](s1));
	}

	T &operator()(size_type s0,size_type s1,size_type s2)
	{
		return arr(idx[0](s0),idx[1](s1),idx[2](s2));
	}

	T &operator()(size_type s0,size_type s1,size_type s2,size_type s3)
	{
		return arr(idx[0](s0),idx[1](s1),idx[2](s2),idx[3](s3));
	}

	T &operator()(size_type s0,size_type s1,size_type s2,size_type s3,size_type s4)
	{
		return arr(idx[0](s0),idx[1](s1),idx[2](s2),idx[3](s3),idx[4](s4));
	}

	T &operator()(size_type s0,size_type s1,size_type s2,size_type s3,size_type s4,size_type s5)
	{
		return arr(idx[0](s0),idx[1](s1),idx[2](s2),idx[3](s3),idx[4](s4),idx[5](s5));
	}

	const T& operator()(size_type s0) const
	{
		return arr(idx[0](s0));
	}

	const T& operator()(size_type s0,size_type s1) const
	{
		return arr(idx[0](s0),idx[1](s1));
	}

	const T& operator()(size_type s0,size_type s1,size_type s2) const
	{
		return arr(idx[0](s0),idx[1](s1),idx[2](s2));
	}

	const T& operator()(size_type s0,size_type s1,size_type s2,size_type s3) const
	{
		return arr(idx[0](s0),idx[1](s1),idx[2](s2),idx[3](s3));
	}

	const T& operator()(size_type s0,size_type s1,size_type s2,size_type s3,size_type s4) const
	{
		return arr(idx[0](s0),idx[1](s1),idx[2](s2),idx[3](s3),idx[4](s4));
	}

	const T& operator()(size_type s0,size_type s1,size_type s2,size_type s3,size_type s4,size_type s5) const
	{
		return arr(idx[0](s0),idx[1](s1),idx[2](s2),idx[3](s3),idx[4](s4),idx[5](s5));
	}

	arridx idx[arrtype::MAX_DIM];
	arrtype &arr;

	bool test_index(int pmc)
	{
		if(pmc>=(int)arrtype::MAX_DIM)
		{
			return false;
		}
		size_type dims[arrtype::MAX_DIM];
		for(int i=0; i<pmc; i++)
		{
			dims[i]=arr.size(i);
		}
		for(int i=0; i<pmc; i++)
		{
			if(!idx[i].test(dims[i]))
			{
				return false;
			}
		}
		return true;
	}

	void enlarge(int pmc)
	{
		bool flag=false;
		size_type dims[arrtype::MAX_DIM];
		if(arr.size()==0)
		{
			dims[0]=0;
			for(size_t i=1; i<arrtype::MAX_DIM; i++)
			{
				dims[i]=1;
			}
		}
		else
		{
			for(size_t i=0; i<arrtype::MAX_DIM; i++)
			{
				dims[i]=arr.size(i);
			}
		}
		for(int i=0; i<pmc; i++)
		{
			if(idx[i].imin<0)
			{
				Exception::XInvalidIndex();
			}
			size_type m=idx[i].imax+1;
			if(dims[i]<m)
			{
				flag=true;
				dims[i]=m;
			}
		}
		if(flag)
		{
			arr.resize(dims[0],dims[1],dims[2],dims[3],dims[4],dims[5]);
		}
	}
};

VHWD_LEAVE
#endif
