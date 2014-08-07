#ifndef __H_PARSER__
#define __H_PARSER__


#include "vhwd/scripting/scanner.h"


VHWD_ENTER

class VHWD_DLLIMPEXP TNode_statement_list;
class VHWD_DLLIMPEXP tokItem;

template<typename T>
class read_node_handler;

template<int N>
class read_expr_handler;

class VHWD_DLLIMPEXP VHWD_DLLIMPEXP Parser
{
public:

	DataPtrT<TNode_statement_list> parse(const String& p);
	DataPtrT<TNode_statement_list> parse(tokItem* t);

protected:

	template<typename T>
	friend class read_node_handler;

	template<int N>
	friend class read_expr_handler;

	void match(tokType type);
	void match(tokType type,const String& s);
	bool test(tokType type);
	bool test(tokType type,const String& s);

	void kerror(const String& s);
	void kexpected(const String s);
	tokItem* pcur;


	BitFlags flags;
};

VHWD_LEAVE

#endif


