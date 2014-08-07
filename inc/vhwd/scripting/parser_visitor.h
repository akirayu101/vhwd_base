#ifndef __H_PARSER_VISITOR__
#define __H_PARSER_VISITOR__

#include "vhwd/basic.h"

VHWD_ENTER

class VHWD_DLLIMPEXP TNode;

class VHWD_DLLIMPEXP TNode_expression;
class VHWD_DLLIMPEXP TNode_variable_list;
class VHWD_DLLIMPEXP TNode_expression_list;
class VHWD_DLLIMPEXP TNode_item;
class VHWD_DLLIMPEXP TNode_var;
class VHWD_DLLIMPEXP TNode_val;
class VHWD_DLLIMPEXP TNode_val_nil;
class VHWD_DLLIMPEXP TNode_val_string;
class VHWD_DLLIMPEXP TNode_val_double;
class VHWD_DLLIMPEXP TNode_val_integer;
class VHWD_DLLIMPEXP TNode_val_boolean;
class VHWD_DLLIMPEXP TNode_val_function;
class VHWD_DLLIMPEXP TNode_val_class;

class VHWD_DLLIMPEXP TNode_expression_op;
class VHWD_DLLIMPEXP TNode_expression_op1;
class VHWD_DLLIMPEXP TNode_expression_op2;
class VHWD_DLLIMPEXP TNode_expression_op_assign;
class VHWD_DLLIMPEXP TNode_expression_call;
class VHWD_DLLIMPEXP TNode_expression_dot;
class VHWD_DLLIMPEXP TNode_expression_arr;
class VHWD_DLLIMPEXP TNode_statement;
class VHWD_DLLIMPEXP TNode_statement_nil;
class VHWD_DLLIMPEXP TNode_statement_assignment;
class VHWD_DLLIMPEXP TNode_statement_if;
class VHWD_DLLIMPEXP TNode_statement_loop;

class VHWD_DLLIMPEXP TNode_statement_list;
class VHWD_DLLIMPEXP TNode_statement_block;
class VHWD_DLLIMPEXP TNode_statement_switch;

class VHWD_DLLIMPEXP TNode_statement_control;
class VHWD_DLLIMPEXP TNode_statement_return;

// for future use

class VHWD_DLLIMPEXP TNode_extern1;
class VHWD_DLLIMPEXP TNode_extern2;
class VHWD_DLLIMPEXP TNode_extern3;
class VHWD_DLLIMPEXP TNode_extern4;
class VHWD_DLLIMPEXP TNode_extern5;
class VHWD_DLLIMPEXP TNode_extern6;
class VHWD_DLLIMPEXP TNode_extern7;



class VHWD_DLLIMPEXP TNodeVisitor : public Object
{
public:
	virtual void parse(TNode*){}

	virtual void handle(TNode*);

	virtual void visit(TNode*){}
	virtual void visit(TNode_variable_list*){}

	virtual void visit(TNode_expression*){}
	virtual void visit(TNode_val_nil*){}
	virtual void visit(TNode_expression_list*){}
	virtual void visit(TNode_item*){}
	virtual void visit(TNode_var*){}
	virtual void visit(TNode_val*){}
	virtual void visit(TNode_val_string*){}
	virtual void visit(TNode_val_double*){}
	virtual void visit(TNode_val_integer*){}
	virtual void visit(TNode_val_boolean*){}
	virtual void visit(TNode_val_function*){}
	virtual void visit(TNode_val_class*){}

	virtual void visit(TNode_expression_op*){}
	virtual void visit(TNode_expression_op1*){}
	virtual void visit(TNode_expression_op2*){}
	virtual void visit(TNode_expression_op_assign*){}
	virtual void visit(TNode_expression_call*){}
	virtual void visit(TNode_expression_dot*){}
	virtual void visit(TNode_expression_arr*){}

	virtual void visit(TNode_statement*){}
	virtual void visit(TNode_statement_nil*){}
	virtual void visit(TNode_statement_assignment*){}
	virtual void visit(TNode_statement_if*){}
	virtual void visit(TNode_statement_loop*){}

	virtual void visit(TNode_statement_list*){}
	virtual void visit(TNode_statement_block*){}
	virtual void visit(TNode_statement_control*){}
	virtual void visit(TNode_statement_return*){}
	virtual void visit(TNode_statement_switch*){}


	virtual void visit(TNode_extern1*){}
	virtual void visit(TNode_extern2*){}
	virtual void visit(TNode_extern3*){}
	virtual void visit(TNode_extern4*){}
	virtual void visit(TNode_extern5*){}
	virtual void visit(TNode_extern6*){}
	virtual void visit(TNode_extern7*){}


};


