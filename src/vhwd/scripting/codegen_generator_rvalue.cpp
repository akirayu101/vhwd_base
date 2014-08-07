#include "vhwd/scripting/codegen_generator.h"
#include "vhwd/scripting/parser_node.h"
#include "vhwd/scripting/executor.h"
#include "vhwd/scripting/variant_table.h"

VHWD_ENTER


void TNodeVisitorCG_GeneratorRValue::handle(TNode*)
{
	kerror("unknown expected TNode");
}

void TNodeVisitorCG_GeneratorRValue::visit(TNode* node)
{
	handle(node);
}


void TNodeVisitorCG_GeneratorRValue::visit(TNode_variable_list* node)
{
	for(TNode_variable_list::nodelist::iterator it=node->aList.begin();it!=node->aList.end();++it)
	{
		(*it)->accept(*this);
	}
}

void TNodeVisitorCG_GeneratorRValue::visit(TNode_expression*)
{
	cgen.kerror("unknown TNode_expression");
}


void TNodeVisitorCG_GeneratorRValue::visit(TNode_val_nil* node)
{
	cgen.emit(XOP_PUSH_NIL,1);
}


void TNodeVisitorCG_GeneratorRValue::visit(TNode_expression_list* node)
{
	for(TNode_expression_list::nodelist::iterator it=node->aList.begin();it!=node->aList.end();++it)
	{
		(*it)->accept(*this);
	}
}


void TNodeVisitorCG_GeneratorRValue::visit(TNode_item* node)
{
	cgen.kerror("unknown TNode_item");
}


void TNodeVisitorCG_GeneratorRValue::visit(TNode_var* node)
{
	int ds=node->flags.get(TNode_var::FLAG_LOCAL)?VAR_LOCAL:VAR_DEFAULT;

	int id=cgen.find_local(node->m_sName,ds);
	if(id>0)
	{
		cgen.emit_get_local(id);
		return;
	}

	id=cgen.find_param(node->m_sName);
	if(id>0)
	{
		cgen.emit(XOP_GET_PARAM,id);
		return;
	}

	id=cgen.findglobal(node->m_sName);
	if(id>=0)
	{
		cgen.emit_get_table(node->m_sName);
		return;
	}

	id=cgen.find_ggvar(node->m_sName);
	if(id>=0)
	{
		cgen.emit(XOP_GET_GGVAR,id);
		return;
	}

	cgen.addglobal(node->m_sName);
	cgen.emit_get_table(node->m_sName);

}


void TNodeVisitorCG_GeneratorRValue::visit(TNode_val* node)
{
	cgen.kerror("unknown TNode_val");
}


void TNodeVisitorCG_GeneratorRValue::visit(TNode_val_string* node)
{
	cgen.emit_push(node->m_tValue);
}


void TNodeVisitorCG_GeneratorRValue::visit(TNode_val_double* node)
{
	cgen.emit_push(node->m_tValue);
}


void TNodeVisitorCG_GeneratorRValue::visit(TNode_val_integer* node)
{
	cgen.emit(XOP_PUSH_INT_DIRECT,node->m_tValue);
}


void TNodeVisitorCG_GeneratorRValue::visit(TNode_val_boolean* node)
{
	cgen.emit_push(node->m_tValue);
}


void TNodeVisitorCG_GeneratorRValue::visit(TNode_expression_op* node)
{
	cgen.kerror("unknown TNode_expression_op:"+node->m_sOpName);
}


void TNodeVisitorCG_Generator::pmadj(TNode_expression* nd,int pm)
{
	if(nd->pmc()==pm)
	{
		nd->accept(rvis);
	}
	else
	{
		emit(XOP_PROG1);
		nd->accept(rvis);
		emit(XOP_PMADJ,pm);
		emit(XOP_PROG2);
	}
}

void TNodeVisitorCG_GeneratorRValue::visit(TNode_expression_op1* node)
{

	if(node->m_sOpName=="+")
	{
		node->param[0]->accept(*this);
	}
	else if(node->m_sOpName=="-")
	{
		cgen.pmadj(node->param[0].get(),1);
		cgen.emit(XOP1_INV);
	}
	else if(node->m_sOpName=="~")
	{
		cgen.pmadj(node->param[0].get(),1);
		cgen.emit(XOP1_BITWISE_INV);
	}
	else if(node->m_sOpName=="++"||node->m_sOpName=="--")
	{
		if(node->param[0]->pmc()!=1)
		{
			handle(node);
		}
		int xop2=node->m_sOpName=="++"?XOP2_ADD:XOP2_SUB;

		node->param[0]->accept(*this);
		if(node->flags.get(TNode_expression::FLAG_POST))
		{
			cgen.emit(XOP_GET_STACK,0);
			cgen.emit_push(1);
			cgen.emit(xop2);

			node->param[0]->accept(cgen.lvis);
		}
		else
		{
			cgen.emit_push(1);
			cgen.emit(xop2);
			cgen.emit(XOP_GET_STACK,0);	

			node->param[0]->accept(cgen.lvis);
		}

	}
	else if(node->m_sOpName=="!")
	{
		cgen.pmadj(node->param[0].get(),1);
		cgen.emit(XOP1_NOT);
	}
	else
	{
		cgen.kerror("unknown TNode_expression_op1:"+node->m_sOpName);
	}
}

