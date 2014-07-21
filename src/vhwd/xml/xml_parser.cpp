#include "xml_parser.h"
#include "vhwd/basic/system.h"
#include "vhwd/basic/exception.h"
#include "vhwd/basic/clock.h"
#include "vhwd/basic/console.h"
#include "vhwd/logging/logger.h"

#include <fstream>

VHWD_ENTER

template<unsigned N>
class lkt_name
{
public:
	static const int value=(N!='\0' && N!='=' && N!='!' && N!='>' && N!='<' && N!='?' && N!='/' && N!='\\') && (!lkt_whitespace<N>::value);
};

template<unsigned N>
class lkt_not_gt
{
public:
	static const int value=!(N=='\0'||N=='>');
};


XmlParser::XmlParser(XmlDocument& xmldoc_):xmldoc(xmldoc_)
{

}

bool XmlParser::parse_document()
{
	xmldoc.DeleteChildren();
	xmldoc.DeleteAttributes();

	pcur=pbeg;
	pend=pcur+size;
	nodes.clear();

	nodes.push_back(&xmldoc);

	try
	{
		parse_subnodes();
	}
	catch(std::exception& e)
	{
		this_logger().LogError(e.what());
		nodes.clear();
		return false;
	}

	nodes.clear();
	return true;
}


void XmlParser::kerror(const String& msg)
{
	size_t pos=pcur-pbeg;
	(void)&pos;
	String desc=String::Format("xml_parser: %s",msg);
	Exception::XError(desc.c_str());
}

void XmlParser::kexpected(const String& msg)
{
	size_t pos=pcur-pbeg;
	(void)&pos;
	String desc=String::Format("xml_parser: %s expected",msg);
	Exception::XError(desc.c_str());
}

template<unsigned N>
class lkt_number16b
{
public:
	static const unsigned char value1=(N>='0'&&N<='9')?(N-'0'):0xFF;
	static const unsigned char value2=(N>='A'&&N<='Z')?(N+10-'A'):value1;
	static const unsigned char value3=(N>='a'&&N<='z')?(N+10-'a'):value2;
	static const unsigned char value=value3;
};

template<unsigned N>
class lkt_number10b
{
public:
	static const unsigned char value=(N>='0'&&N<='9')?(N-'0'):0xFF;
};

inline void  XmlParser::string_assign(String& s0,mychar_ptr p1,mychar_ptr p2)
{
	if(p1[0]=='\''||p1[0]=='\"')
	{
		p1=p1+1;
		p2=p2-1;
	}

	tempbuf.reserve(p2-p1);

	// &amp; &apos; &quot; &gt; &lt; &#...; 
	mychar* dest=tempbuf.data();
	while(p1!=p2)
	{
		if(p1[0]=='&')
		{
			switch(p1[1])
			{
			case 'a':
				if(p1[2]=='m'&&p1[3]=='p'&&p1[4]==';')
				{
					*dest++='&';
					p1+=5;
				}
				else if(p1[2]=='p'&&p1[3]=='o'&&p1[4]=='s'&&p1[5]==';')
				{
					*dest++='\'';
					p1+=6;
				}
				else
				{
					pcur=p1;kexpected("&amp; or &apos;");
					return;
				}
				break;
			case 'g':
				if(p1[2]=='t'&&p1[3]==';')
				{
					*dest++='>';
					p1+=4;
				}
				else
				{
					pcur=p1;kexpected("&gt;");
					return;
				}
				break;
			case 'l':
				if(p1[2]=='t'&&p1[3]==';')
				{
					*dest++='<';
					p1+=4;
				}
				else
				{
					pcur=p1;kexpected("&lt;");
					return;
				}
				break;
			case 'q':
				if(p1[2]=='u'&&p1[3]=='o'&&p1[4]=='t'&&p1[5]==';')
				{
					*dest++='\"';
					p1+=6;
				}
				else
				{
					pcur=p1;kexpected("&quot;");
					return;
				}
				break;
			case '#':
				{
					uint32_t code = 0;
					if(p1[2]=='x')
					{
						p1+=3;
						for(;;)
						{
							unsigned char digit =lookup_table<lkt_number16b>::test(p1[0]);
							if (digit == 0xFF) break;
							code=(code<<4)+digit;p1+=1;
						}
					}
					else
					{
						p1+=2;
						for(;;)
						{
							unsigned char digit =lookup_table<lkt_number10b>::test(p1[0]);
							if (digit == 0xFF) break;
							code=(code*10)+digit;p1+=1;
						}
					}
					if(p1[0]!=';')
					{
						pcur=p1;kexpected(";");
						return;
					}
					p1+=1;

					if (code < 0x80)
					{
						dest[0]=code;
						dest+=1;
					}
					else if (code < 0x800)
					{
						dest[1] = ((code | 0x80) & 0xBF); code >>= 6;
						dest[0] = (code | 0xC0);
						dest+=2;
					}
					else if (code < 0x10000)
					{
						dest[2] = ((code | 0x80) & 0xBF); code >>= 6;
						dest[1] = ((code | 0x80) & 0xBF); code >>= 6;
						dest[0] = (code | 0xE0);
						dest+=3;
					}
					else if (code < 0x110000)
					{
						dest[3] = ((code | 0x80) & 0xBF); code >>= 6;
						dest[2] = ((code | 0x80) & 0xBF); code >>= 6;
						dest[1] = ((code | 0x80) & 0xBF); code >>= 6;
						dest[0] = (code | 0xF0);
						dest+=4;
					}
					else
					{
						pcur=p1;kerror("invalid unicode number");
						return;
					}
				}
				break;
			default:
				*dest++='&';
				break;
			}
		}
		else
		{
			*dest++=*p1++;
		}
	}

	s0.assign(tempbuf.data(),dest);
}


