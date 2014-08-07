#include "vhwd/scripting/parser.h"
#include "vhwd/scripting/parser_node.h"

VHWD_ENTER


typedef indexer_map<String,int> op_priority_map;

op_priority_map g_create_op2priority_map()
{
	op_priority_map m;

	m["++"]=10;
	m["--"]=10;
	m["~"]=10;
	m["!"]=10;

	m["&"]=8;
	m["|"]=8;
	m["^"]=8;
	m[".*"]=7;
	m["./"]=7;
	m["*"]=7;
	m["/"]=7;
	m["%"]=6;
	m["+"]=5;
	m["-"]=5;
	// 4 is reserved for colon a:b
	m[">"]=3;
	m["<"]=3;
	m[">="]=3;
	m["<="]=3;
	m["=="]=3;
	m["!="]=3;
	m["&&"]=2;
	m["||"]=2;
	m["^^"]=2;
	m[".."]=2;
	// 1 is reserved for comma, a,b,c
	m["="]=0;
	m["+="]=0;
	m["-="]=0;
	m["*="]=0;
	m["/="]=0;
	m["+="]=0;
	m["&="]=0;
	m["|="]=0;
	m["^="]=0;
	return m;
}

const op_priority_map op2priority_map=g_create_op2priority_map();

class parser_handler
{
public:

	static bool is_stmt_end(tokItem* pcur)
	{
		tokType tk=pcur[0].type;
		if(tk==TOK_KEY && pcur[0].word=="case") return true;
		if(tk==TOK_KEY && pcur[0].word=="default") return true;
		return tk==TOK_END||tk==TOK_KET1||tk==TOK_KET2||tk==TOK_KET3;
	}

	static bool is_expr_end(tokItem* pcur)
	{
		tokType tk=pcur[0].type;
		return tk==TOK_END||tk==TOK_KET1||tk==TOK_KET2||tk==TOK_KET3||tk==TOK_SEMICOLON;
	}

	static bool is_ket3_end(tokItem* pcur)
	{
		tokType tk=pcur[0].type;
		return tk==TOK_END||tk==TOK_KET3;
	}
};

template<typename T>
class read_node_handler : public parser_handler
{
public:
	static DataPtrT<T> g(Parser& parser);
};

template<int N>
class read_expr_handler : public parser_handler
{
public:
	static DataPtrT<TNode_expression> g(Parser& parser);
};

template<> DataPtrT<TNode_item> read_node_handler<TNode_item>::g(Parser& parser);


template<> DataPtrT<TNode_statement> read_node_handler<TNode_statement>::g(Parser& parser);
template<> DataPtrT<TNode_statement_assignment> read_node_handler<TNode_statement_assignment>::g(Parser& parser);
template<> DataPtrT<TNode_statement_block> read_node_handler<TNode_statement_block>::g(Parser& parser);
template<> DataPtrT<TNode_statement_control> read_node_handler<TNode_statement_control>::g(Parser& parser);
template<> DataPtrT<TNode_statement_if> read_node_handler<TNode_statement_if>::g(Parser& parser);
template<> DataPtrT<TNode_statement_list> read_node_handler<TNode_statement_list>::g(Parser& parser);
template<> DataPtrT<TNode_statement_return> read_node_handler<TNode_statement_return>::g(Parser& parser);
template<> DataPtrT<TNode_statement_loop> read_node_handler<TNode_statement_loop>::g(Parser& parser);
template<> DataPtrT<TNode_statement_switch> read_node_handler<TNode_statement_switch>::g(Parser& parser);
template<> DataPtrT<TNode_expression> read_node_handler<TNode_expression>::g(Parser& parser);
template<> DataPtrT<TNode_expression_list> read_node_handler<TNode_expression_list>::g(Parser& parser);
template<> DataPtrT<TNode_val_class> read_node_handler<TNode_val_class>::g(Parser& parser);
template<> DataPtrT<TNode_val_function> read_node_handler<TNode_val_function>::g(Parser& parser);
template<> DataPtrT<TNode_variable_list> read_node_handler<TNode_variable_list>::g(Parser& parser);