indexer_map<String,int> mp_op2;

void TNodeVisitorCG_GeneratorRValue::visit(TNode_expression_op2* node)
{

	node->param[0]->accept(*this);
	node->param[1]->accept(*this);

	if(mp_op2.size()==0)
	{
		mp_op2["+"]=XOP2_ADD;
		mp_op2["-"]=XOP2_SUB;
		mp_op2["*"]=XOP2_MUL;
		mp_op2["/"]=XOP2_DIV;
		mp_op2["%"]=XOP2_MOD;
		mp_op2["^"]=XOP2_POW;
		mp_op2[".*"]=XOP2_DOT_MUL;
		mp_op2["./"]=XOP2_DOT_DIV;

		mp_op2[">"]=XOP2_GT;
		mp_op2["<"]=XOP2_LT;
		mp_op2[">="]=XOP2_GE;
		mp_op2["<="]=XOP2_LE;
		mp_op2["=="]=XOP2_EQ;
		mp_op2["!="]=XOP2_NE;

		mp_op2["&&"]=XOP2_AND;
		mp_op2["||"]=XOP2_OR;
		mp_op2["^^"]=XOP2_AND;

		mp_op2["&"]=XOP2_BITWISE_AND;
		mp_op2["|"]=XOP2_BITWISE_OR;
		mp_op2["^"]=XOP2_BITWISE_AND;
	}

	int op=mp_op2[node->m_sOpName];
	if(op>0)
	{
		cgen.emit(op);
	}
	else
	{
		cgen.kerror("unknown TNode_expression_op2:"+node->m_sOpName);
	}
}


void TNodeVisitorCG_GeneratorRValue::visit(TNode_expression_op_assign* node)
{

	TNode_expression_op_assign* aval=dynamic_cast<TNode_expression_op_assign* >(node->param[0].get());
	TNode_expression* lval=node->param[0].get();
	TNode_expression* rval=node->param[1].get();

	if(aval)
	{
		lval=aval->param[1].get();
	}

	int n1=lval->pmc();
	if(n1<0)
	{
		cgen.kerror("invalid lvalue");
		return;
	}

	int n2=rval->pmc();

	if(n2<0)
	{
		cgen.emit(XOP_PROG1);
	}

	rval->accept(*this);

	if(n2<0)
	{
		cgen.emit(XOP_PMADJ,n1);
	}
	else if(n1>n2)
	{
		cgen.emit(XOP_PUSH_NIL,n1-n2);
	}
	else if(n1<n2)
	{
		cgen.emit(XOP_SPADJ,n1-n2);
	}

	{
		lval->accept(cgen.lvis);
	}

	if(n2<0)
	{
		cgen.emit(XOP_PROG2);
	}

	if(aval)
	{
		aval->accept(*this);
	}
}


void TNodeVisitorCG_GeneratorRValue::visit(TNode_expression_call* node)
{
	if(node->cls)
	{
		cgen.emit(XOP_PROG1);
		node->param->accept(*this);
		node->tbl->accept(*this);
		cgen.emit(XOP_PROG1);
		node->cls->accept(*this);
		cgen.emit(XOP_CALL_THIS);
	}
	else
	{
		cgen.emit(XOP_PROG1);
		node->param->accept(*this);
		node->tbl->accept(*this);
		cgen.emit(XOP_CALL);
	}
}


void TNodeVisitorCG_GeneratorRValue::visit(TNode_expression_dot* node)
{
	node->tbl->accept(*this);
	cgen.emit_get_index(node->m_sIndex);

}


void TNodeVisitorCG_GeneratorRValue::visit(TNode_expression_arr* node)
{
	cgen.emit(XOP_PROG1);
	node->param->accept(*this);
	cgen.pmadj(node->tbl.get(),1);
	cgen.emit(XOP_GET_ARRAY);
}



void TNodeVisitorCG_GeneratorRValue::visit(TNode_statement*)
{
	cgen.kerror("unknown TNode_statement");
}


void TNodeVisitorCG_GeneratorRValue::visit(TNode_statement_nil*)
{	

}


void TNodeVisitorCG_GeneratorRValue::visit(TNode_statement_assignment* node)
{
	node->value->accept(*this);
}


