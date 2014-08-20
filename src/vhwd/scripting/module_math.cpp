#include "vhwd/scripting/variant_op1.h"
#include "module.h"


VHWD_ENTER


template<int N>
class CallableMathOp1 : public CallableFunction
{
public:

	int __fun_call(Executor& ks)
	{

		if(!CheckParamCount(ks,1))
		{
			return -1;
		}

		kvar_helper<N>::g1(ks);		
		return 1;
	}

};


template<int N>
class CallableMathOp2 : public CallableFunction
{
public:

	int __fun_call(Executor& ks)
	{	
		if(!CheckParamCount(ks,2))
		{
			ks.kerror("param count should be 2");
			return -1;
		}

		kvar_helper<N>::g2(ks);		
		return 1;
	}
};



template<typename P>
class CallableHandler1 : public CallableFunction
{
public:

	int __fun_call(Executor& ks)
	{	
		if(!CheckParamCount(ks,1))
		{
			ks.kerror("param count should be 1");
			return -1;
		}

		kvar_handler1<PL1_opx_num<P> >::g1(ks,kvar_helper_base::f1(ks));
		return 1;
	}

};



class CallableFunctionComplex : public CallableFunction
{
public:

	int __fun_call(Executor& ks)
	{	
		int pmc=ks.stack.nsp-ks.stack.vbp.back();

		switch(pmc)
		{
		case 0:
			ks.stack[ks.stack.nsp+1].reset<std::complex<double> >();
			break;
		case 1:
			kvar_handler1<PL1_opx_num<PL1_cpx> >::g1(ks,kvar_helper_base::f1(ks));
			break;
		case 2:
			kvar_handler2<PL2_opx_num<PL2_cpx> >::g1(ks,kvar_helper_base::f2(ks));
			break;
		default:
			return -1;
		}


		return 1;
	}

};

void init_scripting_module_math()
{
	kvar_table& ggvar(Executor::ggvar);

	CallableTableEx* t=new CallableTableEx;
	ggvar["math"].kptr(t);
	kvar_table& ggtbl(t->value);

	ggtbl["sin"].kptr(new CallableMathOp1<XOP1_SIN>);
	ggtbl["cos"].kptr(new CallableMathOp1<XOP1_COS>);
	ggtbl["tan"].kptr(new CallableMathOp1<XOP1_TAN>);
	ggtbl["log"].kptr(new CallableMathOp1<XOP1_LOG>);
	ggtbl["log10"].kptr(new CallableMathOp1<XOP1_LOG10>);

	ggtbl["add"].kptr(new CallableMathOp2<XOP2_ADD>);
	ggtbl["sub"].kptr(new CallableMathOp2<XOP2_SUB>);
	ggtbl["div"].kptr(new CallableMathOp2<XOP2_DIV>);
	ggtbl["mul"].kptr(new CallableMathOp2<XOP2_MUL>);
	ggtbl["mod"].kptr(new CallableMathOp2<XOP2_MOD>);
	ggtbl["pow"].kptr(new CallableMathOp2<XOP2_POW>);
	ggtbl["dot_div"].kptr(new CallableMathOp2<XOP2_DOT_MUL>);
	ggtbl["dot_mul"].kptr(new CallableMathOp2<XOP2_DOT_MUL>);

	ggtbl["ne"].kptr(new CallableMathOp2<XOP2_NE>);
	ggtbl["eq"].kptr(new CallableMathOp2<XOP2_EQ>);
	ggtbl["gt"].kptr(new CallableMathOp2<XOP2_GT>);
	ggtbl["lt"].kptr(new CallableMathOp2<XOP2_LT>);
	ggtbl["ge"].kptr(new CallableMathOp2<XOP2_GE>);
	ggtbl["le"].kptr(new CallableMathOp2<XOP2_LE>);

	ggtbl["and"].kptr(new CallableMathOp2<XOP2_AND>);
	ggtbl["or"].kptr(new CallableMathOp2<XOP2_OR>);
	ggtbl["xor"].kptr(new CallableMathOp2<XOP2_XOR>);

	ggtbl["bitwise_and"].kptr(new CallableMathOp2<XOP2_BITWISE_AND>);
	ggtbl["bitwise_or"].kptr(new CallableMathOp2<XOP2_BITWISE_OR>);
	ggtbl["bitwise_xor"].kptr(new CallableMathOp2<XOP2_BITWISE_XOR>);


	ggtbl["abs"].kptr(new CallableHandler1<PL1_abs>);
	ggtbl["sqrt"].kptr(new CallableHandler1<PL1_sqrt>);
	ggtbl["real"].kptr(new CallableHandler1<PL1_real>);
	ggtbl["imag"].kptr(new CallableHandler1<PL1_imag>);
	ggtbl["conj"].kptr(new CallableHandler1<PL1_conj>);
	ggtbl["complex"].kptr(new CallableFunctionComplex);

	ggvar["complex"]=ggtbl["complex"];

	for(kvar_table::iterator it=ggtbl.begin();it!=ggtbl.end();++it)
	{
		CallableFunction* f=dynamic_cast<CallableFunction*>((*it).second.kptr());
		if(f){f->m_sName="math."+(*it).first;}
	}
};


VHWD_LEAVE
