#ifndef __H_VHWD_SCRIPTING_VARIANT_TABLE__
#define __H_VHWD_SCRIPTING_VARIANT_TABLE__

#include "vhwd/scripting/variant.h"


VHWD_ENTER

class CallableTableEx : public CallableValueT<kvar_table>
{
public:
	int __setindex(Executor& ks,const String& s);
	int __setarray(Executor& ks);

	BitFlags flags;
};

class CallableMetatable : public CallableValueT<kvar_table>
{
public:
	kvar_table table_member;
	kvar_table& table_static;
	CallableMetatable():table_static(value){}

	virtual int __fun_call(Executor& ks);

	static CallableMetatable* get(Variant& v);

};


class CallableClass : public CallableData
{
public:

	virtual int __getindex(Executor& ks,const String& s);
	virtual int __setindex(Executor& ks,const String& s);

	int __getarray(Executor& ks);
	int __setarray(Executor& ks);

	arr_1t<Variant> value;
	DataPtrT<CallableMetatable> metax;

	virtual CallableClass* ToClass() {return this;}

	virtual CallableIterator* GetIterator(int n);

};


VHWD_LEAVE
#endif