template<>
DataPtrT<TNode_item> read_node_handler<TNode_item>::g(Parser& parser)
{
	DataPtrT<TNode_item> node;
	if(parser.test(TOK_ID))
	{
		node.reset(new TNode_var(parser.pcur[-1].word));
	}
	else if(parser.test(TOK_DOUBLE))
	{
		node.reset(new TNode_val_double(parser.pcur[-1].word));
	}
	else if(parser.test(TOK_INTEGER))
	{
		node.reset(new TNode_val_integer(parser.pcur[-1].word));
	}
	else if(parser.test(TOK_STRING))
	{
		node.reset(new TNode_val_string(parser.pcur[-1].word));
	}
	else if(parser.test(TOK_KEY,"true"))
	{
		node.reset(new TNode_val_boolean(parser.pcur[-1].word));
	}
	else if(parser.test(TOK_KEY,"false"))
	{
		node.reset(new TNode_val_boolean(parser.pcur[-1].word));
	}
	else if(parser.test(TOK_KEY,"nil"))
	{
		node.reset(new TNode_val_nil);
	}
	else if(parser.pcur[0].type==TOK_KEY && parser.pcur[0].word=="function")
	{
		node=read_node_handler<TNode_val_function>::g(parser);
	}
	else if(parser.pcur[0].type==TOK_KEY && parser.pcur[0].word=="class")
	{
		node=read_node_handler<TNode_val_class>::g(parser);
	}
	else
	{
		parser.kerror("item expected");
	}

	return node;
}




template<int N>
DataPtrT<TNode_expression> read_expr_handler<N>::g(Parser& parser)
{
	DataPtrT<TNode_expression> node(read_expr_handler<N+1>::g(parser));
	while(parser.pcur[0].type==TOK_OP)
	{
		if(op2priority_map[parser.pcur[0].word]!=N)
		{
			break;
		}

		TNode_expression_op2 *n=new TNode_expression_op2;
		n->m_sOpName=parser.pcur[0].word;
		n->param[0]=node;
		node.reset(n);

		++parser.pcur;
		n->param[1]=read_expr_handler<N+1>::g(parser);
	}

	return node;
}


template<>
DataPtrT<TNode_expression> read_expr_handler<4>::g(Parser& parser)
{
	const int N=4;
	DataPtrT<TNode_expression> node(read_expr_handler<N+1>::g(parser));
	if(parser.test(TOK_COLON))
	{
		TNode_expression_call* n=new TNode_expression_call;
		TNode_expression_list* l=new TNode_expression_list;
		n->param.reset(l);
		l->aList.append(node.get());
		n->tbl.reset(new TNode_var("colon"));
		node.reset(n);

		l->aList.append(read_expr_handler<N+1>::g(parser));
		if(parser.test(TOK_COLON))
		{
			l->aList.append(read_expr_handler<N+1>::g(parser));
		}
	}

	return node;
}



template<>
DataPtrT<TNode_expression> read_expr_handler<1>::g(Parser& parser)
{
	const int N=1;

	if(parser.pcur[0].type==TOK_KEY&&parser.pcur[0].word=="function")
	{
		return read_node_handler<TNode_val_function>::g(parser);
	}
	if(parser.pcur[0].type==TOK_KEY&&parser.pcur[0].word=="class")
	{
		return read_node_handler<TNode_val_class>::g(parser);
	}

	if(parser.flags.get(TNode_expression::FLAG_LOCAL))
	{
		DataPtrT<TNode_variable_list> vl=read_node_handler<TNode_variable_list>::g(parser);
		for(TNode_variable_list::nodelist::iterator it=vl->aList.begin();it!=vl->aList.end();++it)
		{
			(*it)->flags.add(TNode_expression::FLAG_LOCAL);
		}
		return vl;
	}

	DataPtrT<TNode_expression_list> node(new TNode_expression_list);

	for(;;)
	{
		while(parser.test(TOK_COMMA))
		{
			node->aList.append(new TNode_val_nil);
		}

		if(parser.pcur[0].type==TOK_OP&&op2priority_map[parser.pcur[0].word]<1)
		{
			break;
		}

		if(is_expr_end(parser.pcur))
		{
			break;
		}

		node->aList.append(read_expr_handler<N+1>::g(parser));
		if(!parser.test(TOK_COMMA))
		{
			break;
		}
	}

	return node;
}



