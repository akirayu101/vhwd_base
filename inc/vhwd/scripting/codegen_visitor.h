#ifndef __H_CODEGEN_VISITOR__
#define __H_CODEGEN_VISITOR__

#include "vhwd/scripting/parser_visitor.h"
#include "vhwd/scripting/codegen.h"


VHWD_ENTER


class TNodeVisitorCG : public TNodeVisitorRecursive
{
public:
	typedef TNodeVisitorRecursive basetype;
	CodeGen& cg;
	TNodeVisitorCG(CodeGen& g):cg(g){}

	virtual void kerror(const String& s);

	virtual void parse(TNode* q){q->accept(*this);}

};

class TNodeVisitorCG_Analyzer : public TNodeVisitorCG
{
public:
	typedef TNodeVisitorCG basetype;

	TNodeVisitorCG_Analyzer(CodeGen& g);

	using basetype::visit;

	virtual void visit(TNode*);
	virtual void visit(TNode_var*);
	virtual void visit(TNode_val_string*);
	virtual void visit(TNode_val_double*);
	virtual void visit(TNode_val_integer*);
	virtual void visit(TNode_val_boolean*);
	virtual void visit(TNode_expression_op_assign*);
	virtual void visit(TNode_val_class*);
	virtual void visit(TNode_val_function*);

	void kerror(const String& s);

protected:
	int n_this_count;
};




VHWD_LEAVE
#endif