class VHWD_DLLIMPEXP TNodeVisitorVisitBase : public TNodeVisitor
{
public:


	template<typename T>
	void visit_base(T* v){visit((typename T::basetype*)v);}

	virtual void visit(TNode*);
	virtual void visit(TNode_variable_list*);

	virtual void visit(TNode_expression*);
	virtual void visit(TNode_val_nil*);
	virtual void visit(TNode_expression_list*);
	virtual void visit(TNode_item*);
	virtual void visit(TNode_var*);
	virtual void visit(TNode_val*);
	virtual void visit(TNode_val_string*);
	virtual void visit(TNode_val_double*);
	virtual void visit(TNode_val_integer*);
	virtual void visit(TNode_val_boolean*);
	virtual void visit(TNode_expression_op*);
	virtual void visit(TNode_expression_op1*);
	virtual void visit(TNode_expression_op2*);
	virtual void visit(TNode_expression_op_assign*);
	virtual void visit(TNode_expression_call*);
	virtual void visit(TNode_expression_dot*);
	virtual void visit(TNode_expression_arr*);

	virtual void visit(TNode_statement*);
	virtual void visit(TNode_statement_nil*);
	virtual void visit(TNode_statement_assignment*);
	virtual void visit(TNode_statement_if*);
	virtual void visit(TNode_statement_loop*);

	virtual void visit(TNode_statement_list*);
	virtual void visit(TNode_statement_block*);
	virtual void visit(TNode_val_function*);

	virtual void visit(TNode_val_class*);
	virtual void visit(TNode_statement_control*);
	virtual void visit(TNode_statement_return*);

	virtual void visit(TNode_extern1*){}
	virtual void visit(TNode_extern2*){}
	virtual void visit(TNode_extern3*){}
	virtual void visit(TNode_extern4*){}
	virtual void visit(TNode_extern5*){}
	virtual void visit(TNode_extern6*){}
	virtual void visit(TNode_extern7*){}
	virtual void visit(TNode_statement_switch*){}


};

class VHWD_DLLIMPEXP TNodeVisitorRecursive : public TNodeVisitorVisitBase
{
public:

	virtual void visit(TNode_variable_list*);
	virtual void visit(TNode_expression_list*);
	virtual void visit(TNode_expression_op*);
	virtual void visit(TNode_expression_op1*);
	virtual void visit(TNode_expression_op2*);

	virtual void visit(TNode_expression_call*);
	virtual void visit(TNode_expression_dot*);
	virtual void visit(TNode_expression_arr*);

	virtual void visit(TNode_statement_assignment*);
	virtual void visit(TNode_statement_if*);
	virtual void visit(TNode_statement_loop*);

	virtual void visit(TNode_statement_list*);
	virtual void visit(TNode_val_function*);

	virtual void visit(TNode_val_class*);
	virtual void visit(TNode_statement_return*);

	virtual void visit(TNode_statement_switch* node);

};

class VHWD_DLLIMPEXP TNodeVisitorPrint : public TNodeVisitorVisitBase
{
public:

	virtual void visit(TNode_expression_list*);
	virtual void visit(TNode_variable_list*);
	virtual void visit(TNode_var*);
	virtual void visit(TNode_val*);
	virtual void visit(TNode_expression_op1*);
	virtual void visit(TNode_expression_op2*);
	virtual void visit(TNode_expression_op_assign*);
	virtual void visit(TNode_statement_assignment*);
	virtual void visit(TNode_statement_if*);
	virtual void visit(TNode_statement_loop*);
	virtual void visit(TNode_statement_nil*);

	virtual void visit(TNode_statement_list*);
	virtual void visit(TNode_statement_block*);
	virtual void visit(TNode_val_function*);
	virtual void visit(TNode_expression_call*);
	virtual void visit(TNode_expression_dot*);
	virtual void visit(TNode_expression_arr*);

	virtual void visit(TNode_val_class*);
	virtual void visit(TNode_statement_control*){}
	virtual void visit(TNode_statement_return*){}

	virtual void visit(TNode_extern1*){}
	virtual void visit(TNode_extern2*){}
	virtual void visit(TNode_extern3*){}
	virtual void visit(TNode_extern4*){}
	virtual void visit(TNode_extern5*){}
	virtual void visit(TNode_extern6*){}
	virtual void visit(TNode_extern7*){}
	virtual void visit(TNode_statement_switch*){}


};



VHWD_LEAVE
#endif
