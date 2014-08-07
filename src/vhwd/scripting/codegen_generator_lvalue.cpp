#include "vhwd/scripting/codegen_generator.h"
#include "vhwd/scripting/parser_node.h"
#include "vhwd/scripting/executor.h"
#include "vhwd/scripting/variant_table.h"

VHWD_ENTER


void TNodeVisitorCG_GeneratorLValue::handle(TNode*)
{
	cgen.kerror("invalid lvalue");
}

void TNodeVisitorCG_GeneratorLValue::visit(TNode_var* node)
{
	int ds=node->flags.get(TNode_var::FLAG_LOCAL)?VAR_LOCAL:VAR_DEFAULT;

	int id=cgen.find_local(node->m_sName,ds);
	if(id<1)
	{
		id=cgen.find_param(node->m_sName);
		if(id>0)
		{
			cgen.emit(XOP_SET_PARAM,id);	
			return;
		}

		id=cgen.find_ggvar(node->m_sName);
		if(id>=0)
		{
			kerror("system variables cannot be lvalue");
			return;
		}

		cgen.emit_set_table(node->m_sName);
		return;
	}

	cgen.emit_set_local(id);

}

void TNodeVisitorCG_GeneratorLValue::visit(TNode_val_nil* node)
{
	cgen.emit(XOP_SPADJ,-1);
}

void TNodeVisitorCG_GeneratorLValue::visit(TNode_variable_list* node)
{
	for(TNode_variable_list::nodelist::reverse_iterator it=node->aList.rbegin();it!=node->aList.rend();++it)
	{
		(*it)->accept(*this);
	}
}

void TNodeVisitorCG_GeneratorLValue::visit(TNode_expression_dot* node)
{
	node->tbl->accept(cgen.rvis);
	cgen.emit_set_index(node->m_sIndex);
}

void TNodeVisitorCG_GeneratorLValue::visit(TNode_expression_arr* node)
{
	cgen.emit(XOP_PROG1);
	node->param->accept(cgen.rvis);
	cgen.pmadj(node->tbl.get(),1);
	cgen.emit(XOP_SET_ARRAY);

}

void TNodeVisitorCG_GeneratorLValue::visit(TNode_expression_list* node)
{
	for(TNode_expression_list::nodelist::reverse_iterator it=node->aList.rbegin();it!=node->aList.rend();++it)
	{
		(*it)->accept(*this);
	}
}


VHWD_LEAVE