static DataPtrT<TNode_expression_op_assign> read_node_function_adjust(TNode_val_decl* node)
{
		DataPtrT<TNode_expression_op_assign> nopa=new TNode_expression_op_assign;
		nopa->m_sOpName="=";
		nopa->param[0].reset(new TNode_var(node->m_sName));
		nopa->param[1]=node;
		node->m_sName="";
		return nopa;
}

template<>
DataPtrT<TNode_expression> read_expr_handler<0>::g(Parser& parser)
{
	const int N =0;

	LockState<BitFlags> lock1(parser.flags,0);

	if(parser.pcur[0].type==TOK_KEY && parser.pcur[0].word=="local")
	{
		++parser.pcur;
		parser.flags.add(TNode_expression::FLAG_LOCAL);
	}


	DataPtrT<TNode_expression> node(read_expr_handler<N+1>::g(parser));

	TNode_val_decl* qf=dynamic_cast<TNode_val_decl*>(node.get());
	if(qf)
	{
		DataPtrT<TNode_expression_op_assign> qa=read_node_function_adjust(qf);
		if(parser.flags.get(TNode_expression::FLAG_LOCAL)) qa->param[0]->flags.add(TNode_expression::FLAG_LOCAL);
		return qa;
	}



	if(parser.pcur[0].type!=TOK_OP||op2priority_map[parser.pcur[0].word]!=0)
	{
		DataPtrT<TNode_expression_op_assign> nopa=new TNode_expression_op_assign;
		if(parser.flags.get(TNode_expression::FLAG_LOCAL))
		{
			nopa->param[0]=node;
			nopa->param[1].reset(new TNode_val_nil);
		}
		else
		{
			nopa->param[0].reset(new TNode_val_nil);
			nopa->param[1]=node;
		}

		return nopa;
	}

	parser.flags.clr(0);

	while(parser.pcur[0].type==TOK_OP)
	{
		if(op2priority_map[parser.pcur[0].word]!=0)
		{
			break;
		}

		TNode_expression_op_assign *n=new TNode_expression_op_assign;
		n->m_sOpName=parser.pcur[0].word;
		n->param[0]=node;
		node.reset(n);

		++parser.pcur;
		n->param[1]=read_expr_handler<N+1>::g(parser);
	}

	return node;
}

template<>
DataPtrT<TNode_expression> read_expr_handler<9>::g(Parser& parser)
{
	typedef arr_1t<String> astr_array;
	DataPtrT<TNode_expression> node;
	astr_array aOp;
	while(parser.pcur[0].type==TOK_OP)
	{
		String &s(parser.pcur[0].word);
		if(s=="+"||s=="-"||s=="!"||s=="~"||s=="++"||s=="--")
		{
			aOp.push_back(s);
		}
		else
		{
			parser.kexpected("expression start");
		}
		++parser.pcur;
	}

	if(parser.test(TOK_BRA1))
	{
		node=read_node_handler<TNode_expression>::g(parser);
		node->flags.add(TNode_expression::FLAG_WITH_BRAKET);
		parser.match(TOK_KET1);
	}
	else
	{
		node=read_node_handler<TNode_item>::g(parser);
	}

	for(;;)
	{
		if(parser.test(TOK_DOT))
		{
			if(!parser.test(TOK_ID))
			{
				parser.kerror("id");
			}
			TNode_expression_dot *n=new TNode_expression_dot;
			n->tbl=node;
			node.reset(n);
			n->m_sIndex=parser.pcur[-1].word;
		}
		else if(parser.test(TOK_BRA2))
		{
			TNode_expression_arr *n=new TNode_expression_arr;
			n->tbl=node;
			node.reset(n);
			n->param=read_node_handler<TNode_expression_list>::g(parser);
			parser.match(TOK_KET2);
		}
		else if(parser.test(TOK_BRA1))
		{
			TNode_expression_call* n=new TNode_expression_call;
			n->tbl=node;node.reset(n);
			n->param=read_node_handler<TNode_expression_list>::g(parser);
			parser.match(TOK_KET1);
		}
		else if(parser.test(TOK_PTR))
		{
			TNode_expression_call* n=new TNode_expression_call;
			n->cls=node;node.reset(n);
			n->tbl=read_node_handler<TNode_item>::g(parser);
			parser.match(TOK_BRA1);
			n->param=read_node_handler<TNode_expression_list>::g(parser);
			parser.match(TOK_KET1);
		}
		else
		{
			break;
		}
	}


	for(astr_array::reverse_iterator it=aOp.rbegin();it!=aOp.rend();++it)
	{
		TNode_expression_op1 *n=new TNode_expression_op1;
		n->m_sOpName=(*it);
		n->param[0]=node;
		node.reset(n);
	}

	while(parser.pcur[0].type==TOK_OP && (parser.pcur[0].word=="++"||parser.pcur[0].word=="--"))
	{
		TNode_expression_op1 *n=new TNode_expression_op1;
		n->m_sOpName=parser.pcur[0].word;
		n->param[0]=node;
		n->flags.add(TNode_expression_op1::FLAG_POST);
		node.reset(n);
		++parser.pcur;
	}


	return node;
}



