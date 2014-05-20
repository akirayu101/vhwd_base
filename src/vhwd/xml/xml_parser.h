#include "vhwd/xml/xml_attribute.h"
#include "vhwd/xml/xml_node.h"
#include "vhwd/xml/xml_document.h"
#include "vhwd/basic/stringbuffer.h"

#include <fstream>

VHWD_ENTER

class ParserBase
{
public:

	typedef char mychar;
	typedef char* mychar_ptr;

	template<template<unsigned> class P> static inline void skip(mychar_ptr& p)
	{
		mychar_ptr tmp=p;
		while(lookup_table<P>::test(*tmp)) ++tmp;
		p=tmp;
	}
};


	
class XmlParser : public ParserBase
{
public:

	XmlParser(XmlDocument& xmldoc_);

	bool load(const char* pstr_,size_t size_);
	bool load(const String& f);
	bool save(const String& f);

	inline XmlNode* CreateNode(int t=XmlNode::XMLNODE_ELEMENT)
	{
		return new XmlNode(t);
	}

	inline XmlAttribute* CreateAttr()
	{
		return new XmlAttribute();
	}

protected:	

	XmlDocument& xmldoc;
	arr_1t<XmlNode*> nodes;
	StringBuffer<mychar> buffer;
	StringBuffer<mychar> tempbuf; //string_assign buffer

	class NodeLocker
	{
	public:
		NodeLocker(arr_1t<XmlNode*>& nodes_,XmlNode* pnode):nodes(nodes_)
		{
			pnode->m_pParent.reset(nodes.back());
			nodes.back()->AppendChild(pnode);
			nodes.push_back(pnode);
		}

		~NodeLocker()
		{
			nodes.pop_back();
		}

		arr_1t<XmlNode*>& nodes;

	};

	mychar_ptr pbeg; 
	mychar_ptr pend;
	mychar_ptr pcur; //current position
	size_t size;


	static void savenode(std::ostream& ofs,XmlNode* pnode,int lv=0);
	static void tabindent(std::ostream& ofs,int lv){for(int i=0;i<lv;i++) ofs<<"\t";}
	static void savestring(std::ostream& ofs,const String& v);

	void kerror(mychar_ptr p1,const char* msg);

	bool parse_document();


	XmlNode* parse_element_node();

	//A document node is a specialized kind of element node. 
	//It has a type p but no attributes. Instead it has an optional URL u. 
	//The intent of the URL is to specify a specialized data model for this node and its children. 
	//A document node looks like this:
	//<!doctype p "u">c1 . . . cm for m>0
	void parse_doctype_node();

	//A processing instruction (PI) node is always a leaf node. 
	//It only has an instruction i associated with it. 
	//The instruction is a sequence of zero or more characters, without any restrictions, 
	//except that the sequence may not start with the three characters `xml' (upper, lower or mixed case) followed by a space or newline. 
	//It looks like this in the XML document:
	//<?i?>
	void parse_instruction_node();

	//A comment node is similar to a processing instruction. It is also a leaf node and has only a comment c:
	//<!--c-->
	void parse_comment_node();

	void parse_subnodes();
	void parse_data();
	void parse_cdata();

	void parse_attributes();

	void parse_value();
	void parse_string1();
	void parse_string2();

	void string_assign(String& s0,mychar_ptr p1,mychar_ptr p2);

};

VHWD_LEAVE
