#include "module.h"

VHWD_ENTER


template<>
CallableMetatableTypeT<Variant::V_STRING>::CallableMetatableTypeT()
{
	
}

template<>
int CallableMetatableTypeT<Variant::V_STRING>::__fun_call(Executor& ks)
{
	int nbp=ks.stack.vbp.back();
	ks.stack[nbp+1].reset<String>();
	return 1;
}


template<>
void CallableMetatableTypeT<Variant::V_STRING>::init()
{
	kvar_table& ggvar(Executor::ggvar);
	gVariantMetaTable[Variant::V_STRING].reset(new CallableMetatableTypeT<Variant::V_STRING>);
	ggvar["type"].ref<kvar_table>()["string"].kptr(gVariantMetaTable[Variant::V_STRING]);

	class kvar_table_is_string : public CallableFunction
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

			ks.stack[nbp+1].reset<bool>(ks.stack[nbp+1].flag()==Variant::V_STRING);
			return 1;
		}
	};

	ggvar["is_string"].kptr(new kvar_table_is_string);

	class kvar_string_size : public CallableFunction
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

			String* t=ks.stack[nbp+1].ptr<String>();
			if(t)
			{
				ks.stack[nbp+1].reset<int64_t>(t->size());
			}
			else
			{
				ks.kerror("not string");
			}
			
			return 1;
		}
	};

	ggvar["string_size"].kptr(new kvar_string_size);
}

VHWD_LEAVE
