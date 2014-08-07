
#include "vhwd/scripting/executor.h"
#include "vhwd/scripting/variant_op1.h"

VHWD_ENTER

template<>
void kvar_helper<XOP1_NOT>::g1(Executor& ks)
{
	kvar_handler1<PL1_opx_num<PL1_not> >::g1(ks,f1(ks));
}

template<>
void kvar_helper<XOP1_INV>::g1(Executor& ks)
{
	kvar_handler1<PL1_opx_num<PL1_inv> >::g1(ks,f1(ks));
}
template<>
void kvar_helper<XOP1_SIN>::g1(Executor& ks)
{
	kvar_handler1<PL1_opx_num<PL1_sin> >::g1(ks,f1(ks));
}
template<>
void kvar_helper<XOP1_COS>::g1(Executor& ks)
{
	kvar_handler1<PL1_opx_num<PL1_cos> >::g1(ks,f1(ks));
}
template<>
void kvar_helper<XOP1_TAN>::g1(Executor& ks)
{
	kvar_handler1<PL1_opx_num<PL1_tan> >::g1(ks,f1(ks));
}
template<>
void kvar_helper<XOP1_LOG>::g1(Executor& ks)
{
	kvar_handler1<PL1_opx_num<PL1_log> >::g1(ks,f1(ks));
}
template<>
void kvar_helper<XOP1_LOG10>::g1(Executor& ks)
{
	kvar_handler1<PL1_opx_num<PL1_log10> >::g1(ks,f1(ks));
}
template<>
void kvar_helper<XOP1_BITWISE_INV>::g1(Executor& ks)
{
	kvar_handler1<PL1_opx_num<PL1_bw_inv> >::g1(ks,f1(ks));
}

VHWD_LEAVE
