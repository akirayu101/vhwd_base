#include "vhwd/scripting/variant.h"
#include "vhwd/scripting/variant_op1.h"
#include "vhwd/scripting/variant_table.h"
#include "vhwd/scripting/executor.h"

VHWD_ENTER


class CallableIterator_Class1 : public CallableIterator
{
public:
	CallableIterator_Class1(CallableClass& t){itcur=t.value.begin();itend=t.value.end();}

	typedef arr_1t<Variant>::iterator iterator;
	iterator itcur,itend;

	virtual int __fun_call(Executor& ks)
	{
		int nbp=ks.stack.vbp.back();

		if(itcur==itend)
		{
			ks.stack[nbp+1].reset<bool>(false);
			return 1;
		}
		else
		{
			ks.stack[nbp+1]=(*itcur);
			ks.stack[nbp+2].reset<bool>(true);
			itcur++;
			return 2;
		}
	}
};


class CallableIterator_Class2 : public CallableIterator_Class1
{
public:
	CallableIterator_Class2(CallableClass& t):CallableIterator_Class1(t)
	{
		itval=t.metax->table_member.begin();
	}

	kvar_table::iterator itval;

	virtual int __fun_call(Executor& ks)
	{
		int nbp=ks.stack.vbp.back();

		if(itcur==itend)
		{
			ks.stack[nbp+1].reset<bool>(false);
			return 1;
		}
		else
		{
			ks.stack[nbp+1].reset((*itval).first);
			ks.stack[nbp+2]=(*itcur);
			ks.stack[nbp+3].reset<bool>(true);
			itval++;
			itcur++;
			return 3;
		}
	}
};

CallableIterator* CallableClass::GetIterator(int n)
{
	if(n==1)
	{
		return new CallableIterator_Class1(*this);
	}
	else if(n==2)
	{
		return new CallableIterator_Class2(*this);		
	}
	else
	{
		return new CallableIterator;
	}
}

int CallableMetatable::__fun_call(Executor& ks) // top(...)
{
	int nbp=ks.stack.vbp.back();
	CallableClass* p=new CallableClass;
	size_t n=table_member.size();
	p->value.resize(n);
	for(size_t i=0;i<n;i++)
	{
		p->value[i]=table_member.get(i).second;
	}

	p->metax.reset(this);	
	ks.stack[nbp+1].kptr(p);
	return 1;
}

int CallableClass::__getindex(Executor& ks,const String& s) // top.s
{

	int id=metax->table_member.find(s);
	if(id>=0)
	{
		ks.push(value[id]);
		return 1;
	}

	id=metax->table_static.find(s);
	if(id>=0)
	{
		ks.push(metax->table_static.get(id).second);
		return 1;
	}
	return -1;
}

int CallableClass::__setindex(Executor& ks,const String& s) // top.s=val
{

	int id=metax->table_member.find(s);
	if(id>=0)
	{
		ks.popq(value[id]);
		return 0;
	}

	id=metax->table_static.find(s);
	if(id>=0)
	{
		ks.popq(metax->table_static.get(id).second);
		return 0;
	}

	return -1;
}



int CallableClass::__getarray(Executor& ks)
{
	int pmc=ks.stack.nsp-ks.stack.vbp.back();
	if(pmc!=1) return -1;

	size_t x=PLCast<size_t>::g(ks.top());
	if(x<value.size())
	{
		ks.top()=value[x];
		return 1;
	}
	else
	{
		ks.logger.LogWarning("invalid index: %u > table.size:%u",(uint32_t)x,(uint32_t)value.size());
		ks.top().clear();
		return 1;
	}

}

int CallableClass::__setarray(Executor& ks)
{
	int pmc=ks.stack.nsp-ks.stack.vbp.back();
	if(pmc!=1) return -1;

	size_t x=PLCast<size_t>::g(ks.top());
	if(x<value.size())
	{
		value[x]=ks.get(-1);
		return 0;
	}
	else
	{
		ks.logger.LogWarning("invalid index: %u > table.size:%u",(uint32_t)x,(uint32_t)value.size());
		return 0;
	}
}
VHWD_LEAVE