template<>
DataPtrT<TNode_expression> read_node_handler<TNode_expression>::g(Parser& parser)
{
	return read_expr_handler<2>::g(parser);
}


template<>
DataPtrT<TNode_expression_list> read_node_handler<TNode_expression_list>::g(Parser& parser)
{
	if(is_expr_end(parser.pcur))
	{
		return new TNode_expression_list;
	}

	LockState<BitFlags> lock1(parser.flags,0);
	DataPtrT<TNode_expression> q=read_expr_handler<1>::g(parser);
	TNode_expression_list* l=dynamic_cast<TNode_expression_list*>(q.get());
	if(l) return l;
	l=new TNode_expression_list;
	l->aList.append(q);
	return l;
}

template<>
DataPtrT<TNode_statement_list> read_node_handler<TNode_statement_list>::g(Parser& parser)
{
	DataPtrT<TNode_statement_list> node(new TNode_statement_list);
	while(!is_stmt_end(parser.pcur))
	{
		DataPtrT<TNode_statement> d=read_node_handler<TNode_statement>::g(parser);
		if(!d)
		{
			continue;
		}

		node->aList.append(d.get());
	}
	return node;
}


template<>
DataPtrT<TNode_statement_block> read_node_handler<TNode_statement_block>::g(Parser& parser)
{

	parser.match(TOK_BRA3);

	DataPtrT<TNode_statement_block> node(new TNode_statement_block);
	DataPtrT<TNode_statement_list> q(read_node_handler<TNode_statement_list>::g(parser));
	if(q)
	{
		node->aList.swap(q->aList);
	}
	parser.match(TOK_KET3);

	return node;

}



template<>
DataPtrT<TNode_statement> read_node_handler<TNode_statement>::g(Parser& parser)
{
	DataPtrT<TNode_statement> q;
	if(parser.pcur[0].type==TOK_SEMICOLON)
	{
		q=new TNode_statement_nil;
	}
	else if(parser.pcur[0].type==TOK_KEY)
	{
		const String& sval(parser.pcur[0].word);
		if(sval=="if")
		{
			q=read_node_handler<TNode_statement_if>::g(parser);
		}
		else if(sval=="while"||sval=="for"||sval=="do"||sval=="for_each")
		{
			q=read_node_handler<TNode_statement_loop>::g(parser);
		}
		else if(sval=="function"||sval=="local"||sval=="class")
		{
			q=read_node_handler<TNode_statement_assignment>::g(parser);
		}
		else if(sval=="break"||sval=="continue"||sval=="continue2"||sval=="break2"||sval=="break3")
		{
			q=read_node_handler<TNode_statement_control>::g(parser);
		}
		else if(sval=="return")
		{
			q=read_node_handler<TNode_statement_return>::g(parser);
		}
		else if(sval=="switch")
		{
			q=read_node_handler<TNode_statement_switch>::g(parser);
		}
	}
	else if(is_stmt_end(parser.pcur))
	{
		q=new TNode_statement_nil;
	}
	else if(parser.pcur[0].type==TOK_BRA3)
	{
		q=read_node_handler<TNode_statement_block>::g(parser);
	}
	else
	{
		q=read_node_handler<TNode_statement_assignment>::g(parser);
	}

	if(parser.test(TOK_SEMICOLON))
	{
		q->flags.add(TNode_statement::FLAG_WITH_SEMI);
	}
	return q;
}

