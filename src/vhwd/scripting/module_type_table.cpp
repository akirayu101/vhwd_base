#include "module.h"

VHWD_ENTER


template<>
CallableMetatableTypeT<Variant::V_TABLE>::CallableMetatableTypeT()
{
	
}

template<>
int CallableMetatableTypeT<Variant::V_TABLE>::__fun_call(Executor& ks)
{
	int nbp=ks.stack.vbp.back();
	ks.stack[nbp+1].reset<kvar_table>();
	return 1;
}


class kvar_make_table : public CallableFunction
{
public:
	int __fun_call(Executor& ks)
	{	
		int nbp=ks.stack.vbp.back();
		int pmc=ks.stack.nsp-nbp;
		if(pmc!=0)
		{
			ks.kerror("pmc!=0");
		}			
		ks.stack[nbp+1].reset<kvar_table>();
		return 1;
	}
};

class kvar_make_metatable : public CallableFunction
{
public:
	int __fun_call(Executor& ks)
	{	
		int nbp=ks.stack.vbp.back();
		int pmc=ks.stack.nsp-nbp;
		if(pmc==0)
		{
			ks.stack[nbp+1].kptr(new CallableMetatable);
			return 1;
		}
		else if(pmc==2)
		{
			kvar_table* p1=ks.stack[nbp+1].ptr<kvar_table>();
			kvar_table* p2=ks.stack[nbp+2].ptr<kvar_table>();
			if(!p1||!p2)
			{
				return -1;
			}

			CallableMetatable* mt=new CallableMetatable;
			mt->table_member.swap(*p1);
			mt->table_static.swap(*p2);
			ks.stack[nbp+1].kptr(mt);
			return 1;

		}
		else
		{
			return -1;
		}
	}
};


class kvar_is_nil : public CallableFunction
{
public:
	int __fun_call(Executor& ks)
	{	
		if(!CheckParamCount(ks,1))
		{
			return -1;
		}

		Variant& v(ks.get(0));
		bool b=v.bptr()&&v.kptr()==NULL;
		v.reset<bool>(b);
		return 1;
	}
};

class kvar_get_iterator : public CallableFunction
{
public:
	int __fun_call(Executor& ks)
	{	
		int nbp=ks.stack.vbp.back();
		int pmc=ks.stack.nsp-nbp;
		if(pmc!=2)
		{
			ks.kerror("pmc!=2");
		}
		CallableData* d=ks.stack[nbp+1].kptr();
		int n=PLCast<int>::g(ks.stack[nbp+2]);
		if(d)
		{
			CallableData* g=d->GetIterator(n);
			ks.stack[nbp+1].kptr(g);
		}
		else
		{
			ks.stack[nbp+1].clear();
		}
		
		return 1;
	}
};

template<>
void CallableMetatableTypeT<Variant::V_TABLE>::init()
{
	kvar_table& ggvar(Executor::ggvar);
	gVariantMetaTable[Variant::V_TABLE].reset(new CallableMetatableTypeT<Variant::V_TABLE>);
	ggvar["type"].ref<kvar_table>()["table"].kptr(gVariantMetaTable[Variant::V_TABLE]);

	class kvar_is_table : public CallableFunction
	{
	public:
		int __fun_call(Executor& ks)
		{	
			int nbp=ks.stack.vbp.back();
			int pmc=ks.stack.nsp-nbp;
			if(pmc!=1)
			{
				ks.kerror("pmc!=1");
			}
			CallableData* t=ks.stack[nbp+1].kptr();
			ks.stack[nbp+1].reset<bool>(t && t->ToTable());
			return 1;
		}
	};

	ggvar["is_table"].kptr(new kvar_is_table);
	ggvar["is_nil"].kptr(new kvar_is_nil);

	ggvar["table"].kptr(new kvar_make_table);
	ggvar["metatable"].kptr(new kvar_make_metatable);
	ggvar["get_iterator"].kptr(new kvar_get_iterator);

	class kvar_table_size : public CallableFunction
	{
	public:
		int __fun_call(Executor& ks)
		{	
			int nbp=ks.stack.vbp.back();
			int pmc=ks.stack.nsp-nbp;
			if(pmc!=1)
			{
				ks.kerror("pmc!=1");
			}
			CallableData* d=ks.stack[nbp+1].kptr();
			CallableValueT<kvar_table>* t=d->ToTable();
			if(t)
			{
				ks.stack[nbp+1].reset<int64_t>(t->value.size());
			}
			else
			{
				ks.kerror("not table");
			}
			
			return 1;
		}
	};

	ggvar["table_size"].kptr(new kvar_table_size);
}

VHWD_LEAVE
