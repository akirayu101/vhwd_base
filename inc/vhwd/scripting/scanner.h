#ifndef __H_SCANNER__
#define __H_SCANNER__

#include "vhwd/basic.h"
#include "vhwd/collection/indexer_map.h"

VHWD_ENTER


enum tokType
{
	TOK_ID,				// variable name
	TOK_KEY,			// keyword
	TOK_OP,
	TOK_INTEGER,
	TOK_DOUBLE,
	TOK_STRING,
	TOK_COMMENT,
	TOK_DOT,
	TOK_PTR,
	TOK_COMMA,
	TOK_COLON,
	TOK_QUESTION,
	TOK_SEMICOLON,
	TOK_BRA1,
	TOK_KET1,
	TOK_BRA2,
	TOK_KET2,
	TOK_BRA3,
	TOK_KET3,
	TOK_END,
};

class VHWD_DLLIMPEXP tokItem
{
public:	
	tokItem(){tags=0;}
	String word;
	tokType type;
	int tags;
	int line;
	int cpos;
};


class VHWD_DLLIMPEXP tokState
{
public:
	typedef char mychar;
	typedef const mychar* mychar_ptr;

	mychar_ptr pcur; //current position
	size_t line;
	size_t cpos;
	int last;

	void inc();

};

class VHWD_DLLIMPEXP Parser;

class VHWD_DLLIMPEXP ScannerBase : public Object
{
public:

	typedef tokState::mychar mychar;
	typedef tokState::mychar_ptr mychar_ptr;

protected:

	template<template<unsigned> class P> 
	static void skip(tokState& p);


	StringBuffer<mychar> tempbuf;

	mychar_ptr pbeg; 
	mychar_ptr pend;

	tokState stok;

	arr_1t<tokItem> aTokens;
	tokItem tokitem;

	indexer_map<String,tokType> aKeyword;

};


class VHWD_DLLIMPEXP Scanner : public ScannerBase
{
public:

	friend class Parser;

	Scanner();

	bool parse(const String& p);

protected:

	void add_item();
	void gen_item(tokType t,mychar_ptr p1,mychar_ptr p2);
	void new_item(tokType t);

	void read_name();

	void read_number();

	void read_string();

	void read_op2_a();

	void read_op2_b();

	void read_op2_c();

	void read_dot();

	// comment // ...
	void read_comment1();

	// comment /* ... */
	void read_comment2();

	void kerror(const String& s);

	void skip_whitespace();


};


template<template<unsigned> class P> 
void ScannerBase::skip(tokState& p)
{
	if(P<'\n'>::value)
	{
		while(lookup_table<P>::test(*p.pcur))
		{
			++p.pcur;
			if(*p.pcur=='\n')
			{
				++p.line;
				p.cpos=1;
			}
			else
			{
				++p.cpos;
			}
		}
	}
	else
	{
		while(lookup_table<P>::test(*p.pcur))
		{
			++p.pcur;			
			++p.cpos;
		}
	}
}

VHWD_LEAVE

#endif



