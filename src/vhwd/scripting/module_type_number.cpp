#include "module.h"

VHWD_ENTER

template<>
CallableMetatableTypeT<Variant::V_DOUBLE>::CallableMetatableTypeT()
{
	
}

template<>
int CallableMetatableTypeT<Variant::V_DOUBLE>::__fun_call(Executor& ks)
{
	typedef double type;

	int nbp=ks.stack.vbp.back();
	int pmc=ks.stack.nsp-nbp;
	if(pmc==0)
	{
		ks.stack[nbp+1].reset<type>();
	}
	else if(pmc==1)
	{
		ks.stack[nbp+1].reset<type>(PLCast<type>::g(ks.stack[nbp+1]));
	}
	else
	{
		arr_xt<type> xt;
		xt.resize(pmc);
		for(int i=0;i<pmc;i++)
		{
			xt(i)=PLCast<type>::g(ks.stack[nbp+i+1]);
		}

		ks.stack[nbp+1].reset<arr_xt<type> >(xt);
	}
	return 1;
}

template<>
void CallableMetatableTypeT<Variant::V_DOUBLE>::init()
{
	kvar_table& ggvar(Executor::ggvar);
	gVariantMetaTable[Variant::V_DOUBLE].reset(new CallableMetatableTypeT<Variant::V_DOUBLE>);
	ggvar["type"].ref<kvar_table>()["double"].kptr(gVariantMetaTable[Variant::V_DOUBLE]);

}


template<>
CallableMetatableTypeT<Variant::V_INT>::CallableMetatableTypeT()
{
	
}

template<>
int CallableMetatableTypeT<Variant::V_INT>::__fun_call(Executor& ks)
{
	typedef int64_t type;

	int nbp=ks.stack.vbp.back();
	int pmc=ks.stack.nsp-nbp;
	if(pmc==0)
	{
		ks.stack[nbp+1].reset<type>();
	}
	else if(pmc==1)
	{
		ks.stack[nbp+1].reset<type>(PLCast<type>::g(ks.stack[nbp+1]));
	}
	else
	{
		arr_xt<type> xt;
		xt.resize(pmc);
		for(int i=0;i<pmc;i++)
		{
			xt(i)=PLCast<type>::g(ks.stack[nbp+i+1]);
		}

		ks.stack[nbp+1].reset<arr_xt<type> >(xt);
	}
	return 1;
}



template<>
void CallableMetatableTypeT<Variant::V_INT>::init()
{
	kvar_table& ggvar(Executor::ggvar);
	gVariantMetaTable[Variant::V_INT].reset(new CallableMetatableTypeT<Variant::V_INT>);
	ggvar["type"].ref<kvar_table>()["integer"].kptr(gVariantMetaTable[Variant::V_INT]);
}


template<>
CallableMetatableTypeT<Variant::V_COMPLEX>::CallableMetatableTypeT()
{
	
}

template<>
int CallableMetatableTypeT<Variant::V_COMPLEX>::__fun_call(Executor& ks)
{
	int nbp=ks.stack.vbp.back();
	int nsp=ks.stack.nsp;
	int pmc=nsp-nbp;
	if(pmc>2)
	{
		return -1;
	}

	std::complex<double> v;
	if(pmc>0)
	{
		v.real(PLCast<double>::g(ks.stack[nbp+1]));
	}
	if(pmc>1)
	{
		v.imag(PLCast<double>::g(ks.stack[nbp+2]));
	}	
	
	ks.stack[nbp+1].reset(v);
	return 1;
}



template<>
void CallableMetatableTypeT<Variant::V_COMPLEX>::init()
{
	kvar_table& ggvar(Executor::ggvar);
	gVariantMetaTable[Variant::V_COMPLEX].reset(new CallableMetatableTypeT<Variant::V_COMPLEX>);
	ggvar["type"].ref<kvar_table>()["complex"].kptr(gVariantMetaTable[Variant::V_COMPLEX]);
}



template<>
CallableMetatableTypeT<Variant::V_BOOL>::CallableMetatableTypeT()
{
	
}

template<>
int CallableMetatableTypeT<Variant::V_BOOL>::__fun_call(Executor& ks)
{
	int nbp=ks.stack.vbp.back();
	ks.stack[nbp+1].reset<bool>();
	return 1;
}



template<>
void CallableMetatableTypeT<Variant::V_BOOL>::init()
{
	kvar_table& ggvar(Executor::ggvar);
	CallableMetatable* p=new CallableMetatableTypeT<Variant::V_BOOL>();
	gVariantMetaTable[Variant::V_BOOL].reset(p);
	ggvar["type"].ref<kvar_table>()["boolean"].kptr(p);
}


VHWD_LEAVE
