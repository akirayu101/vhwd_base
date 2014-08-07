#ifndef __H_VHWD_SCRIPTING_INSTRUCTION__
#define __H_VHWD_SCRIPTING_INSTRUCTION__

#include "vhwd/config.h"

VHWD_ENTER


enum
{
	XOP_NOOP,
	XOP2_ADD,
	XOP2_SUB,
	XOP2_MUL,
	XOP2_DIV,
	XOP2_MOD,
	XOP2_POW,
	XOP2_DOT_MUL,
	XOP2_DOT_DIV,

	XOP2_AND,
	XOP2_OR,
	XOP2_XOR,

	XOP2_BITWISE_AND,
	XOP2_BITWISE_OR,
	XOP2_BITWISE_XOR,


	XOP2_GT,
	XOP2_GE,
	XOP2_LT,
	XOP2_LE,
	XOP2_EQ,
	XOP2_NE,

	XOP1_INV,
	XOP1_SIN,
	XOP1_COS,
	XOP1_TAN,
	XOP1_LOG,
	XOP1_LOG10,

	XOP1_NOT,
	XOP1_BITWISE_NOT,
	XOP1_BITWISE_INV,

	XOP_PUSH_INT_DIRECT,
	XOP_PUSH_BOOL,
	XOP_PUSH_NIL,

	XOP_SPADJ,		// add stack pointer

	XOP_GET_GGVAR,	// system variables
	XOP_GET_LLVAR,	// local constants

	XOP_GET_ARRAY,	// operator[] (rvalue)
	XOP_SET_ARRAY,	// operator[] (lvalue)
	XOP_GET_TABLE,	// global variable(rvalue)
	XOP_SET_TABLE,	// global variable(lvalue)
	XOP_GET_INDEX,	// operator. (rvalue)
	XOP_SET_INDEX,	// operator. (lvalue)

	XOP_GET_LOCAL,	// local variable (rvalue)
	XOP_SET_LOCAL,	// local variable (lvalue)
	XOP_GET_PARAM,	// function param (rvalue)
	XOP_SET_PARAM,	// function param (lvalue)
	XOP_GET_STACK,
	XOP_SET_STACK,

	XOP_CALL,
	XOP_CALL_THIS,

	XOP_PROG1,
	XOP_PROG2,
	XOP_PMADJ,	

	XOP_JUMP,
	XOP_JUMP_N,
	XOP_JUMP_TRUE,
	XOP_JUMP_FALSE,

	XOP_RETURN,
	XOP_HALT,

};

VHWD_LEAVE
#endif
