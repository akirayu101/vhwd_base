#ifndef __H_CODEGEN__
#define __H_CODEGEN__

#include "vhwd/scripting/parser_visitor.h"
#include "vhwd/scripting/parser_node.h"
#include "vhwd/scripting/variant_code.h"

VHWD_ENTER


enum
{
	VAR_LOCAL,
	VAR_DEFAULT,
	VAR_GLOBAL,
};


class CodeGen : public Object
{
public:

	CodeGen();

	bool parse(const String& p);
	bool parse(TNode_statement_list* q);


	DataPtrT<TNode_statement_list> stmt_list;
	DataPtrT<CallableCode> pkstmt;

	void kerror(const String& s);

};



VHWD_LEAVE

#endif
