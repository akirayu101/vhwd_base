#include "vhwd/scripting/codegen_visitor.h"
#include "vhwd/scripting/parser_node.h"
#include "vhwd/scripting/executor.h"

VHWD_ENTER



void TNodeVisitorCG_Analyzer::visit(TNode* node)
{
	(void)node;
}


void TNodeVisitorCG_Analyzer::visit(TNode_var* node)
{
	if(node->m_sName=="this")
	{
		n_this_count++;
	}
}


void TNodeVisitorCG_Analyzer::visit(TNode_val_string* node)
{
	node->m_tValue=node->m_sValue;
}

void TNodeVisitorCG_Analyzer::visit(TNode_val_double* node)
{
	if(!node->m_sValue.ToNumber(&node->m_tValue))
	{
		kerror("not number");
	}
}


void TNodeVisitorCG_Analyzer::visit(TNode_val_integer* node)
{
	if(!node->m_sValue.ToNumber(&node->m_tValue))
	{
		kerror("not number");
	}
}


void TNodeVisitorCG_Analyzer::visit(TNode_val_boolean* node)
{
	node->m_tValue=node->m_sValue.c_str()[0]=='t'||node->m_sValue.c_str()[0]=='T';
}



void TNodeVisitorCG_Analyzer::visit(TNode_expression_op_assign* node)
{

	if(node->m_sOpName.size()>1)
	{

		DataPtrT<TNode_expression_op2> n(new TNode_expression_op2);
		n->param[0]=node->param[0];
		n->param[1]=node->param[1];
		n->m_sOpName=node->m_sOpName.substr(0,node->m_sOpName.size()-1);
		node->m_sOpName="=";
		node->param[1]=n;
	}	

	node->param[1]->accept(*this);
	node->param[0]->accept(*this);

}





void TNodeVisitorCG_Analyzer::visit(TNode_val_function* node)
{
	if(node->m_sName!="")
	{
		kerror("local function with a name?!");
	}	

	LockState<int> lock(n_this_count,0);

	node->func_body->accept(*this);

	if(n_this_count>0)
	{
		node->flags.add(TNode_val_function::FLAG_WITH_THIS);
	}
}


void TNodeVisitorCG_Analyzer::visit(TNode_val_class* node)
{
	int n=n_this_count;	

	node->func_body->accept(*this);

	if(n_this_count>0)
	{
		node->flags.add(TNode_val_function::FLAG_WITH_THIS);
	}
	n_this_count=n;
}


TNodeVisitorCG_Analyzer::TNodeVisitorCG_Analyzer(CodeGen& g):TNodeVisitorCG(g)
{
	n_this_count=0;
}

void TNodeVisitorCG_Analyzer::kerror(const String& s)
{
	Exception::XError(String::Format("CodeGen:Analyzer:%s",s));
}

VHWD_LEAVE

