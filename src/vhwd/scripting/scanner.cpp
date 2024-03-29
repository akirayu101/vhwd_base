#include "vhwd/scripting/scanner.h"

VHWD_ENTER

void tokState::inc()
{
	if(pcur[0]=='\n')
	{
		++line;
		cpos=1;
	}
	else
	{
		++cpos;
	}
	++pcur;
}


template<unsigned N>
class lkt_is_character
{
public:
	static const int value=(N>='a'&&N<='z')||(N>='A'&&N<='Z');
};

template<unsigned N>
class lkt_is_number10
{
public:
	static const int value=(N>='0'&&N<='9');
};

template<unsigned N>
class lkt_is_number16
{
public:
	static const int value=lkt_is_number10<N>::value||(N>='a'&&N<='f')||(N>='A'&&N<='F');
};

template<unsigned N>
class lkt_is_namestart
{
public:
	static const int value=lkt_is_character<N>::value||N=='_';
};

template<unsigned N>
class lkt_is_name
{
public:
	static const int value=lkt_is_namestart<N>::value||lkt_is_number10<N>::value;
};

template<unsigned N>
class lkt_is_string
{
public:
	static const int value=N!='\0'&&N!='"'&&N!='\\';
};

template<unsigned N>
class lkt_is_comment2
{
public:
	static const int value=N!='\0'&&N!='/';
};

template<unsigned N>
class lkt_is_comment1
{
public:
	static const int value=N!='\0'&&N!='\n';
};



void Scanner::read_name()
{
	mychar_ptr p1=stok.pcur;
	skip<lkt_is_name>(stok);
	mychar_ptr p2=stok.pcur;
	tokitem.word.assign(p1,p2);	
	tokitem.type=aKeyword[tokitem.word];
	add_item();
}

void Scanner::read_number()
{
	mychar_ptr p1=stok.pcur;

	if(stok.pcur[0]=='0'&&(stok.pcur[1]=='x'||stok.pcur[1]=='X'))
	{
		stok.inc();
		stok.inc();
		skip<lkt_is_number16>(stok);
		mychar_ptr p2=stok.pcur;
		tokitem.type=TOK_INTEGER;
		tokitem.word.assign(p1,p2);	
		add_item();
	}

	skip<lkt_is_number10>(stok);
	if(stok.pcur[0]=='.')
	{
		tokitem.type=TOK_DOUBLE;
		stok.inc();
		skip<lkt_is_number10>(stok);
	}
	else
	{
		tokitem.type=TOK_INTEGER;
	}

	if(stok.pcur[0]=='e')
	{
		stok.inc();
		if(stok.pcur[0]=='+'||stok.pcur[0]=='-')
		{
			stok.inc();
		}
		skip<lkt_is_number10>(stok);
	}
	mychar_ptr p2=stok.pcur;

	tokitem.word.assign(p1,p2);	
	add_item();
	
}

void Scanner::read_string()
{
	StringBuffer<char> sb;

	while(stok.pcur[0]=='"')
	{
		stok.inc();
		for(;;)
		{
			mychar_ptr p1=stok.pcur;
			skip<lkt_is_string>(stok);
			sb.append(p1,stok.pcur);

			if(stok.pcur[0]=='"')
			{
				stok.inc();
				break;
			}
			if(stok.pcur[0]=='\0')
			{
				kerror("unexpected end of string");
				return;
			}
			if(stok.pcur[0]=='\\')
			{
				switch(stok.pcur[1])
				{
				case 'n':
					sb.append('\n');
					break;
				case 't':
					sb.append('\t');
					break;
				case 'r':
					sb.append('\r');
					break;
				case '\\':
					sb.append('\\');
					break;
				case '\"':
					sb.append('\"');
					break;
				case '\n':
					break;
				default:
					kerror("unkown string escape");
					break;
				}

				stok.inc();
				stok.inc();
			}
		}
		skip<lkt_whitespace>(stok);
	}
	
	tokitem.type=TOK_STRING;
	tokitem.word=sb;
	add_item();

}