template<unsigned N>
class lkt_string_key1
{
public:
	static const int value=!(N=='\0'||N=='\''||N=='\\');
};
inline void XmlParser::parse_string1()
{
	pcur+=1;
	for(;;)
	{
		skip<lkt_string_key1>(pcur);
		if(pcur[0]=='\'')
		{
			pcur+=1;
			return;
		}
		else if(pcur[0]=='\\')
		{
			if(pcur[1]=='\0')
			{
				kerror("unexpected data end");
				return;
			}
			pcur+=2;
			continue;
		}
		else
		{
			kexpected("\'");
			return;
		}
	}
}

template<unsigned N>
class lkt_string_key2
{
public:
	static const int value=!(N=='\0'||N=='\"'||N=='\\');
};
inline void XmlParser::parse_string2()
{
	pcur+=1;
	for(;;)
	{
		skip<lkt_string_key2>(pcur);
		if(pcur[0]=='\"')
		{
			pcur+=1;
			return;
		}
		else if(pcur[0]=='\\')
		{
			if(pcur[1]=='\0')
			{
				kerror("unexpected data end");
				return;
			}

			pcur+=2;
			continue;
		}
		else
		{
			kexpected("\"");
			return;
		}
	}
}


inline void XmlParser::parse_value()
{
	switch(pcur[0])
	{
	case '\'':
		parse_string1();
		break;
	case '\"':
		parse_string2();
		break;
	default:
		skip<lkt_name>(pcur);
	}
}


inline void XmlParser::parse_comment_node()
{
	if(pcur[2]!='-')
	{
		kexpected("<!--");
	}
	pcur+=3;

	skip<lkt_not_gt>(pcur);
	if(pcur[0]!='>')
	{
		kexpected(">");
	}

	if(pcur[-1]!='-'||pcur[-2]!='-')
	{
		pcur-=2;kexpected("-->");
	}

	pcur+=1;
}


template<unsigned N>
class lkt_not_ltgt
{
public:
	static const int value=!(N=='\0'||N=='>'||N=='<');
};
inline void XmlParser::parse_doctype_node()
{
	pcur+=2;

	skip_tag(pcur,"DOCTYPE");

	size_t lt=1;

	for(;;)
	{
		skip<lkt_not_ltgt>(pcur);
		if(pcur[0]=='>')
		{
			pcur+=1;
			if(--lt==0)
			{
				return;
			}
		}
		else if(pcur[0]=='<')
		{
			pcur+=1;
			++lt;
		}
		else
		{
			kexpected(">");
			return;
		}
	}


}


