#ifndef __H_PARSER_NODE__
#define __H_PARSER_NODE__


#include "vhwd/scripting/variant.h"
#include "vhwd/scripting/parser_visitor.h"

#define SYNTAX_NODE(X) virtual void accept(TNodeVisitor& vis){vis.visit(this);}

VHWD_ENTER



class VHWD_DLLIMPEXP TNode : public ObjectData
{
public:

	enum
	{
		FLAG_LOCAL	=1<<0,
		FLAG_MAX	=1<<1,
	};

	BitFlags flags;

	SYNTAX_NODE(TNode);
};




template<typename T,typename B=TNode>
class VHWD_DLLIMPEXP TNode_list : public B
{
public:
	typedef B basetype;
	typedef ObjectGroupT<T> nodelist;

	nodelist aList;
};


class VHWD_DLLIMPEXP TNode_statement : public TNode
{
public:
	typedef TNode basetype;

	enum
	{
		FLAG_WITH_SEMI=basetype::FLAG_MAX,
		FLAG_MAX=FLAG_WITH_SEMI<<1,
	};
	
	SYNTAX_NODE(TNode_statement);
};


class VHWD_DLLIMPEXP TNode_statement_nil : public TNode_statement
{
public:
	typedef TNode_statement basetype;
	SYNTAX_NODE(TNode_statement_nil);
};


class VHWD_DLLIMPEXP TNode_expression : public TNode
{
public:
	typedef TNode basetype;

	enum
	{
		FLAG_MIN			=basetype::FLAG_MAX,
		FLAG_POST			=FLAG_MIN<<0,
		FLAG_WITH_BRAKET	=FLAG_MIN<<1,
		FLAG_MAX			=FLAG_MIN<<3,
	};


	virtual int pmc(){return -1;}
	virtual bool is_const(){return false;}

	virtual bool get(Variant&){return false;}
	

	SYNTAX_NODE(TNode_expression);
};


class VHWD_DLLIMPEXP TNode_expression_list : public TNode_list<TNode_expression,TNode_expression>
{
public:

	typedef TNode_list<TNode_expression,TNode_expression> basetype;

	virtual int pmc()
	{
		int n=0;
		for(nodelist::iterator it=aList.begin();it!=aList.end();++it)
		{
			int t=(*it)->pmc();
			if(t<0) return -1;
			n+=t;
		}
		return n;
	}


	SYNTAX_NODE(TNode_expression_list);
};


class VHWD_DLLIMPEXP TNode_item : public TNode_expression
{
public:
	typedef TNode_expression basetype;

	virtual int pmc(){return 1;}




	SYNTAX_NODE(TNode_item);
};


class VHWD_DLLIMPEXP TNode_var : public TNode_item
{
public:
	typedef TNode_item basetype;

	TNode_var(){}
	TNode_var(const String& s):m_sName(s){}
	String m_sName;
	virtual int pmc(){return 1;}


	SYNTAX_NODE(TNode_var);
};

class VHWD_DLLIMPEXP TNode_variable_list : public TNode_list<TNode_var,TNode_expression>
{
public:
	typedef TNode_expression basetype;
	virtual int pmc(){return (int)aList.size();}

	SYNTAX_NODE(TNode_variable_list);
};


class VHWD_DLLIMPEXP TNode_val : public TNode_item
{
public:

	typedef TNode_item basetype;

	String m_sValue;

	TNode_val(){}
	TNode_val(const String& s):m_sValue(s){}
	SYNTAX_NODE(TNode_val);
};

class VHWD_DLLIMPEXP TNode_val_nil : public TNode_val
{
public:
	typedef TNode_val basetype;

	virtual int pmc(){return 1;}

	virtual bool is_const(){return true;}

	virtual bool get(Variant& v){v.clear();return true;}


	SYNTAX_NODE(TNode_expression);
};


template<typename T> 
class VHWD_DLLIMPEXP TNode_valT : public TNode_val
{
public:
	typedef TNode_val basetype;
	T m_tValue;
	TNode_valT(){}
	TNode_valT(const String& v):TNode_val(v){}

	bool get(Variant& v){v.reset<T>(m_tValue);return true;}

	virtual bool is_const(){return true;}
};

class VHWD_DLLIMPEXP TNode_val_string : public TNode_valT<String>
{
public:
	TNode_val_string(const String& v):TNode_valT<String>(v){}
	SYNTAX_NODE(TNode_val_string);
};

class VHWD_DLLIMPEXP TNode_val_double : public TNode_valT<double>
{
public:
	TNode_val_double(const String& v):TNode_valT<double>(v){}
	SYNTAX_NODE(TNode_val_double);
};

class VHWD_DLLIMPEXP TNode_val_integer : public TNode_valT<int64_t>
{
public:
	TNode_val_integer(const String& v):TNode_valT<int64_t>(v){}
	SYNTAX_NODE(TNode_val_integer);
};

class VHWD_DLLIMPEXP TNode_val_boolean : public TNode_valT<bool>
{
public:
	TNode_val_boolean(const String& v):TNode_valT<bool>(v){}
	SYNTAX_NODE(TNode_val_boolean);
};

class VHWD_DLLIMPEXP TNode_expression_op : public TNode_expression
{
public:
	typedef TNode_expression basetype;
	String m_sOpName;
	virtual int size() const{return -1;}
	virtual TNode_expression* getp(int n){return NULL;}
	SYNTAX_NODE(TNode_expression_op);
};

template<unsigned N>
class VHWD_DLLIMPEXP TNode_expression_opT : public TNode_expression_op
{
public:
	typedef TNode_expression_op basetype;
	DataPtrT<TNode_expression> param[N];
	virtual int size() const{return N;}
	virtual TNode_expression* getp(int n){return param[n].get();}

