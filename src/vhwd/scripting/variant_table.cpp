
#include "vhwd/scripting/executor.h"
#include "vhwd/scripting/variant_op1.h"
#include "vhwd/scripting/variant_table.h"

VHWD_ENTER

bool kvar_table::operator==(const  kvar_table& v2) const
{
	const kvar_table& v1(*this);
	if(v1.size()!=v2.size())
	{
		return false;
	}

	for(size_t i=0;i<v1.size();i++)
	{
		if(v1.get(i)!=v2.get(i))
		{
			return false;
		}
	}
	return true;
}

bool kvar_table::operator!=(const  kvar_table& v2) const
{
	const kvar_table& v1(*this);
	return !(v1==v2);
}

uint32_t hash_t<kvar_table>::operator()(const kvar_table& o)
{
	return hash_array<kvar_table::value_proxy>::hash(o.begin(),o.end());
}

int CallableValueT<kvar_table>::__getindex(Executor& ks,const String& s)
{
	ks.push(value[s]);
	return 1;
}

int CallableValueT<kvar_table>::__setindex(Executor& ks,const String& s)
{
	ks.popq(value[s]);
	return 0;
}



int CallableValueT<kvar_table>::__getarray(Executor& ks)
{
	int pmc=ks.stack.nsp-ks.stack.vbp.back();
	if(pmc!=1) return -1;

	size_t x=PLCast<size_t>::g(ks.top());
	if(x<value.size())
	{
		ks.top()=value.get(x).second;
		return 1;
	}
	else
	{
		ks.logger.LogWarning("invalid index: %u > table.size:%u",(uint32_t)x,(uint32_t)value.size());
		ks.top().clear();
		return 1;
	}

}

int CallableValueT<kvar_table>::__setarray(Executor& ks)
{
	int pmc=ks.stack.nsp-ks.stack.vbp.back();
	if(pmc!=1) return -1;

	size_t x=PLCast<size_t>::g(ks.top());
	if(x<value.size())
	{
		value.get(x).second=ks.get(-1);
		return 0;
	}
	else
	{
		ks.logger.LogWarning("invalid index: %u > table.size:%u",(uint32_t)x,(uint32_t)value.size());
		return 0;
	}
}




class CallableIterator_Table1 : public CallableIterator
{
public:
	CallableIterator_Table1(kvar_table& t){itcur=t.begin();itend=t.end();}

	typedef kvar_table::iterator iterator;
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
			ks.stack[nbp+1]=(*itcur).second;
			ks.stack[nbp+2].reset<bool>(true);
			itcur++;
			return 2;
		}
	}
};

class CallableIterator_Table2 : public CallableIterator
{
public:
	CallableIterator_Table2(kvar_table& t){itcur=t.begin();itend=t.end();}

	typedef kvar_table::iterator iterator;
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
			ks.stack[nbp+1].reset((*itcur).first);
			ks.stack[nbp+2]=(*itcur).second;
			ks.stack[nbp+3].reset<bool>(true);
			itcur++;
			return 3;
		}
	}
};


CallableIterator* CallableValueT<kvar_table>::GetIterator(int n)
{
	if(n==1)
	{
		return new CallableIterator_Table1(value);
	}
	if(n==2)
	{
		return new CallableIterator_Table2(value); 
	}
	return new CallableIterator;
}


int CallableTableEx::__setindex(Executor& ks,const String& s)
{
	ks.logger.LogError("attempt to modified const table[%s]",s);
	return 0;
}

int CallableTableEx::__setarray(Executor& ks)
{
	ks.logger.LogError("attempt to modified const table");
	return 0;
}

uint32_t CallableValueT<kvar_table>::hashcode() const
{
	hash_t<type> h;
	return h(value);
}

bool CallableValueT<kvar_table>::IsEqualTo(const CallableData* d) const
{
	const CallableValueT<type>* q=((CallableValueT<type>*)d)->ToTable();
	return q && value==q->value;
}



VHWD_LEAVE
