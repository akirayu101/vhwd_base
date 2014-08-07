#ifndef __H_VHWD_SCRIPTING_VARIANT_CODE__
#define __H_VHWD_SCRIPTING_VARIANT_CODE__

#include "vhwd/scripting/variant.h"

VHWD_ENTER


class kvar_inst
{
public:
	kvar_inst(int32_t op=0,int32_t pm=0):inst(op),parm(pm){}
	int32_t inst;
	int32_t parm;
};


class CallableCode : public CallableFunction
{
public:

	CallableCode();

	arr_1t<kvar_inst> aInsts;
	arr_1t<Variant> aConst;

	intptr_t nLocal;
	intptr_t npThis;
	BitFlags flags;

	virtual int __fun_call(Executor& ks);

};


VHWD_LEAVE
#endif
