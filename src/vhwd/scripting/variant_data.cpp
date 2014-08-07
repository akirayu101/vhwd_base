#include "vhwd/scripting/variant.h"
#include "vhwd/scripting/variant_op1.h"
#include "vhwd/scripting/executor.h"

VHWD_ENTER


bool CallableFunction::CheckParamCount(Executor& ks,int pm)
{
	if(ks.stack.nsp-ks.stack.vbp.back()==pm)
	{
		return true;
	}
	return false;

}


int CallableFunction::__getindex(Executor& ks,const String& s)
{
	if(s=="name")
	{
		ks.push(m_sName);
		return 1;
	}
	else
	{
		return -1;
	}
}

int CallableFunction::__setindex(Executor& ks,const String& s)
{
	if(flags.get(CF_READONLY))
	{
		return -1;
	}

	if(s=="name")
	{
		m_sName=PLCast<String>::g(ks.top());ks.popq();
		return 0;
	}
	else
	{
		return -1;
	}
}

int CallableData::__getindex(Executor& ks,const String&) // top.s
{
	
	return -1;
}

int CallableData::__setindex(Executor&,const String&) // top.s=val
{
	return -1;
}

int CallableData::__getarray(Executor&) // top[s]
{
	return -1;
}

int CallableData::__setarray(Executor&) // top[s]=val
{
	return -1;
}

int CallableData::__fun_call(Executor&) // top(...)
{
	return -1;
}

int CallableData::__new_item(Executor&) // new top(...)
{
	return -1;
}

uint32_t hash_t<CallableData*>::operator()(CallableData* d)
{
	return d?d->hashcode():0;
}


uint32_t hash_t<DataPtrT<CallableData> >::operator()(const DataPtrT<CallableData>& o)
{
	return o.get()!=0?o->hashcode():0;
}


VHWD_LEAVE
