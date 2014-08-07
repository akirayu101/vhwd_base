#include "module.h"

VHWD_ENTER

template<typename P>
class kvar_func_array
{
public:

	typedef void type;
	static type invalid_type()
	{
		Exception::XBadCast();
	}

	template<typename T>
	static void k(Executor& ks,T& v)
	{
		P::g(ks,v);
	}

	template<typename T>
	static void k(Executor& ks,arr_xt<T>& v)
	{
		P::k(ks,v);	
	}

	template<int N>
	class rebind
	{
	public:
		static void g(Executor& ks)
		{
			k(ks,ks.top().ref<typename kvar_type<N>::type>());
		}
	};
};

class CallableFunctionArrayTrans : public CallableFunction
{
public:


	template<typename T>
	static void g(Executor& ks,T& v)
	{

	}


	template<typename T>
	static void k(Executor& ks,arr_xt<T>& v)
	{
		size_t s1=v.size(0);size_t s2=v.size(1);
		arr_xt<T> t;t.resize(s2,s1);
		for(size_t i=0;i<s1;i++) for(size_t j=0;j<s2;j++)
		{
			t(j,i)=v(i,j);
		}
		t.swap(v);	
	}

	CallableFunctionArrayTrans(){m_sName="trans";}

	int __fun_call(Executor& ks)
	{	
		int nbp=ks.stack.vbp.back();
		int pmc=ks.stack.nsp-nbp;
		if(pmc!=1)
		{
			return -1;
		}
		kvar_handler1<kvar_func_array<CallableFunctionArrayTrans> >::g1(ks,kvar_helper_base::f1(ks));
		return 1;
	};

};



class CallableFunctionSize : public CallableFunction
{
public:


	template<typename T>
	static void g(Executor& ks,T& v)
	{
		ks.top().reset<int64_t>(1);
	}


	template<typename T>
	static void k(Executor& ks,arr_xt<T>& v)
	{		
		arr_xt<int64_t> t;
		t.resize(6);
		for(size_t i=0;i<6;i++) t(i)=v.size(i);
		ks.top().reset(t);		
	}

	CallableFunctionSize(){m_sName="size";}

	int __fun_call(Executor& ks)
	{	
		if(!CheckParamCount(ks,1))
		{
			return -1;
		}

		kvar_handler1<kvar_func_array<CallableFunctionSize> >::g1(ks,kvar_helper_base::f1(ks));
		return 1;
	};

};

class CallableFunctionLength : public CallableFunctionSize
{
public:

	template<typename T>
	static void k(Executor& ks,arr_xt<T>& v)
	{		
		ks.top().reset<int64_t>(v.size());
	}

	CallableFunctionLength(){m_sName="length";}

	int __fun_call(Executor& ks)
	{	
		if(!CheckParamCount(ks,1))
		{
			return -1;
		}

		kvar_handler1<kvar_func_array<CallableFunctionLength> >::g1(ks,kvar_helper_base::f1(ks));
		return 1;
	};

};


class CallableFunctionLinspace : public CallableFunction
{
public:
	CallableFunctionLinspace(){m_sName="linspace";}

	int __fun_call(Executor& ks)
	{	
		int nbp=ks.stack.vbp.back();
		int pmc=ks.stack.nsp-nbp;
		double v1,v2;
		int64_t vn;

		if(pmc<2||pmc>3)
		{
			return -1;
		}
		
		v1=PLCast<double>::g(ks.stack[nbp+1]);
		v2=PLCast<double>::g(ks.stack[nbp+2]);
		vn=2;				
			
		if(pmc==3)
		{
			vn=PLCast<int64_t>::g(ks.stack[nbp+3]);
		}

		arr_xt<double> aa;

		aa.resize(vn);
		if(vn>1)
		{
			vn=vn-1;
			for(int64_t i=0;i<=vn;i++)
			{
				aa(i)=v1*double(vn-i)/double(vn) + v2* double(i)/double(vn);
			}
		}
		else if(vn==1)
		{
			aa(0)=0.5*(v1+v2);
		}

		ks.stack[nbp+1].reset<arr_xt<double> >(aa);			
		return 1;
	}
};


class CallableFunctionColon : public CallableFunction
{
public:
	CallableFunctionColon(){m_sName="colon";}

	int __fun_call(Executor& ks)
	{	
		int nbp=ks.stack.vbp.back();
		int pmc=ks.stack.nsp-nbp;

		double s1,s2,sp;

		if(pmc==2)
		{
			s1=PLCast<double>::g(ks.stack[nbp+1]);
			s2=PLCast<double>::g(ks.stack[nbp+2]);
			sp=s2>=s1?+1.0:-1.0;
		}
		else
		{
			s1=PLCast<double>::g(ks.stack[nbp+1]);
			sp=PLCast<double>::g(ks.stack[nbp+2]);
			s2=PLCast<double>::g(ks.stack[nbp+3]);
		}

		size_t np=1+(s2-s1+sp*0.00000001)/sp;

		arr_xt<double> sxt;
		sxt.resize(np);
		for(size_t i=0;i<np;i++)
		{
			sxt(i)=s1+sp*i;
		}		

		ks.stack[nbp+1].reset<arr_xt<double> >(sxt);			
		return 1;
	}
};



class CallableFunctionArray : public CallableFunctionSize
{
public:

	CallableFunctionArray(){m_sName="array";}

	int __fun_call(Executor& ks)
	{
		int nsp=ks.stack.nsp;
		int nbp=ks.stack.vbp.back();
		int pmc=nsp-nbp;

		kvar_table av;
		kvar_table::value_array aa;
		
		aa.resize(pmc);
		for(int i=0;i<pmc;i++)
		{
			aa[i].second=ks.stack[nbp+1+i];
		}
		av.swap(aa);
		ks.stack[nbp+1].reset<kvar_table>(av);
		return 1;
	};

};



void init_scripting_module_array()
{
	kvar_table& ggvar(Executor::ggvar);

	ggvar["linspace"].kptr(new CallableFunctionLinspace);
	ggvar["trans"].kptr(new CallableFunctionArrayTrans);
	ggvar["size"].kptr(new CallableFunctionSize);
	ggvar["length"].kptr(new CallableFunctionLength);
	ggvar["colon"].kptr(new CallableFunctionColon);
	ggvar["array"].kptr(new CallableFunctionArray);
}


VHWD_LEAVE
