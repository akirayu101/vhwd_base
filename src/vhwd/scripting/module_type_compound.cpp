#include "module.h"

VHWD_ENTER


template<>
CallableMetatableTypeT<Variant::V_COMPOUND>::CallableMetatableTypeT()
{
	
}

template<>
int CallableMetatableTypeT<Variant::V_COMPOUND>::__fun_call(Executor& ks)
{
	int nbp=ks.stack.vbp.back();
	ks.stack[nbp+1].reset<kvar_compound>();
	return 1;
}


template<>
void CallableMetatableTypeT<Variant::V_COMPOUND>::init()
{
	kvar_table& ggvar(Executor::ggvar);
	gVariantMetaTable[Variant::V_COMPOUND].reset(new CallableMetatableTypeT<Variant::V_COMPOUND>);
	ggvar["type"].ref<kvar_table>()["compound"].kptr(gVariantMetaTable[Variant::V_COMPOUND]);

	
}

VHWD_LEAVE
