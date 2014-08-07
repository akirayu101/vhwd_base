#ifndef __H__VHWD_SCRIPTING_MODULE__
#define __H__VHWD_SCRIPTING_MODULE__

#include "vhwd/scripting/executor.h"
#include "vhwd/scripting/variant_op1.h"
#include "vhwd/scripting/variant_op2.h"
#include "vhwd/scripting/variant_table.h"


VHWD_ENTER

extern LitePtrT<CallableMetatable> gVariantMetaTable[Variant::V_TYPEMASK+1];

class CallableMetatableType : public CallableMetatable
{
public:
	CallableMetatableType();
};

template<int N>
class CallableMetatableTypeT : public CallableMetatableType
{
public:

	CallableMetatableTypeT();

	int __fun_call(Executor& ks);

	static void init();
};

void init_scripting_module_os();
void init_scripting_module_io();
void init_scripting_module_array();
void init_scripting_module_math();
void init_scripting_module_type();

VHWD_LEAVE
#endif