void Scanner::read_op2_a()
{
	mychar_ptr p1=stok.pcur;
	stok.inc();
	if(stok.pcur[0]=='=')
	{
		stok.inc();
		mychar_ptr p2=stok.pcur;
		gen_item(TOK_OP,p1,p2);
		return;
	}

	mychar_ptr p2=stok.pcur;
	gen_item(TOK_OP,p1,p2);	
}

void Scanner::read_op2_b()
{
	mychar_ptr p1=stok.pcur;
	if(stok.pcur[1]==stok.pcur[0]||stok.pcur[1]=='=')
	{
		stok.inc();
		stok.inc();
		mychar_ptr p2=stok.pcur;
		gen_item(TOK_OP,p1,p2);
		return;
	}

	stok.inc();
	mychar_ptr p2=stok.pcur;
	gen_item(TOK_OP,p1,p2);		
}

void Scanner::read_op2_c()
{
	mychar_ptr p1=stok.pcur;
	stok.inc();

	if(stok.pcur[-1]==stok.pcur[0])
	{
		stok.inc();
	}

	if(stok.pcur[0]=='=')
	{
		stok.inc();
	}

	mychar_ptr p2=stok.pcur;
	gen_item(TOK_OP,p1,p2);		
}

// comment /* ... */
void Scanner::read_comment2()
{
	stok.inc();
	stok.inc();
	if(stok.pcur[0]=='\0')
	{
		kerror("unexpected end of string");
		return;
	}

	stok.inc();
	for(;;)
	{
		skip<lkt_is_comment2>(stok);
		if(stok.pcur[0]=='\0')
		{
			kerror("unexpected end of string");
			return;
		}
		if(stok.pcur[-1]=='*')
		{
			stok.inc();
			break;
		}
		stok.inc();
	}
}

// comment // ...
void Scanner::read_comment1()
{
	stok.inc();
	stok.inc();

	skip<lkt_is_comment1>(stok);
	if(stok.pcur[0]=='\0')
	{
		kerror("unexpected end of string");
		return;
	}			
	stok.inc();

}


void Scanner::read_dot()
{
	mychar_ptr p1=stok.pcur;
	stok.inc();

	switch(stok.pcur[0])
	{
	case '*':
	case '/':
		stok.inc();
		tokitem.type=TOK_OP;
		tokitem.word.assign(p1,2);
		break;
	case '.':
		stok.inc();
		if(stok.pcur[0]=='.')
		{
			stok.inc();
			tokitem.type=TOK_ID;
			tokitem.word.assign(p1,3);
		}
		else
		{
			tokitem.type=TOK_OP;
			tokitem.word.assign(p1,2);
		}
		break;
	default:
		tokitem.type=TOK_DOT;
		tokitem.word.assign(p1,1);
		break;
	}
	add_item();

}

Scanner::Scanner()
{
	aKeyword["if"]=TOK_KEY;
	aKeyword["else"]=TOK_KEY;
	aKeyword["do"]=TOK_KEY;
	aKeyword["while"]=TOK_KEY;
	aKeyword["for"]=TOK_KEY;
	aKeyword["switch"]=TOK_KEY;
	aKeyword["for_each"]=TOK_KEY;
	aKeyword["end"]=TOK_KEY;
	aKeyword["global"]=TOK_KEY;
	aKeyword["local"]=TOK_KEY;
	aKeyword["in"]=TOK_KEY;
	aKeyword["break"]=TOK_KEY;
	aKeyword["continue"]=TOK_KEY;
	aKeyword["continue2"]=TOK_KEY;
	aKeyword["break2"]=TOK_KEY;
	aKeyword["break3"]=TOK_KEY;

	aKeyword["return"]=TOK_KEY;
	aKeyword["class"]=TOK_KEY;
	aKeyword["function"]=TOK_KEY;
	aKeyword["case"]=TOK_KEY;
	aKeyword["default"]=TOK_KEY;

	aKeyword["true"]=TOK_KEY;
	aKeyword["false"]=TOK_KEY;
	aKeyword["nil"]=TOK_KEY;

}

