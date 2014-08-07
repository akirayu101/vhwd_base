#ifndef __H_CODEGEN_GENERATOR__
#define __H_CODEGEN_GENERATOR__

#include "vhwd/scripting/codegen_visitor.h"

VHWD_ENTER


class TNodeVisitorCG_Generator;

class syn_scope_base : public ObjectData
{
public:
	indexer_set<String> variables;
};

class syn_scope_block : public syn_scope_base
{
public:
	syn_scope_block(int ln=0){local_num=ln;}
	int GetLocalNum(){return local_num+variables.size();}
	int local_num;
};


class syn_kloop_jmp : public arr_1t<int>
{
public:
	
};

class syn_scope_function : public syn_scope_base
{
public:

	syn_scope_function(TNodeVisitorCG_Generator& cg_):cgen(cg_)
	{
		pCode.reset(new CallableCode);
		aBlock.push_back(new syn_scope_block(0));
	}

	indexer_set<Variant> aConst;
	arr_1t<kvar_inst> aInsts;
	arr_1t<syn_kloop_jmp> aLoop1;
	arr_1t<syn_kloop_jmp> aLoop2;
	arr_1t<DataPtrT<syn_scope_block> > aBlock;
	DataPtrT<CallableCode> pCode;

	TNodeVisitorCG_Generator& cgen;

};

class syn_global
{
public:
	indexer_set<String> variables;
};

class syn_param : public ObjectT<syn_scope_function>
{
public:
	syn_param(TNodeVisitorCG_Generator& cg_):cgen(cg_){}
	arr_1t<DataPtrT<syn_scope_function> > stk;

	void lock_scope();
	void unlock_scope();

	void lock_block();
	void unlock_block();

	void lock_loop1();
	void unlock_loop1();

	void lock_loop2();
	void unlock_loop2();

	indexer_set<String>& local_variables()
	{
		return m_refData->aBlock.back()->variables;
	}

	void add_loop1(size_t n)
	{
		m_refData->aLoop1.back().push_back(n);
	}

	void add_loop2(size_t n)
	{
		m_refData->aLoop2.back().push_back(n);
	}

	void set_loop1(size_t n);
	void set_loop2(size_t n);

	TNodeVisitorCG_Generator& cgen;

};


class LP_param
{
public:
	static void lock(syn_param& mtx){mtx.lock_scope();}
	static void unlock(syn_param& mtx){	mtx.unlock_scope();}
};

class LP_block
{
public:
	static void lock(syn_param& mtx){mtx.lock_block();}
	static void unlock(syn_param& mtx){	mtx.unlock_block();}
};

class LP_loop1
{
public:
	static void lock(syn_param& mtx){mtx.lock_loop1();}
	static void unlock(syn_param& mtx){	mtx.unlock_loop1();}
};

class LP_loop2
{
public:
	static void lock(syn_param& mtx){mtx.lock_loop2();}
	static void unlock(syn_param& mtx){	mtx.unlock_loop2();}
};



class TNodeVisitorCG_GeneratorRValue;
class TNodeVisitorCG_GeneratorLValue;


class TNodeVisitorCG_GeneratorUnit : public TNodeVisitorCG
{
public:
	TNodeVisitorCG_GeneratorUnit(TNodeVisitorCG_Generator& cg_);

	TNodeVisitorCG_Generator& cgen;

};

class TNodeVisitorCG_GeneratorRValue : public TNodeVisitorCG_GeneratorUnit
{
public:
	TNodeVisitorCG_GeneratorRValue(TNodeVisitorCG_Generator& cg_):TNodeVisitorCG_GeneratorUnit(cg_){}


	virtual void handle(TNode*);

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
	virtual void visit(TNode_val_class*);
	virtual void visit(TNode_statement_list*);
	virtual void visit(TNode_statement_block*);
	virtual void visit(TNode_val_function*);
	virtual void visit(TNode_statement_return*);
	virtual void visit(TNode_statement_control*);
	virtual void visit(TNode_statement_switch*);
};

class TNodeVisitorCG_GeneratorLValue : public TNodeVisitorCG_GeneratorUnit
{
public:
	TNodeVisitorCG_GeneratorLValue(TNodeVisitorCG_Generator& cg_):TNodeVisitorCG_GeneratorUnit(cg_){}

	virtual void handle(TNode*);

	virtual void visit(TNode_var*);
	virtual void visit(TNode_variable_list*);
	virtual void visit(TNode_expression_dot*);
	virtual void visit(TNode_expression_arr*);
	virtual void visit(TNode_expression_list*);
	virtual void visit(TNode_val_nil*);
};


class TNodeVisitorCG_Generator : public TNodeVisitorCG
{
public:

	TNodeVisitorCG_GeneratorLValue lvis;
	TNodeVisitorCG_GeneratorRValue rvis;

	syn_global	gscope;
	syn_param	ascope;

	DataPtrT<CallableCode> pCode;

	virtual void parse(TNode* q);

	void pmadj(TNode_expression* nd,int pm);

	TNodeVisitorCG_Generator(CodeGen& g);

	int find_ggvar(const String& s);
	int find_param(const String& s);
	int find_local(const String& s,int d=VAR_DEFAULT);
	int findglobal(const String& s);

	int addglobal(const String& s);
	int add_local(const String& s);
	int add_param(const String& s);

	indexer_set<Variant> aConst;
	arr_1t<kvar_inst> aInsts;

	void emit(int op,int pm=0);

	void emit_push(const String& s);
	void emit_push(int64_t s);
	void emit_push(int32_t s);
	void emit_push(double s);
	void emit_push(bool s);
	void emit_push(CallableData* s);

	void emit_get_ggvar(const String& s);
	void emit_set_table(const String& s);
	void emit_get_table(const String& s);
	void emit_set_index(const String& s);
	void emit_get_index(const String& s);
	void emit_set_local(int n);
	void emit_get_local(int n);

	size_t ggidx(const String& s);
	size_t ggidx(double s);
	size_t ggidx(int64_t s);
	size_t ggidx(CallableData* s);

	void kerror(const String& s);
};

VHWD_LEAVE

#endif