	virtual int pmc(){return 1;}

};

class VHWD_DLLIMPEXP TNode_expression_op1 : public TNode_expression_opT<1>
{
public:
	TNode_expression_op1(){}

	virtual bool is_const(){return param[0]->is_const();}
	SYNTAX_NODE(TNode_expression_op1);
};

class VHWD_DLLIMPEXP TNode_expression_op2 : public TNode_expression_opT<2>
{
public:
	virtual bool is_const(){return param[0]->is_const()&&param[1]->is_const();}

	SYNTAX_NODE(TNode_expression_op2);
};

class VHWD_DLLIMPEXP TNode_expression_op_assign : public TNode_expression_op2
{
public:
	typedef TNode_expression_op2 basetype;

	virtual bool is_const(){return false;}

	virtual int pmc()
	{
		return param[0]->pmc();
	}


	SYNTAX_NODE(TNode_expression_op_assign);
};


class VHWD_DLLIMPEXP TNode_val_decl : public TNode_val
{
public:
	String m_sName;
	BitFlags flags;

	DataPtrT<TNode_statement_block> func_body;

};

class VHWD_DLLIMPEXP TNode_val_class : public TNode_val_decl
{
public:
	typedef TNode_val basetype;

	SYNTAX_NODE(TNode_val_class);
};


class VHWD_DLLIMPEXP TNode_val_function : public TNode_val_decl
{
public:
	typedef TNode_val basetype;

	enum
	{
		FLAG_MIN		=basetype::FLAG_MAX,
		FLAG_WITH_THIS	=FLAG_MIN<<0,
		FLAG_MAX		=FLAG_MIN<<1,
	};
	
	DataPtrT<TNode_variable_list> parm_list;
	SYNTAX_NODE(TNode_val_function);
};

class VHWD_DLLIMPEXP TNode_expression_call : public TNode_expression
{
public:
	typedef TNode_expression basetype;
	DataPtrT<TNode_expression> tbl;
	DataPtrT<TNode_expression> cls;
	DataPtrT<TNode_expression_list> param;

	virtual int pmc(){return -1;}

	SYNTAX_NODE(TNode_expression_call);
};


class VHWD_DLLIMPEXP TNode_expression_dot : public TNode_expression
{
public:
	typedef TNode_expression basetype;
	TNode_expression_dot(){}

	DataPtrT<TNode_expression> tbl;

	String m_sIndex;

	virtual int pmc(){return 1;}


	SYNTAX_NODE(TNode_expression_dot);
};

class VHWD_DLLIMPEXP TNode_expression_arr : public TNode_expression
{
public:
	typedef TNode_expression basetype;

	DataPtrT<TNode_expression> tbl;
	DataPtrT<TNode_expression_list> param;
	virtual int pmc(){return 1;}


	SYNTAX_NODE(TNode_expression_arr);
};



class VHWD_DLLIMPEXP TNode_statement_list : public TNode_list<TNode_statement,TNode_statement>
{
public:
	SYNTAX_NODE(TNode_statement_list);
};


class VHWD_DLLIMPEXP TNode_statement_block : public TNode_statement_list
{
public:
	typedef TNode_statement_list basetype;

	SYNTAX_NODE(TNode_statement_block);
};



class VHWD_DLLIMPEXP TNode_statement_switch : public TNode_statement
{
public:
	typedef TNode_statement basetype;

	class case_item : public ObjectData
	{
	public:
		ObjectGroupT<TNode_item> case_value;
		DataPtrT<TNode_statement_list> case_stmt;
	};

	typedef ObjectGroupT<case_item>::iterator case_iterator;
	typedef ObjectGroupT<TNode_item>::iterator item_iterator;

	DataPtrT<TNode_expression> expr_cond;
	ObjectGroupT<case_item> case_list;

	SYNTAX_NODE(TNode_statement_switch);
};

class VHWD_DLLIMPEXP TNode_statement_assignment : public TNode_statement
{
public:
	typedef TNode_statement basetype;

	DataPtrT<TNode_expression> value;
	SYNTAX_NODE(TNode_statement_assignment);
};

class VHWD_DLLIMPEXP TNode_statement_if : public TNode_statement
{
public:
	typedef TNode_statement basetype;

	DataPtrT<TNode_expression> expr_cond;
	DataPtrT<TNode_statement> stmt_if_1;
	DataPtrT<TNode_statement> stmt_if_0;

	SYNTAX_NODE(TNode_statement_if);
};

class VHWD_DLLIMPEXP TNode_statement_control : public TNode_statement
{
public:
	typedef TNode_statement basetype;
	String m_sName;
	SYNTAX_NODE(TNode_statement_control);
};

class VHWD_DLLIMPEXP TNode_statement_return : public TNode_statement
{
public:
	typedef TNode_statement basetype;
	DataPtrT<TNode_expression> value;

	SYNTAX_NODE(TNode_statement_return);
};

class VHWD_DLLIMPEXP TNode_statement_loop : public TNode_statement
{
public:
	typedef TNode_statement basetype;

	DataPtrT<TNode_statement> stmt_init;
	DataPtrT<TNode_expression> expr_cond;
	DataPtrT<TNode_statement> stmt_fini;
	DataPtrT<TNode_statement> stmt_body;

	String m_sName;

	DataPtrT<TNode_expression> fe_container;
	DataPtrT<TNode_variable_list> fe_variable;	

	SYNTAX_NODE(TNode_statement_loop);
};





VHWD_LEAVE
#endif
