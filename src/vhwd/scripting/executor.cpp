#include "vhwd/scripting/codegen.h"
#include "vhwd/scripting/variant_table.h"
#include "vhwd/scripting/parser_node.h"
#include "vhwd/scripting/executor.h"

#include "module.h"


VHWD_ENTER

void init_scripting_module_modules()
{
	static bool first=true;
	if(!first) return;
	first=false;

	init_scripting_module_type();
	init_scripting_module_io();
	init_scripting_module_os();
	init_scripting_module_math();
	init_scripting_module_array();

}

kvar_stack::kvar_stack()
{
	nsp=0;
	tbl.resize(1024*64);
	vbp.push_back(nsp);
}

void kvar_stack::swap(kvar_stack& o)
{
	tbl.swap(o.tbl);
	vbp.swap(o.vbp);
	std::swap(nsp,o.nsp);
}


Executor::Executor()
{
	init_scripting_module_modules();

}

kvar_table Executor::ggvar;

void Executor::kerror(const String& s)
{
	Exception::XError(s);
}


CallableData* Executor::kptr1()
{
	CallableData* q=get(0).kptr();
	if(!q)
	{
		Exception::XBadCast();
	}
	sadj(-1);
	return q;
}

CallableData* Executor::kptr2()
{
	CallableData* q=top().kptr();
	if(!q)
	{
		//q=CallableMetatable::get(get(0));
		Exception::XBadCast();
	}
	stack[0]=stack[stack.nsp--];
	return q;
}



void Executor::callx()
{
	DataPtrT<CallableData> q(kptr1());
	int m=q->__fun_call(*this);
	if(m<0)
	{
		Exception::XBadCast();
	}
	stack.nsp=stack.vbp.back()+m;
	stack.vbp.pop_back();
}

void Executor::calln(int n)
{
	DataPtrT<CallableData> q(kptr1());
	int nbp=stack.nsp-n;
	stack.vbp.push_back(nbp);
	int m=q->__fun_call(*this);
	stack.vbp.pop_back();
	if(m<0)
	{
		Exception::XBadCast();
	}
	stack.nsp=nbp+m;
}


void Executor::get_array()
{
	DataPtrT<CallableData> q(kptr2());
	int nbp=stack.vbp.back();
	int m=q->__getarray(*this);
	stack.vbp.pop_back();
	if(m!=1)
	{
		Exception::XBadCast();
	}
	stack.nsp=nbp+m;
}

void Executor::set_array()
{
	DataPtrT<CallableData> q(kptr2());
	int nbp=stack.vbp.back();
	int m=q->__setarray(*this);
	stack.vbp.pop_back();
	if(m!=0)
	{
		Exception::XBadCast();
	}
	stack.nsp=nbp-1;
}
void Executor::get_array(int n)
{
	DataPtrT<CallableData> q(kptr2());
	int nbp=stack.nsp-n;
	stack.vbp.push_back(nbp);
	int m=q->__getarray(*this);
	stack.vbp.pop_back();
	if(m!=1)
	{
		Exception::XBadCast();
	}
	stack.nsp=nbp+m;
}

void Executor::set_array(int n)
{
	DataPtrT<CallableData> q(kptr2());
	int nbp=stack.nsp-n;
	stack.vbp.push_back(nbp);
	int m=q->__setarray(*this);
	stack.vbp.pop_back();
	if(m!=0)
	{
		Exception::XBadCast();
	}
	stack.nsp=nbp+m;
}

void Executor::get_index(const String& s)
{
	DataPtrT<CallableData> q(kptr2());
	int m=q->__getindex(*this,s);
	if(m!=1)
	{
		Exception::XBadCast();
	}
}


void Executor::set_index(const String& s)
{
	DataPtrT<CallableData> q(kptr2());
	int m=q->__setindex(*this,s);
	if(m!=0)
	{
		Exception::XBadCast();
	}
}




bool Executor::execute(const String& s)
{
	CodeGen cg;

	try
	{
		if(!cg.parse(s)) 
		{
			return false;
		}

		push(cg.pkstmt.get());
		calln(0);
		return true;

	}
	catch(std::exception& e)
	{
		logger.LogError(e.what());
	}

	return false;
}

VHWD_LEAVE
