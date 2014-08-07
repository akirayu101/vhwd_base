#include "vhwd/scripting/variant.h"
#include "vhwd/scripting/variant_op1.h"
#include "vhwd/scripting/variant_table.h"
#include "vhwd/scripting/executor.h"
#include "vhwd/scripting/sub_xt.h"


VHWD_ENTER


template<typename T>
class CallableIterator_Array1 : public CallableIterator
{
public:
	typedef arr_xt<T> arr_type;
	typedef typename arr_type::iterator iterator;

	CallableIterator_Array1(arr_type& t){itcur=t.begin();itend=t.end();}

	iterator itcur,itend;

	virtual int __fun_call(Executor& ks)
	{
		int nbp=ks.stack.vbp.back();
		if(itcur==itend)
		{
			ks.stack[nbp+1].reset<bool>(false);
			return 1;
		}
		else
		{
			ks.stack[nbp+1].reset<T>(*itcur);
			ks.stack[nbp+2].reset<bool>(true);
			itcur++;
			return 2;
		}
	}
};

template<typename T>
CallableIterator* CallableValueT<arr_xt<T> >::GetIterator(int n)
{
	if(n!=1) return new CallableIterator;
	return new CallableIterator_Array1<T>(value);
}




template<typename T>
int CallableValueT<arr_xt<T> >::__getindex(Executor& ks,const String& s)
{
	//int nbp=ks.stack.vbp.back();
	//int nsp=ks.stack.nsp;
	//int pmc=nsp-nbp;



	return -1;
}

template<typename T>
int CallableValueT<arr_xt<T> >::__setindex(Executor& ks,const String& s)
{
	return -1;
}

void handle_subarray_index(Executor& ks,arridx& idx,Variant& v,size_t n)
{
	arr_xt<int64_t> a=PLCast<arr_xt<int64_t> >::g(v);
	idx.arrobj(a);
}

template<typename T>
int CallableValueT<arr_xt<T> >::__getarray(Executor& ks)
{
	int nbp=ks.stack.vbp.back();
	int nsp=ks.stack.nsp;
	int pmc=nsp-nbp;

	if(pmc>6)
	{
		ks.kerror("too many dimensions");
	}

	size_t dm[6];

	sub_xt<T> tmp(value);
	for(int i=0;i<pmc;i++)
	{
		handle_subarray_index(ks,tmp.idx[i],ks.stack[nbp+i+1],value.size(i));
	}

	if(!tmp.test_index(pmc))
	{
		Exception::XError("invalid array index");
	}

	for(int i=0;i<6;i++)
	{
		dm[i]=tmp.idx[i].size;
	}



	arr_xt<T> sxt;
	sxt.resize(dm[0],dm[1],dm[2],dm[3],dm[4],dm[5]);


	if(sxt.size()==1)
	{
		ks.stack[nbp+1].reset<T>(tmp(0,0,0,0,0,0));
	}
	else
	{

		for(size_t k5=0;k5<dm[5];k5++)
		for(size_t k4=0;k4<dm[4];k4++)
		for(size_t k3=0;k3<dm[3];k3++)
		for(size_t k2=0;k2<dm[2];k2++)
		for(size_t k1=0;k1<dm[1];k1++)
		for(size_t k0=0;k0<dm[0];k0++)
		{
			sxt(k0,k1,k2,k3,k4,k5)=tmp(k0,k1,k2,k3,k4,k5);
		}
		ks.stack[nbp+1].reset<arr_xt<T> >(sxt);
	}

	return 1;
}

template<typename T>
int CallableValueT<arr_xt<T> >::__setarray(Executor& ks)
{
	int nbp=ks.stack.vbp.back();
	int nsp=ks.stack.nsp;
	int pmc=nsp-nbp;

	if(pmc>6)
	{
		ks.kerror("too many dimensions");
	}

	size_t dm[6];

	sub_xt<T> tmp(value);
	for(int i=0;i<pmc;i++)
	{
		handle_subarray_index(ks,tmp.idx[i],ks.stack[nbp+i+1],value.size(i));
	}

	tmp.enlarge(pmc);
	int num=1;

	for(int i=0;i<6;i++)
	{
		dm[i]=tmp.idx[i].size;
		num*=tmp.idx[i].size;
	}



	if(num==1)
	{
		tmp(0,0,0,0,0,0)=PLCast<T>::g(ks.stack[nbp]);
		return 0;
	}

	arr_xt<T> sxt=PLCast<arr_xt<T> >::g(ks.stack[nbp]);

	if(sxt.size()==1)
	{
		T val(sxt(0));
		for(size_t k5=0;k5<dm[5];k5++)
		for(size_t k4=0;k4<dm[4];k4++)
		for(size_t k3=0;k3<dm[3];k3++)
		for(size_t k2=0;k2<dm[2];k2++)
		for(size_t k1=0;k1<dm[1];k1++)
		for(size_t k0=0;k0<dm[0];k0++)
		{
			tmp(k0,k1,k2,k3,k4,k5)=val;
		}
	}
	else
	{
		if(
			dm[0]!=sxt.size(0)||dm[1]!=sxt.size(1)||dm[2]!=sxt.size(2)||
			dm[3]!=sxt.size(3)||dm[4]!=sxt.size(4)||dm[5]!=sxt.size(5)
			)
		{
			Exception::XInvalidIndex();
		}

		for(size_t k5=0;k5<dm[5];k5++)
		for(size_t k4=0;k4<dm[4];k4++)
		for(size_t k3=0;k3<dm[3];k3++)
		for(size_t k2=0;k2<dm[2];k2++)
		for(size_t k1=0;k1<dm[1];k1++)
		for(size_t k0=0;k0<dm[0];k0++)
		{
			tmp(k0,k1,k2,k3,k4,k5)=sxt(k0,k1,k2,k3,k4,k5);
		}
	}




	return 0;
}


template class CallableValueT<arr_xt<kvar_type<0>::elemtype> >;
template class CallableValueT<arr_xt<kvar_type<1>::elemtype> >;
template class CallableValueT<arr_xt<kvar_type<2>::elemtype> >;
template class CallableValueT<arr_xt<kvar_type<3>::elemtype> >;
template class CallableValueT<arr_xt<kvar_type<4>::elemtype> >;
template class CallableValueT<arr_xt<kvar_type<5>::elemtype> >;
template class CallableValueT<arr_xt<kvar_type<6>::elemtype> >;
template class CallableValueT<arr_xt<kvar_type<7>::elemtype> >;


VHWD_LEAVE