template<>
DataPtrT<TNode_variable_list> read_node_handler<TNode_variable_list>::g(Parser& parser)
{
	DataPtrT<TNode_variable_list> node(new TNode_variable_list);
	while(parser.pcur[0].type==TOK_ID)
	{
		node->aList.append(new TNode_var(parser.pcur[0].word));
		++parser.pcur;
		if(parser.test(TOK_COMMA))
		{
			if(parser.pcur[0].type!=TOK_ID)
			{
				parser.kexpected("id");
			}
			continue;
		}
		else
		{
			break;
		}
	}
	return node;
}



template<>
DataPtrT<TNode_val_function> read_node_handler<TNode_val_function>::g(Parser& parser)
{

	DataPtrT<TNode_val_function> node(new TNode_val_function);

	++parser.pcur;

	if(parser.pcur[0].type==TOK_ID)
	{
		node->m_sName=parser.pcur[0].word;
		++parser.pcur;
	}

	parser.match(TOK_BRA1);
	node->parm_list=read_node_handler<TNode_variable_list>::g(parser);
	parser.match(TOK_KET1);

	node->func_body=read_node_handler<TNode_statement_block>::g(parser);

	return node;

}

template<>
DataPtrT<TNode_val_class> read_node_handler<TNode_val_class>::g(Parser& parser)
{

	DataPtrT<TNode_val_class> node(new TNode_val_class);

	++parser.pcur;

	if(parser.pcur[0].type==TOK_ID)
	{
		node->m_sName=parser.pcur[0].word;
		++parser.pcur;
	}

	node->func_body=read_node_handler<TNode_statement_block>::g(parser);

	return node;

}
template<>
DataPtrT<TNode_statement_if> read_node_handler<TNode_statement_if>::g(Parser& parser)
{
	DataPtrT<TNode_statement_if> node(new TNode_statement_if);
	++parser.pcur;
	parser.match(TOK_BRA1);
	node->expr_cond=read_node_handler<TNode_expression>::g(parser);
	parser.match(TOK_KET1);
	node->stmt_if_1=read_node_handler<TNode_statement>::g(parser);
	if(parser.test(TOK_KEY,"else"))
	{
		node->stmt_if_0=read_node_handler<TNode_statement>::g(parser);
	}
	return node;
}