void Scanner::skip_whitespace()
{
	skip<lkt_whitespace>(stok);
	tokitem.line=stok.line;
	tokitem.cpos=stok.cpos;
}

template<unsigned N>
class lk_sm_start
{
public:

	static const unsigned N1=lkt_is_namestart<N>::value?1:0;
	static const unsigned N2=N1>0?N1:(lkt_is_number10<N>::value?2:0);
	static const unsigned N3=N2>0?N2:((N>0&&N<5)?3:0);
	static const unsigned value=N3>0?N3:N;
};

bool Scanner::parse(const String& s)
{

	aTokens.clear();

	pbeg=(mychar_ptr)s.c_str();
	pend=(mychar_ptr)pbeg+s.size();

	stok.pcur=pbeg;
	stok.line=1;
	stok.cpos=1;
	stok.last=TOK_END;

	for(;;)
	{
		skip_whitespace();
		unsigned char ch=lookup_table<lk_sm_start>::test(stok.pcur[0]);	
		switch(ch)
		{
		case 0:
			tokitem.type=TOK_END;
			tokitem.word="eof";
			add_item();
			return true;
		case 1:
			read_name();
			break;
		case 2:
			read_number();
			break;
		case '\"':
			read_string();
			break;
		case '/':
			if(stok.pcur[1]=='/')
			{
				read_comment1();
				break;
			}
			else if(stok.pcur[1]=='*')
			{
				read_comment2();
				break;
			}
		case '*':
		case '%':
		case '>':
		case '<':
		case '!':
			read_op2_a();
			break;
		case '=':
			if(stok.pcur[1]!='=')
			{
				new_item(TOK_OP);
			}
			else
			{
				mychar_ptr p1=stok.pcur;
				stok.inc();stok.inc();
				gen_item(TOK_OP,p1,stok.pcur);
			}
			break;
		case '-':
			if(stok.pcur[1]=='>')
			{
				mychar_ptr p1=stok.pcur;
				stok.inc();stok.inc();
				gen_item(TOK_PTR,p1,stok.pcur);
				break;
			}
		case '+':
			read_op2_b();
			break;
		case '|':
		case '&':
		case '^':
			read_op2_c();
			break;
		case '~':
			new_item(TOK_OP);
			break;
		case '.':
			read_dot();
			break;
		case '(':
			new_item(TOK_BRA1);
			break;
		case '[':
			new_item(TOK_BRA2);
			break;
		case '{':
			new_item(TOK_BRA3);
			break;
		case ')':
			new_item(TOK_KET1);
			break;
		case ']':
			new_item(TOK_KET2);
			break;
		case '}':
			new_item(TOK_KET3);
			break;
		case '?':
			new_item(TOK_QUESTION);
			break;
		case ':':
			new_item(TOK_COLON);
			break;
		case ';':
			new_item(TOK_SEMICOLON);
			break;
		case ',':
			new_item(TOK_COMMA);
			break;
		default:
			kerror("unexpected character");
		}//switch
	}//for

	return true;
}


void Scanner::add_item()
{
	aTokens.push_back(tokitem);
}

void Scanner::gen_item(tokType t,mychar_ptr p1,mychar_ptr p2)
{
	tokitem.type=t;
	tokitem.word.assign(p1,p2);	
	add_item();
}

void Scanner::new_item(tokType t)
{
	tokitem.type=t;
	tokitem.word.assign(stok.pcur,1);
	stok.inc();
	add_item();
}


void Scanner::kerror(const String& s)
{
	Exception::XError(String::Format("Scanner:%s at Line:%d pos:%d",s,(int)stok.line,(int)stok.cpos));
}

VHWD_LEAVE