inline void XmlParser::parse_cdata()
{
	pcur+=2;

	skip_tag(pcur,"[CDATA[");

	mychar_ptr tagvalue1=pcur;
	for(;;)
	{
		skip<lkt_not_gt>(pcur);
		if(pcur[0]=='\0')
		{
			kexpected("]]>");
			return;
		}
		if(pcur[-1]==']'&&pcur[-2]==']')
		{
			break;
		}
		pcur+=1;
	}
	mychar_ptr tagvalue2=pcur-2;
	XmlNode* pnode=CreateNode(XmlNode::XMLNODE_CDATA);
	pnode->m_sValue.assign(tagvalue1,tagvalue2);
	nodes.back()->AppendChild(pnode);
	pcur+=1;
}



inline void XmlParser::parse_instruction_node()
{
	pcur+=2;
	//mychar_ptr tagname1=pcur;
	skip<lkt_name>(pcur);
	//mychar_ptr tagname2=pcur;

	for(;;)
	{

		skip<lkt_whitespace>(pcur);
		if(!lookup_table<lkt_name>::test(pcur[0]))
		{
			break;
		}

		//mychar_ptr attrname1=pcur;
		skip<lkt_name>(pcur);
		//mychar_ptr attrname2=pcur;

		skip<lkt_whitespace>(pcur);
		if(pcur[0]!='=')
		{
			kexpected("=");
		}

		pcur+=1;
		skip<lkt_whitespace>(pcur);

		//mychar_ptr attrvalue1=pcur;
		parse_value();
		//mychar_ptr attrvalue2=pcur;

	}

	if(pcur[0]!='?'||pcur[1]!='>')
	{
		kexpected("?>");
	}
	pcur+=2;

}

template<unsigned N>
class parse_data_key
{
public:
	static const int value=!(N=='\0'||N=='<');
};
inline void XmlParser::parse_subnodes()
{
	mychar_ptr tagvalue1,tagvalue2;
	for(;;)
	{
		tagvalue1=pcur;
		skip<lkt_whitespace>(pcur);
		if(pcur[0]=='<')
		{
			switch(pcur[1])
			{
			case '/':
				pcur+=2;
				tagvalue1=pcur;
				skip<lkt_name>(pcur);
				tagvalue2=pcur;
				skip<lkt_whitespace>(pcur);
				if(pcur[0]!='>')
				{
					kexpected(">");
					return;
				}
				pcur+=1;
				return;

			case '!':
				switch(pcur[2])
				{
				case '-':
					parse_comment_node();
					break;
				case 'd':
				case 'D':
					parse_doctype_node();
					break;
				case '[':
					parse_cdata();
					break;
				default:
					kerror("unexpected character");
					return;
				}
				break;
			case '?':
				parse_instruction_node();
				break;
			default:
				parse_element_node();
				break;
			}
		}
		else
		{
			skip<parse_data_key>(pcur);
			if(pcur[0]!='<')
			{
				if(pcur[0]=='\0')
				{
					return;
				}

				kexpected("<");
				return;
			}

			tagvalue2=pcur;
			if(nodes.back()->m_sValue.empty())
			{
				this->string_assign(nodes.back()->m_sValue,tagvalue1,tagvalue2);
				//nodes.back()->m_sValue.assign(tagvalue1,tagvalue2);
			}

			XmlNode* pnode=CreateNode(XmlNode::XMLNODE_DATA);
			pnode->m_sValue.assign(tagvalue1,tagvalue2);
			nodes.back()->AppendChild(pnode);
		}
	}
}



inline void XmlParser::parse_attributes()
{
	for(;;)
	{

		skip<lkt_whitespace>(pcur);
		if(!lookup_table<lkt_name>::test(pcur[0])) return;

		mychar_ptr attrname1=pcur;
		skip<lkt_name>(pcur);
		mychar_ptr attrname2=pcur;

		skip<lkt_whitespace>(pcur);
		if(pcur[0]!='=')
		{
			kexpected("=");
		}

		pcur+=1;
		skip<lkt_whitespace>(pcur);

		mychar_ptr attrvalue1=pcur;
		parse_value();
		mychar_ptr attrvalue2=pcur;

		XmlAttribute* attr=CreateAttr();
		attr->m_sName.assign(attrname1,attrname2);
		string_assign(attr->m_sValue,attrvalue1,attrvalue2);
		nodes.back()->AppendAttribute(attr);

	}
}

