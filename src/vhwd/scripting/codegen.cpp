
#include "vhwd/scripting/codegen.h"
#include "vhwd/scripting/codegen_generator.h"
#include "vhwd/scripting/parser_node.h"
#include "vhwd/scripting/executor.h"
#include "vhwd/scripting/parser.h"

VHWD_ENTER

CodeGen::CodeGen()
{

}

bool CodeGen::parse(TNode_statement_list* q)
{
	stmt_list.reset(q);

	TNodeVisitorCG_Analyzer vis_phase1(*this);
	TNodeVisitorCG_Generator vis_phase2(*this);

	try
	{
		vis_phase1.parse(q);
		vis_phase2.parse(q);
	}
	catch(std::exception& e)
	{
		this_logger().LogError(e.what());
		return false;
	}

	return true;
}


bool CodeGen::parse(const String& p)
{
	Parser parser;
	DataPtrT<TNode_statement_list> q=parser.parse(p);
	if(!q) return false;
	return parse(q.get());
}

void CodeGen::kerror(const String& s)
{
	Exception::XError(String::Format("CodeGen:%s",s));
}

void TNodeVisitorCG::kerror(const String& s)
{
	cg.kerror(s);
}




VHWD_LEAVE