void TNodeVisitorCG_GeneratorRValue::visit(TNode_statement_if* node)
{
	node->expr_cond->accept(*this);
	size_t id1=cgen.aInsts.size();
	cgen.emit(XOP_JUMP_FALSE,-1);
	node->stmt_if_1->accept(*this);
	size_t id2=cgen.aInsts.size();
	cgen.emit(XOP_JUMP,-1);
	cgen.aInsts[id1].parm=cgen.aInsts.size();

	if(node->stmt_if_0)
	{
		node->stmt_if_0->accept(*this);
	}

	cgen.aInsts[id2].parm=cgen.aInsts.size();

}

void TNodeVisitorCG_GeneratorRValue::visit(TNode_statement_control* node)
{
	size_t p=cgen.aInsts.size();
	cgen.emit(XOP_JUMP,0);

	arr_1t<syn_kloop_jmp>& aloop1(cgen.ascope.GetData()->aLoop1);
	arr_1t<syn_kloop_jmp>& aloop2(cgen.ascope.GetData()->aLoop2);

	if(node->m_sName=="break")
	{
		if(aloop1.empty())
		{
			cgen.kerror("invalid break");
		}
		aloop1.back().push_back(p);
	}
	else if(node->m_sName=="continue")
	{
		if(aloop2.empty())
		{
			cgen.kerror("invalid continue");
		}
		aloop2.back().push_back(p);
	}
	else if(node->m_sName=="continue2")
	{
		if(aloop2.size()<2)
		{
			cgen.kerror("invalid continue2");
		}
		(*(aloop2.rbegin()+1)).push_back(p);
	}
	else if(node->m_sName=="break2")
	{
		if(aloop1.size()<2)
		{
			cgen.kerror("invalid break2;");
		}
		(*(aloop1.rbegin()+1)).push_back(p);
	}
	else if(node->m_sName=="break2")
	{
		if(aloop1.size()<3)
		{
			cgen.kerror("invalid break3");
		}
		(*(aloop1.rbegin()+2)).push_back(p);
	}
	else
	{
		cgen.kerror("unknown TNode_statement_control:"+node->m_sName);
	}


}
void TNodeVisitorCG_GeneratorRValue::visit(TNode_statement_loop* node)
{

	LockGuard<syn_param,LP_block> lock1(cgen.ascope);

	if(node->m_sName=="for_each")
	{
		int id=cgen.add_local("#iterator");
		for(TNode_variable_list::nodelist::iterator it=node->fe_variable->aList.begin();it!=node->fe_variable->aList.end();++it)
		{
			cgen.add_local((*it)->m_sName);
		}
		cgen.emit(XOP_PROG1);
		node->fe_container->accept(*this);
		cgen.emit_push((int32_t)node->fe_variable->aList.size());
		cgen.emit_get_ggvar("get_iterator");
		cgen.emit(XOP_CALL);
		cgen.emit_set_local(id);

		size_t jp_next=cgen.aInsts.size();

		cgen.emit(XOP_PROG1);
		cgen.emit_get_local(id);
		cgen.emit(XOP_CALL);

		LockGuard<syn_param,LP_loop1> lock2(cgen.ascope);
		LockGuard<syn_param,LP_loop2> lock3(cgen.ascope);

		cgen.ascope.add_loop1(cgen.aInsts.size());

		cgen.emit(XOP_JUMP_FALSE,0); // jump exit
		node->fe_variable->accept(cgen.lvis);
		node->stmt_body->accept(*this);
		cgen.emit(XOP_JUMP,jp_next); // jump next

		cgen.ascope.set_loop1(cgen.aInsts.size());
		cgen.ascope.set_loop2(jp_next);

		return;
	}

	if(node->stmt_init)
	{
		node->stmt_init->accept(*this);
	}

	LockGuard<syn_param,LP_loop1> lock2(cgen.ascope);
	LockGuard<syn_param,LP_loop2> lock3(cgen.ascope);

	if(node->m_sName!="do")
	{
		cgen.emit(XOP_JUMP,0);
	}

	size_t j1=cgen.aInsts.size();

	node->stmt_body->accept(*this);

	size_t jc=cgen.aInsts.size();

	if(node->stmt_fini)
	{
		node->stmt_fini->accept(*this);
	}

	if(node->m_sName!="do")
	{
		cgen.aInsts[j1-1].parm=cgen.aInsts.size();
	}

	if(node->expr_cond)
	{
		node->expr_cond->accept(*this);
		cgen.emit(XOP_JUMP_TRUE,j1);
	}
	else
	{
		cgen.emit(XOP_JUMP,j1);
	}

	cgen.ascope.set_loop1(cgen.aInsts.size());
	cgen.ascope.set_loop2(jc);

}

