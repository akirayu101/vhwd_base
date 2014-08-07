#include "vhwd/scripting/codegen_generator.h"
#include "vhwd/scripting/parser_node.h"
#include "vhwd/scripting/executor.h"
#include "vhwd/scripting/variant_table.h"

VHWD_ENTER

TNodeVisitorCG_Generator::TNodeVisitorCG_Generator(CodeGen& g)
:TNodeVisitorCG(g)
,lvis(*this)
,rvis(*this)
,ascope(*this)
{

}
void syn_param::lock_scope()
{

	stk.push_back(m_refData);
	m_refData.reset(new syn_scope_function(cgen));
	cgen.pCode=m_refData->pCode;
	m_refData->aConst.swap(cgen.aConst);
	m_refData->aInsts.swap(cgen.aInsts);

	lock_block();

}

void syn_param::unlock_scope()
{
	unlock_block();

	cgen.emit(XOP_HALT);

	m_refData->pCode->aConst.assign(cgen.aConst.begin(),cgen.aConst.end());
	m_refData->pCode->aInsts.swap(cgen.aInsts);

	m_refData->aConst.swap(cgen.aConst);
	m_refData->aInsts.swap(cgen.aInsts);

	m_refData=stk.back();
	stk.pop_back();
}


void syn_param::lock_block()
{
	m_refData->aBlock.push_back(new syn_scope_block(m_refData->aBlock.back()->GetLocalNum()));

}

void syn_param::unlock_block()
{
	int n=m_refData->aBlock.back()->GetLocalNum();
	if(n>m_refData->pCode->nLocal)
	{
		m_refData->pCode->nLocal=n;
	}
	m_refData->aBlock.pop_back();
}

void syn_param::lock_loop1()
{
	m_refData->aLoop1.push_back(syn_kloop_jmp());
}

void syn_param::unlock_loop1()
{
	m_refData->aLoop1.pop_back();
}

void syn_param::lock_loop2()
{
	m_refData->aLoop2.push_back(syn_kloop_jmp());
}

void syn_param::unlock_loop2()
{
	m_refData->aLoop2.pop_back();
}

void syn_param::set_loop1(size_t n)
{
	for(syn_kloop_jmp::iterator it=m_refData->aLoop1.back().begin();it!=m_refData->aLoop1.back().end();++it)
	{
		cgen.aInsts[*it].parm=n;
	}
}

void syn_param::set_loop2(size_t n)
{
	for(syn_kloop_jmp::iterator it=m_refData->aLoop2.back().begin();it!=m_refData->aLoop2.back().end();++it)
	{
		cgen.aInsts[*it].parm=n;
	}
}


TNodeVisitorCG_GeneratorUnit::TNodeVisitorCG_GeneratorUnit(TNodeVisitorCG_Generator& cg_):TNodeVisitorCG(cg_.cg),cgen(cg_)
{

}


void TNodeVisitorCG_Generator::parse(TNode* node)
{
	LockGuard<syn_param,LP_param> lock1(ascope);
	cg.pkstmt.reset(ascope.GetData()->pCode.get());
	node->accept(rvis);
}


int TNodeVisitorCG_Generator::addglobal(const String& s)
{
	return gscope.variables.find2(s);
}

int TNodeVisitorCG_Generator::add_local(const String& s)
{
	return ascope.local_variables().find2(s)+1;
}

int TNodeVisitorCG_Generator::add_param(const String& s)
{
	return ascope.GetData()->variables.find2(s)+1;
}

int TNodeVisitorCG_Generator::find_ggvar(const String& s)
{
	return Executor::ggvar.find(s);
}


int TNodeVisitorCG_Generator::find_param(const String& s)
{
	return ascope.GetData()->variables.find(s)+1;
}

int TNodeVisitorCG_Generator::findglobal(const String& s)
{
	return gscope.variables.find(s);
}

int TNodeVisitorCG_Generator::find_local(const String& s,int d)
{
	typedef arr_1t<DataPtrT<syn_scope_block> > arr_type;
	typedef arr_type::reverse_iterator iterator;

	arr_type& arr(ascope.GetData()->aBlock);

	if(d==VAR_DEFAULT)
	{
		for(iterator it=arr.rbegin();it!=arr.rend();++it)
		{
			int id=(*it)->variables.find(s);
			if(id<0) continue;
			return id+(*it)->local_num+1;
		}

		return -1;
	}

	if(d==VAR_LOCAL)
	{
		iterator it=arr.rbegin();
		int id=(*it)->variables.find2(s);
		return id+(*it)->local_num+1;
	}

	return -1;

}



size_t TNodeVisitorCG_Generator::ggidx(const String& s)
{
	Variant v;v.reset(s);
	return aConst.insert(v);
}

size_t TNodeVisitorCG_Generator::ggidx(double s)
{
	Variant v;v.reset(s);
	return aConst.insert(v);
}

size_t TNodeVisitorCG_Generator::ggidx(int64_t s)
{
	Variant v;v.reset(s);
	return aConst.insert(v);
}


size_t TNodeVisitorCG_Generator::ggidx(CallableData* s)
{
	Variant v;v.kptr(s);
	return aConst.insert(v);
}

void TNodeVisitorCG_Generator::emit(int op,int pm)
{
	aInsts.push_back(kvar_inst(op,pm));
}

void TNodeVisitorCG_Generator::emit_push(const String& s)
{
	emit(XOP_GET_LLVAR,ggidx(s));
}

void TNodeVisitorCG_Generator::emit_push(int64_t s)
{
	emit(XOP_GET_LLVAR,ggidx(s));
}

void TNodeVisitorCG_Generator::emit_push(double s)
{
	emit(XOP_GET_LLVAR,ggidx(s));
}

void TNodeVisitorCG_Generator::emit_push(CallableData* s)
{
	emit(XOP_GET_LLVAR,ggidx(s));
}

void TNodeVisitorCG_Generator::emit_push(int32_t s)
{
	emit(XOP_PUSH_INT_DIRECT,s);
}

void TNodeVisitorCG_Generator::emit_push(bool s)
{
	emit(XOP_PUSH_BOOL,s);
}

void TNodeVisitorCG_Generator::emit_get_ggvar(const String& s)
{
	int id=this->find_ggvar(s);
	if(id<0)
	{
		this->kerror("not found");
	}
	emit(XOP_GET_GGVAR,id);
}

void TNodeVisitorCG_Generator::emit_set_table(const String& s)
{
	emit(XOP_SET_TABLE,ggidx(s));
}
void TNodeVisitorCG_Generator::emit_get_table(const String& s)
{
	emit(XOP_GET_TABLE,ggidx(s));
}
void TNodeVisitorCG_Generator::emit_set_index(const String& s)
{
	emit(XOP_SET_INDEX,ggidx(s));
}
void TNodeVisitorCG_Generator::emit_get_index(const String& s)
{
	emit(XOP_GET_INDEX,ggidx(s));
}
void TNodeVisitorCG_Generator::emit_set_local(int n)
{
	emit(XOP_SET_LOCAL,n);
}
void TNodeVisitorCG_Generator::emit_get_local(int n)
{
	emit(XOP_GET_LOCAL,n);
}


void TNodeVisitorCG_Generator::kerror(const String& s)
{
	Exception::XError(String::Format("CodeGen:Generator:%s",s));
}

VHWD_LEAVE