inline XmlNode* XmlParser::parse_element_node()
{
	pcur+=1;

	mychar_ptr tagname1=pcur;
	skip<lkt_name>(pcur);
	mychar_ptr tagname2=pcur;

	XmlNode* pnode=CreateNode();
	pnode->m_sName.assign(tagname1,tagname2);

	NodeLocker lock1(nodes,pnode);

	parse_attributes();

	switch(pcur[0])
	{
	case '/':
		if(pcur[1]!='>')
		{
			kexpected(">");
		}
		pcur+=2;
		return NULL;
	case '>':
		pcur+=1;
		parse_subnodes();
		break;
	default:
		kexpected(">");
	}

	return NULL;
}


bool XmlParser::load(const char* pstr_,size_t size_)
{
	if(pstr_[size_]=='\0'||(size_>0&&pstr_[size_-1]!='\0'))
	{
		pbeg=(mychar_ptr)pstr_;
		size=size_;
	}
	else
	{
		buffer.resize(size_+1);
		memcpy(buffer.data(),pstr_,size_);
		buffer[size_]='\0';
		pbeg=(mychar_ptr)buffer.data();
		size=buffer.size();
	}

	return parse_document();
}

bool XmlParser::load(const String& f)
{
	if(!buffer.load(f,FILE_BINARY))
	{
		return false;
	}

	size=buffer.size();
	buffer.push_back(0);
	pbeg=(mychar_ptr)buffer.data();

	return parse_document();
}

bool XmlParser::save(const String& s)
{
	std::ofstream ofs(s.c_str());
	if(!ofs.good()) return false;

	for(XmlNode* pnode=xmldoc.GetFirstChild();pnode;pnode=pnode->GetNext())
	{
		savenode(ofs,pnode,0);
	}

	return true;
}

void XmlParser::savestring(std::ostream& ofs,const String& v)
{
	//size_t ln=v.size();
	const char* p1=v.c_str();
	const char px[]="&quot;";
	for(;;)
	{
		const char* p2=::strstr(p1,"\"");
		if(p2==NULL)
		{
			ofs<<p1;
			return;
		}
		ofs.write(p1,p2-p1);
		ofs.write(px,6);
		p1=p2+1;
	}
}

void XmlParser::savenode(std::ostream& ofs,XmlNode* pnode,int lv)
{

	if(pnode->GetType()==XmlNode::XMLNODE_DATA)
	{
		ofs<<pnode->GetValue();
		return;
	}

	if(pnode->GetType()==XmlNode::XMLNODE_CDATA)
	{
		ofs<<"<![CDATA[[";
		ofs<<pnode->GetValue();
		ofs<<"]]>";
		return;
	}

	tabindent(ofs,lv);
	ofs<< "<" << pnode->GetName();

	for(XmlAttribute* pattr=pnode->GetFirstAttribute();pattr;pattr=pattr->GetNext())
	{
		ofs<<" "<<pattr->GetName()<<"=\"";
		savestring(ofs,pattr->GetValue());
		ofs<<"\"";
	}

	XmlNode* fnode=pnode->GetFirstChild();

	if(fnode==NULL)
	{
		ofs<<"/>"<<std::endl;
		return;
	}

	ofs<<">";

	if(fnode->GetNext()==NULL && fnode->GetType()==XmlNode::XMLNODE_DATA)
	{
		savenode(ofs,fnode);
	}
	else
	{
		if(fnode->GetType()==XmlNode::XMLNODE_ELEMENT)
		{
			ofs<<std::endl;
		}
		for(XmlNode* tnode=fnode;tnode;tnode=tnode->GetNext())
		{
			savenode(ofs,tnode,lv+1);
		}

		tabindent(ofs,lv);
	}
	ofs<< "</" << pnode->GetName()<<">"<<std::endl;

}

VHWD_LEAVE
