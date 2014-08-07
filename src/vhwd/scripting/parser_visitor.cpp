#include "vhwd/scripting/parser.h"
#include "vhwd/scripting/parser_node.h"

VHWD_ENTER

void TNodeVisitor::handle(TNode* q)
{
	
}

void TNodeVisitorVisitBase::visit(TNode* node)
{
	handle(node);
}

void TNodeVisitorVisitBase::visit(TNode_variable_list* node){visit_base(node);}
void TNodeVisitorVisitBase::visit(TNode_expression* node){visit_base(node);}
void TNodeVisitorVisitBase::visit(TNode_val_nil* node){visit_base(node);}
void TNodeVisitorVisitBase::visit(TNode_expression_list* node){visit_base(node);}
void TNodeVisitorVisitBase::visit(TNode_item* node){visit_base(node);}
void TNodeVisitorVisitBase::visit(TNode_var* node){visit_base(node);}
void TNodeVisitorVisitBase::visit(TNode_val* node){visit_base(node);}
void TNodeVisitorVisitBase::visit(TNode_val_string* node){visit_base(node);}
void TNodeVisitorVisitBase::visit(TNode_val_double* node){visit_base(node);}
void TNodeVisitorVisitBase::visit(TNode_val_integer* node){visit_base(node);}
void TNodeVisitorVisitBase::visit(TNode_val_boolean* node){visit_base(node);}
void TNodeVisitorVisitBase::visit(TNode_expression_op* node){visit_base(node);}
void TNodeVisitorVisitBase::visit(TNode_expression_op1* node){visit_base(node);}
void TNodeVisitorVisitBase::visit(TNode_expression_op2* node){visit_base(node);}
void TNodeVisitorVisitBase::visit(TNode_expression_op_assign* node){visit_base(node);}
void TNodeVisitorVisitBase::visit(TNode_expression_call* node){visit_base(node);}
void TNodeVisitorVisitBase::visit(TNode_expression_dot* node){visit_base(node);}
void TNodeVisitorVisitBase::visit(TNode_expression_arr* node){visit_base(node);}
void TNodeVisitorVisitBase::visit(TNode_statement* node){visit_base(node);}
void TNodeVisitorVisitBase::visit(TNode_statement_nil* node){visit_base(node);}
void TNodeVisitorVisitBase::visit(TNode_statement_assignment* node){visit_base(node);}
void TNodeVisitorVisitBase::visit(TNode_statement_if* node){visit_base(node);}
void TNodeVisitorVisitBase::visit(TNode_statement_loop* node){visit_base(node);}
void TNodeVisitorVisitBase::visit(TNode_val_class* node){visit_base(node);}
void TNodeVisitorVisitBase::visit(TNode_statement_list* node){visit_base(node);}
void TNodeVisitorVisitBase::visit(TNode_statement_block* node){visit_base(node);}
void TNodeVisitorVisitBase::visit(TNode_val_function* node){visit_base(node);}
void TNodeVisitorVisitBase::visit(TNode_statement_control* node){visit_base(node);}
void TNodeVisitorVisitBase::visit(TNode_statement_return* node){visit_base(node);}

void TNodeVisitorPrint::visit(TNode_expression_list*node)
{
	for(TNode_expression_list::nodelist::iterator it=node->aList.begin();it!=node->aList.end();)
	{
		(*it)->accept(*this);++it;
		if(it!=node->aList.end()) Console::Write(",");
	}
}

void TNodeVisitorPrint::visit(TNode_variable_list* node)
{
	for(TNode_variable_list::nodelist::iterator it=node->aList.begin();it!=node->aList.end();)
	{
		(*it)->accept(*this);++it;
		if(it!=node->aList.end()) Console::Write(",");
	}
}

void TNodeVisitorPrint::visit(TNode_var* node)
{
	Console::Write(node->m_sName);
}

void TNodeVisitorPrint::visit(TNode_val* node)
{
	Console::Write(node->m_sValue);
}


void TNodeVisitorPrint::visit(TNode_expression_op1* node)
{
	if(node->flags.get(TNode_expression::FLAG_WITH_BRAKET))
	{
		Console::Write("(");
	}
	Console::Write(node->m_sOpName);
	node->getp(0)->accept(*this);
	if(node->flags.get(TNode_expression::FLAG_WITH_BRAKET))
	{
		Console::Write(")");
	}
}

void TNodeVisitorPrint::visit(TNode_expression_op2* node)
{
	if(node->flags.get(TNode_expression::FLAG_WITH_BRAKET))
	{
		Console::Write("(");
	}
	node->getp(0)->accept(*this);
	Console::Write(node->m_sOpName);
	node->getp(1)->accept(*this);
	if(node->flags.get(TNode_expression::FLAG_WITH_BRAKET))
	{
		Console::Write(")");
	}
}

void TNodeVisitorPrint::visit(TNode_expression_op_assign* node)
{
	node->getp(0)->accept(*this);
	Console::Write(node->m_sOpName);
	node->getp(1)->accept(*this);
}

void TNodeVisitorPrint::visit(TNode_statement_assignment*node)
{
	node->value->accept(*this);
}

