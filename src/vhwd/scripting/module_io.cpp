#include "module.h"

VHWD_ENTER

void init_scripting_module_io()
{
	kvar_table& ggvar(Executor::ggvar);
	kvar_table& io(ggvar["io"].ref<kvar_table>());


	class CallableFunctionPrint : public CallableFunction
	{
	public:
		CallableFunctionPrint(){m_sName="io.print";}

		void print_impl(Variant& v)
		{
			Console::Write(PLCast<String>::g(v));
		}


		int __fun_call(Executor& ks)
		{	
			int nbp=ks.stack.vbp.back();
			int pmc=ks.stack.nsp-nbp;

			for(int i=1;i<=pmc;i++)
			{
				print_impl(ks.stack[nbp+i]);
			}

			ks.stack[nbp+1].reset<int64_t>(pmc);			
			return 1;
		}
	};

	io["print"].kptr(new CallableFunctionPrint);


	class CallableFunctionPrintln : public CallableFunction
	{
	public:
		CallableFunctionPrintln(){m_sName="io.println";}

		void print_impl(Variant& v)
		{
			Console::WriteLine(PLCast<String>::g(v));
		}

		int __fun_call(Executor& ks)
		{	
			int nbp=ks.stack.vbp.back();
			int pmc=ks.stack.nsp-nbp;

			if(pmc==0)
			{
				Console::Write("\n");
			}
			else
			{
				for(int i=1;i<=pmc;i++)
				{
					print_impl(ks.stack[nbp+i]);
				}
			}

			ks.stack[nbp+1].reset<int64_t>(pmc);			
			return 1;
		}
	};

	io["println"].kptr(new CallableFunctionPrintln);
}

VHWD_LEAVE
