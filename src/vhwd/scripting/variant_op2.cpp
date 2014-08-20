
#include "vhwd/scripting/executor.h"
#include "vhwd/scripting/variant_op2.h"

VHWD_ENTER

template<>
void kvar_helper<XOP2_ADD>::g2(Executor& ks)
{
	kvar_handler2<PL2_opx_num<PL2_add> >::g1(ks,f2(ks));
}
template<>
void kvar_helper<XOP2_SUB>::g2(Executor& ks)
{
	kvar_handler2<PL2_opx_num<PL2_sub> >::g1(ks,f2(ks));
}
template<>
void kvar_helper<XOP2_MUL>::g2(Executor& ks)
{
	kvar_handler2<PL2_opx_num<PL2_mul> >::g1(ks,f2(ks));
}
template<>
void kvar_helper<XOP2_DIV>::g2(Executor& ks)
{
	kvar_handler2<PL2_opx_num<PL2_div> >::g1(ks,f2(ks));
}
template<>
void kvar_helper<XOP2_MOD>::g2(Executor& ks)
{
	kvar_handler2<PL2_opx_num<PL2_mod> >::g1(ks,f2(ks));
}

template<>
void kvar_helper<XOP2_POW>::g2(Executor& ks)
{
	kvar_handler2<PL2_opx_num<PL2_pow> >::g1(ks,f2(ks));
}

template<>
void kvar_helper<XOP2_DOT_MUL>::g2(Executor& ks)
{
	kvar_handler2<PL2_opx_num<PL2_mul> >::g1(ks,f2(ks));
}
template<>
void kvar_helper<XOP2_DOT_DIV>::g2(Executor& ks)
{
	kvar_handler2<PL2_opx_num<PL2_div> >::g1(ks,f2(ks));
}
template<>
void kvar_helper<XOP2_EQ>::g2(Executor& ks)
{
	kvar_handler2<PL2_opx_num<PL2_eq> >::g1(ks,f2(ks));
}
template<>
void kvar_helper<XOP2_NE>::g2(Executor& ks)
{
	kvar_handler2<PL2_opx_num<PL2_ne> >::g1(ks,f2(ks));
}
template<>
void kvar_helper<XOP2_GT>::g2(Executor& ks)
{
	kvar_handler2<PL2_opx_num<PL2_gt> >::g1(ks,f2(ks));
}
template<>
void kvar_helper<XOP2_GE>::g2(Executor& ks)
{
	kvar_handler2<PL2_opx_num<PL2_ge> >::g1(ks,f2(ks));
}
template<>
void kvar_helper<XOP2_LT>::g2(Executor& ks)
{
	kvar_handler2<PL2_opx_num<PL2_lt> >::g1(ks,f2(ks));
}
template<>
void kvar_helper<XOP2_LE>::g2(Executor& ks)
{
	kvar_handler2<PL2_opx_num<PL2_le> >::g1(ks,f2(ks));
}

template<>
void kvar_helper<XOP2_AND>::g2(Executor& ks)
{
	kvar_handler2<PL2_opx_num<PL2_and> >::g1(ks,f2(ks));
}
template<>
void kvar_helper<XOP2_OR>::g2(Executor& ks)
{
	kvar_handler2<PL2_opx_num<PL2_or> >::g1(ks,f2(ks));
}
template<>
void kvar_helper<XOP2_XOR>::g2(Executor& ks)
{
	kvar_handler2<PL2_opx_num<PL2_xor> >::g1(ks,f2(ks));
}
template<>
void kvar_helper<XOP2_BITWISE_AND>::g2(Executor& ks)
{
	kvar_handler2<PL2_opx_num<PL2_bw_and> >::g1(ks,f2(ks));
}
template<>
void kvar_helper<XOP2_BITWISE_OR>::g2(Executor& ks)
{
	kvar_handler2<PL2_opx_num<PL2_bw_or> >::g1(ks,f2(ks));
}
template<>
void kvar_helper<XOP2_BITWISE_XOR>::g2(Executor& ks)
{
	kvar_handler2<PL2_opx_num<PL2_bw_xor> >::g1(ks,f2(ks));
}

template<>
void kvar_helper<XOP2_CAT>::g2(Executor& ks)
{
	String s1=PLCast<String>::g(ks.get(-1));
	String s2=PLCast<String>::g(ks.get(0));
	ks.get(-1).reset<String>(s1+s2);
}


VHWD_LEAVE