void TNodeVisitorPrint::visit(TNode_statement_if*node)
{
	Console::Write("if(");
	node->expr_cond->accept(*this);
	Console::WriteLine(")");
	node->stmt_if_1->accept(*this);
	if(node->stmt_if_0)
	{
		Console::WriteLine("else");
		node->stmt_if_0->accept(*this);
	}

}
void TNodeVisitorPrint::visit(TNode_statement_loop* node)
{
	if(node->m_sName=="while")
	{
		Console::Write("while(");
		node->expr_cond->accept(*this);
		Console::WriteLine(")");

		node->stmt_body->accept(*this);
	
	}
	else if(node->m_sName=="do")
	{
		Console::WriteLine("do");
		node->stmt_body->accept(*this);
		Console::Write("while(");
		node->expr_cond->accept(*this);
		Console::WriteLine(")");
	}
	else
	{
		Console::Write("for(");
		node->stmt_init->accept(*this);
		Console::Write(";");
		node->expr_cond->accept(*this);
		Console::Write(";");
		node->stmt_fini->accept(*this);
		Console::WriteLine(")");

		node->stmt_body->accept(*this);
	}

}
void TNodeVisitorPrint::visit(TNode_val_class*node)
{
	
}

void TNodeVisitorPrint::visit(TNode_statement_nil*)
{
	Console::Write(";");
}


void TNodeVisitorPrint::visit(TNode_statement_list*node)
{
	for(TNode_statement_list::nodelist::iterator it=node->aList.begin();it!=node->aList.end();++it)
	{
		(*it)->accept(*this);
		Console::WriteLine("");
	}
}

void TNodeVisitorPrint::visit(TNode_statement_block*node)
{
	Console::WriteLine("{");
	TNodeVisitorPrint::visit((TNode_statement_list*)node);
	Console::WriteLine("}");
}

void TNodeVisitorPrint::visit(TNode_val_function*node)
{
	Console::Write("function ");
	Console::Write(node->m_sName);
	Console::Write("(");
	node->parm_list->accept(*this);
	Console::WriteLine(")");
	visit_base(node);
	//node->func_body->accept(*this);
}

void TNodeVisitorPrint::visit(TNode_expression_call* node)
{
	node->tbl->accept(*this);
	Console::Write("(");
	node->param->accept(*this);
	Console::Write(")");
}

void TNodeVisitorPrint::visit(TNode_expression_dot* node)
{
	node->tbl->accept(*this);
	Console::Write(".");
	Console::Write(node->m_sIndex);
}

void TNodeVisitorPrint::visit(TNode_expression_arr* node)
{
	node->tbl->accept(*this);
	Console::Write("[");
	node->param->accept(*this);
	Console::Write("]");
}



void TNodeVisitorRecursive::visit(TNode_variable_list* node)
{
	for(TNode_variable_list::nodelist::iterator it=node->aList.begin();it!=node->aList.end();++it)
	{
		(*it)->accept(*this);
	}
}

void TNodeVisitorRecursive::visit(TNode_expression_list* node)
{
	for(TNode_expression_list::nodelist::iterator it=node->aList.begin();it!=node->aList.end();++it)
	{
		(*it)->accept(*this);
	}
}

void TNodeVisitorRecursive::visit(TNode_expression_op* node)
{
	int s=node->size();
	for(int i=0;i<s;i++)
	{
		node->getp(s)->accept(*this);
	}
}

void TNodeVisitorRecursive::visit(TNode_expression_op1* node)
{
	node->param[0]->accept(*this);
}

void TNodeVisitorRecursive::visit(TNode_expression_op2* node)
{
	node->param[1]->accept(*this);
	node->param[0]->accept(*this);
}


void TNodeVisitorRecursive::visit(TNode_expression_call* node)
{

	if(node->cls)
	{
		node->cls->accept(*this);
	}

	node->param->accept(*this);
	node->tbl->accept(*this);
}

void TNodeVisitorRecursive::visit(TNode_expression_dot* node)
{
	node->tbl->accept(*this);
}

void TNodeVisitorRecursive::visit(TNode_expression_arr* node)
{
	node->tbl->accept(*this);
	node->param->accept(*this);
}


void TNodeVisitorRecursive::visit(TNode_statement_assignment* node)
{
	node->value->accept(*this);
}

void TNodeVisitorRecursive::visit(TNode_statement_if* node)
{
	node->expr_cond->accept(*this);
	if(node->stmt_if_1)
	{
		node->stmt_if_1->accept(*this);
	}
	if(node->stmt_if_0)
	{
		node->stmt_if_0->accept(*this);
	}
}

void TNodeVisitorRecursive::visit(TNode_statement_loop* node)
{
	if(node->stmt_init)
	{
		node->stmt_init->accept(*this);
	}
	if(node->expr_cond)
	{
		node->expr_cond->accept(*this);
	}
	if(node->stmt_fini)
	{
		node->stmt_fini->accept(*this);
	}
	if(node->stmt_body)
	{
		node->stmt_body->accept(*this);
	}
	if(node->fe_container)
	{
		node->fe_container->accept(*this);
	}
}


void TNodeVisitorRecursive::visit(TNode_statement_list* node)
{
	for(TNode_statement_list::nodelist::iterator it=node->aList.begin();it!=node->aList.end();++it)
	{
		(*it)->accept(*this);
	}
}

void TNodeVisitorRecursive::visit(TNode_val_function* node)
{
	node->parm_list->accept(*this);
	node->func_body->accept(*this);
}


void TNodeVisitorRecursive::visit(TNode_val_class* node)
{
	visit((TNode_statement_list*)node->func_body.get());
}

void TNodeVisitorRecursive::visit(TNode_statement_return* node)
{
	node->value->accept(*this);
}

void TNodeVisitorRecursive::visit(TNode_statement_switch* node)
{
	node->expr_cond->accept(*this);
	for(TNode_statement_switch::case_iterator it1=node->case_list.begin();it1!=node->case_list.end();++it1)
	{
		for(TNode_statement_switch::item_iterator it2=(*it1)->case_value.begin();
			it2!=(*it1)->case_value.end();++it2)
		{
			(*it2)->accept(*this);
		}

		(*it1)->case_stmt->accept(*this);
	}
}

VHWD_LEAVE
