#include "module.h"

VHWD_ENTER

LitePtrT<CallableMetatable> gVariantMetaTable[Variant::V_TYPEMASK+1];

CallableMetatable* CallableMetatable::get(Variant& v)
{
	return gVariantMetaTable[v.flag()].get();
}


class CallableGetMetatable : public CallableData
{
public:
	CallableGetMetatable(CallableMetatable* d):table(d){}
	LitePtrT<CallableMetatable> table;

	int __fun_call(Executor& ks)
	{	
		int nbp=ks.stack.vbp.back();
		ks.stack[nbp+1].kptr(table.get());
		return 1;
	}
};


CallableMetatableType::CallableMetatableType()
{
	value["metatable"].kptr(new CallableGetMetatable(this));
}


int CallableValue::__getindex(Executor& ks,const String& s)
{
	int flag=ks.stack[ks.stack.nsp+1].flag();
	if(gVariantMetaTable[flag])
	{
		return gVariantMetaTable[flag]->__getindex(ks,s);
	}
	return -1;
}

void init_scripting_module_type()
{
	CallableMetatableTypeT<Variant::V_BOOL>::init();
	CallableMetatableTypeT<Variant::V_INT>::init();
	CallableMetatableTypeT<Variant::V_DOUBLE>::init();
	CallableMetatableTypeT<Variant::V_COMPLEX>::init();
	CallableMetatableTypeT<Variant::V_COMPOUND>::init();
	CallableMetatableTypeT<Variant::V_TABLE>::init();
}

VHWD_LEAVE