void TNodeVisitorCG_GeneratorRValue::visit(TNode_statement_list* node)
{
	for(TNode_statement_list::nodelist::iterator it=node->aList.begin();it!=node->aList.end();++it)
	{
		(*it)->accept(*this);
	}
}


void TNodeVisitorCG_GeneratorRValue::visit(TNode_statement_block* node)
{
	LockGuard<syn_param,LP_block> lock2(cgen.ascope);
	visit_base(node);
}

void TNodeVisitorCG_GeneratorRValue::visit(TNode_statement_return* node)
{
	cgen.emit(XOP_PROG1);
	node->value->accept(*this);
	cgen.emit(XOP_RETURN);
}


void TNodeVisitorCG_GeneratorRValue::visit(TNode_val_class* node)
{
	LockGuard<syn_param,LP_block> lock2(cgen.ascope);

	int id1=cgen.add_local("member");
	int id2=cgen.add_local("static");

	cgen.emit(XOP_PROG1);
	cgen.emit_get_ggvar("table");
	cgen.emit(XOP_CALL);
	cgen.emit_set_local(id1);
	cgen.emit(XOP_PROG1);
	cgen.emit_get_ggvar("table");
	cgen.emit(XOP_CALL);
	cgen.emit_set_local(id2);		
		
	visit((TNode_statement_list*)node->func_body.get());

	cgen.emit(XOP_PROG1);
	cgen.emit_get_local(id1);
	cgen.emit_get_local(id2);
	cgen.emit_get_ggvar("metatable");
	cgen.emit(XOP_CALL);

}


class CallableSwitch : public CallableData
{
public:
	indexer_map<Variant,int64_t> jpm;
	int64_t jpd;

	CallableSwitch()
	{
		jpd=-1;
	}

	bool AddCase(Variant& v,int64_t p)
	{
		if(jpm.find(v)<0)
		{
			jpm[v]=p;
			return true;
		}
		else
		{
			return false;
		}
	}

	bool AddDefault(int64_t p)
	{
		if(jpd!=-1) return false;
		jpd=p;
		return true;
	}

	int __fun_call(Executor& ks)
	{
		int id=jpm.find(ks.top());
		if(id<0)
		{
			ks.top().reset<int64_t>(jpd);
		}
		else
		{
			ks.top().reset<int64_t>(jpm.get(id).second);
		}
		return 1;
	}

};


void TNodeVisitorCG_GeneratorRValue::visit(TNode_statement_switch* node)
{
	LockGuard<syn_param,LP_block> lock1(cgen.ascope);

	DataPtrT<CallableSwitch> pCaswitch(new CallableSwitch);

	cgen.emit(XOP_PROG1);
	node->expr_cond->accept(*this);
	cgen.emit(XOP_PMADJ,1);
	cgen.emit_push(pCaswitch.get());
	cgen.emit(XOP_CALL);	
	cgen.emit(XOP_JUMP_N);

	LockGuard<syn_param,LP_loop1> lock2(cgen.ascope);

	for(TNode_statement_switch::case_iterator it1=node->case_list.begin();it1!=node->case_list.end();++it1)
	{
		Variant v;
		int n=(*it1)->case_value.size();
		if(n>0)
		{
			for(TNode_statement_switch::item_iterator it2=(*it1)->case_value.begin();it2!=(*it1)->case_value.end();++it2)
			{
				TNode_item* p=(*it2).get();
				if(!p->get(v))
				{
					kerror("case must be constant");					
				}
				if(!pCaswitch->AddCase(v,cgen.aInsts.size()))
				{
					kerror("case already exists");
				}
			}

		}
		else
		{
			if(!pCaswitch->AddDefault(cgen.aInsts.size()))
			{
				kerror("multiple default cases");
			}
		}
		
		(*it1)->case_stmt->accept(*this);

		cgen.ascope.add_loop1(cgen.aInsts.size());
		cgen.emit(XOP_JUMP);
	}

	size_t jp_exit=cgen.aInsts.size();
	cgen.ascope.set_loop1(jp_exit);
	pCaswitch->AddDefault(jp_exit);

}

void TNodeVisitorCG_GeneratorRValue::visit(TNode_val_function* node)
{
	DataPtrT<CallableCode> func;
	{
		LockGuard<syn_param,LP_param> lock2(cgen.ascope);

		func.reset(cgen.ascope.GetData()->pCode.get());

		if(node->flags.get(TNode_val_function::FLAG_WITH_THIS))
		{
			cgen.pCode->npThis=cgen.add_local("this");
		}

		for(TNode_variable_list::nodelist::iterator it=node->parm_list->aList.begin();it!=node->parm_list->aList.end();++it)
		{
			cgen.add_param((*it)->m_sName);
		}

		visit((TNode_statement_list*)node->func_body.get());
	}

	cgen.emit_push(func.get());

}


VHWD_LEAVE