template<>
DataPtrT<TNode_statement_switch> read_node_handler<TNode_statement_switch>::g(Parser& parser)
{
	DataPtrT<TNode_statement_switch> node(new TNode_statement_switch);
	++parser.pcur;
	parser.match(TOK_BRA1);
	node->expr_cond=read_node_handler<TNode_expression>::g(parser);

	parser.match(TOK_KET1);
	parser.match(TOK_BRA3);

	for(;;)
	{

		if(parser.test(TOK_KEY,"case"))
		{
			DataPtrT<TNode_statement_switch::case_item> item(new TNode_statement_switch::case_item);
			for(;;)
			{
				DataPtrT<TNode_item> n=read_node_handler<TNode_item>::g(parser);
				item->case_value.append(n);

				if(parser.test(TOK_COLON))
				{
					break;
				}
				if(parser.test(TOK_COMMA))
				{
					continue;
				}
				parser.kerror("invalid case");
				break;

			}
			//parser.match(TOK_COLON);
			item->case_stmt=read_node_handler<TNode_statement_list>::g(parser);
			node->case_list.append(item);

		}
		else if(parser.test(TOK_KEY,"default"))
		{
			DataPtrT<TNode_statement_switch::case_item> item(new TNode_statement_switch::case_item);
			parser.match(TOK_COLON);
			item->case_stmt=read_node_handler<TNode_statement_list>::g(parser);
			node->case_list.append(item);
		}
		else
		{
			break;
		}

	}

	parser.match(TOK_KET3);
	return node;

}
template<>
DataPtrT<TNode_statement_loop> read_node_handler<TNode_statement_loop>::g(Parser& parser)
{
	DataPtrT<TNode_statement_loop> node(new TNode_statement_loop);
	node->m_sName=parser.pcur[0].word;

	if(node->m_sName=="do")
	{
		++parser.pcur;
		node->stmt_body=read_node_handler<TNode_statement>::g(parser);
		parser.match(TOK_KEY,"while");
		parser.match(TOK_BRA1);
		if(parser.pcur[0].type!=TOK_KET1)
		{
			node->expr_cond=read_node_handler<TNode_expression>::g(parser);
		}
		parser.match(TOK_KET1);

		return node;
	}
	else if(node->m_sName=="for_each")
	{
		++parser.pcur;
		parser.match(TOK_BRA1);
		node->fe_variable=read_node_handler<TNode_variable_list>::g(parser);
		parser.match(TOK_KEY,"in");
		node->fe_container=read_node_handler<TNode_expression>::g(parser);
		parser.match(TOK_KET1);
		node->stmt_body=read_node_handler<TNode_statement>::g(parser);
		return node;
	}



	++parser.pcur;
	parser.match(TOK_BRA1);

	if(node->m_sName=="for")
	{
		node->stmt_init=(read_node_handler<TNode_statement>::g(parser));
		if(!parser.test(TOK_SEMICOLON))
		{
			node->expr_cond=read_node_handler<TNode_expression>::g(parser);
			parser.match(TOK_SEMICOLON);
		}
		node->stmt_fini=(read_node_handler<TNode_statement>::g(parser));
	}
	else if(parser.pcur[0].type!=TOK_KET1)
	{
		node->expr_cond=(read_node_handler<TNode_expression>::g(parser));
	}

	parser.match(TOK_KET1);
	node->stmt_body=read_node_handler<TNode_statement>::g(parser);

	return node;
}

template<>
DataPtrT<TNode_statement_control> read_node_handler<TNode_statement_control>::g(Parser& parser)
{
	DataPtrT<TNode_statement_control> node(new TNode_statement_control);
	node->m_sName=parser.pcur[0].word;
	++parser.pcur;
	return node;
}

template<>
DataPtrT<TNode_statement_return> read_node_handler<TNode_statement_return>::g(Parser& parser)
{
	DataPtrT<TNode_statement_return> node(new TNode_statement_return);
	++parser.pcur;
	node->value=(read_node_handler<TNode_expression_list>::g(parser));
	return node;
}

template<>
DataPtrT<TNode_statement_assignment> read_node_handler<TNode_statement_assignment>::g(Parser& parser)
{
	DataPtrT<TNode_statement_assignment> node(new TNode_statement_assignment);
	node->value=(read_expr_handler<0>::g(parser));
	return node;
}

void Parser::kerror(const String& s)
{
	Exception::XError(String::Format("Parser:%s",s));
}

void Parser::kexpected(const String s)
{
	kerror(s+" expected");
}

void Parser::match(tokType type)
{
	if(pcur[0].type==type)
	{
		++pcur;
	}
	else
	{
		kerror("unexpected token type");
	}
}

void Parser::match(tokType type,const String& s)
{
	if(pcur[0].type==type && pcur[0].word==s)
	{
		++pcur;
	}
	else
	{
		kerror("unexpected token type");
	}
}

bool Parser::test(tokType type)
{
	if(pcur[0].type==type)
	{
		++pcur;
		return true;
	}
	else
	{
		return false;
	}
}

bool Parser::test(tokType type,const String& s)
{
	if(pcur[0].type==type && pcur[0].word==s)
	{
		++pcur;
		return true;
	}
	else
	{
		return false;
	}
}

DataPtrT<TNode_statement_list> Parser::parse(const String& p)
{
	Scanner scanner;

	if(!scanner.parse(p))
	{
		return NULL;
	}

	return parse(scanner.aTokens.data());

}


DataPtrT<TNode_statement_list> Parser::parse(tokItem* t)
{
	pcur=t;
	if(!pcur)
	{
		return NULL;
	}

	return read_node_handler<TNode_statement_list>::g(*this);
}

VHWD_LEAVE
