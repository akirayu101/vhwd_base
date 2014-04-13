#include "xml_parser.h"
#include "vhwd/basic/system.h"
#include "vhwd/basic/exception.h"


VHWD_ENTER


class tagCharAttribute
{
public:
	bool is_zm; //字母
	bool is_sz; //数字
	bool is_id; //字母 数字 : _ .
	bool is_sp; //空白字符
	int sp_nt;	//字符串下个字符, '\\'为2，其他为1，暂不考虑中文字符等的处理
};

tagCharAttribute aCharAttribute[256];


void init_char_property()
{

	static bool first=true;
	if(!first) return;
	first=false;

	typedef unsigned char uint8_t;
	memset(aCharAttribute,0,sizeof(tagCharAttribute)*256);
	for(int i=0;i<256;i++) aCharAttribute[i].sp_nt=1;
	aCharAttribute['\\'].sp_nt=2;

	for(uint8_t i='a';i<='z';i++)
	{
		aCharAttribute[i].is_zm=true;
		aCharAttribute[i].is_id=true;
	}
	for(uint8_t i='A';i<='Z';i++)
	{
		aCharAttribute[i].is_zm=true;
		aCharAttribute[i].is_id=true;
	}
	for(uint8_t i='0';i<='9';i++)
	{
		aCharAttribute[i].is_sz=true;
		aCharAttribute[i].is_id=true;
	}
	{
		aCharAttribute['_'].is_id=true;
		aCharAttribute[':'].is_id=true;
		aCharAttribute['.'].is_id=true;
		aCharAttribute['-'].is_id=true;
	}

	{
		aCharAttribute[' '].is_sp=true;
		aCharAttribute['\t'].is_sp=true;
		aCharAttribute['\r'].is_sp=true;
		aCharAttribute['\n'].is_sp=true;
	}		
}


XmlParser::XmlParser()
{
	init_char_property();
}

inline String XmlParser::get_id(cuint8ptr_t p1,cuint8ptr_t p2) //取得字符串的id
{
	return String((char*)p1,(char*)p2);
}

bool XmlParser::parse(XmlDocument& xmldoc)
{
	if(vContents.empty())
	{
		System::LogTrace("XmlParser: contents is empty");
		return NULL;
	}

	pcur=&(*vContents.begin());
	pend=pcur+size;

	try
	{
		skip_header();
		XmlNode* node=parse_node();
		xmldoc.m_pRoot.reset(node);
		return true;
	}
	catch(...)
	{
		return false;
	}
}


void XmlParser::kerror(cuint8ptr_t ,const char* msg)
{
	String desc=String::Format("xml_parser: %s",msg);
	Exception::XError(desc.c_str());
}

inline void XmlParser::skip_sp() //跳过空白字符
{
	for(;aCharAttribute[*pcur].is_sp;pcur++);
}

XmlNode* XmlParser::parse_node() // 读取一个 xml节点
{
	skip_comment();
	cuint8ptr_t p1=pcur;

	if(pcur[0]!='<')
	{
		kerror(p1," < expected");
	}

	++pcur;
	String id=read_id();

	AutoPtrT<XmlNode> p(new XmlNode);
	p->m_sName=id;

	read_prop(p.get());

	if(pcur[0]=='/')
	{
		if(pcur[1]!='>')
		{
			kerror(p1," /> expected");
		}
		pcur+=2;
		return p.release();
	}
	else if(pcur[0]!='>')
	{
		kerror(p1," > expected");
	}

	++pcur;
	read_sub(p.get());

	if(pcur[0]!='<'||pcur[1]!='/')
	{
		kerror(p1," </ expected");
	}
	pcur+=2;

	p1=pcur;

	for(;*++pcur!='>';);

	k_test(p1,"> expected");

	cuint8ptr_t p2=pcur++;

	if(!is_same(p->m_sName,get_id(p1,p2)))
	{
		kerror(p1,"tags not match");
	}

	return p.release();
}

void XmlParser::skip_header()
{
	skip_sp();

	cuint8ptr_t p1=pcur;

	if(pcur[0]!='<' || pcur[1]!='?')
	{
		return;
	}

	pcur+=2;
	for(;pcur[0]!='?'||pcur[1]!='>';pcur++);

	k_test(p1,"?> expected");
	pcur+=2;

}

void XmlParser::skip_comment()
{
	for(;;)
	{
		skip_sp();
		cuint8ptr_t p1=pcur;
		if(pcur[0]!='<'||pcur[1]!='!')
		{
			return;
		}

		pcur+=2;
		if(pcur[0]=='-'&&pcur[1]=='-')
		{
			pcur+=2;
			for(;pcur[0]!='-'||pcur[1]!='-'||pcur[2]!='>';pcur++);
			k_test(p1,"--> expected");
			pcur+=3;
		}
		else
		{
			for(;pcur[0]!='>';pcur++);
			k_test(p1,"> expected");
			pcur++;
		}
	}
}

bool XmlParser::is_same(const String& p1,const String& p2)
{
	return p1==p2;
}


void XmlParser::read_sub(XmlNode* n) //读取子节点
{

	class xml_node_list
	{
	public:
		LitePtrT<XmlNode> pHead,pTail;

		void append(XmlNode* p)
		{
			if(!pHead)
			{
				pHead.reset(p);
				pTail.reset(p);
			}
			else
			{
				pTail->m_pNextSibling.reset(p);
				pTail.reset(p);
			}
		}
	};

	cuint8ptr_t p1=pcur;

	xml_node_list listChilds;

	for(;;)
	{
		skip_sp();
		if(*pcur=='<')
		{
			if(pcur[1]=='/')
			{
				//dummy node
				n->m_sContent=get_id(p1,pcur);
				break;
			}
			else if(pcur[1]=='!')
			{
				skip_comment();
				continue;
			}

			listChilds.append(parse_node());
			p1=pcur;
		}
		else
		{
			for(;*++pcur!='<';);
			k_test(p1,"< expected");

			n->m_sContent=get_id(p1,pcur);
			//n->addchild(dummy_node(p1,pcur));
			if(pcur[1]=='/')
			{
				break;
			}
			listChilds.append(parse_node());
			p1=pcur;
		}
	}

	n->m_pFirstChild.reset(listChilds.pHead.get());
	listChilds.pHead.reset(NULL);
	listChilds.pTail.reset(NULL);
}

void XmlParser::read_prop(XmlNode* n) //读取属性
{
	for(;;)
	{
		skip_sp();
		if(!aCharAttribute[*pcur].is_id)
		{
			break;
		}
		String id=read_id();
		skip_sp();
		if(*pcur!='=')
		{
			n->AddAttribute(id,String());
			continue;
			//kerror(p1," = expected");
		}
		++pcur;
		skip_sp();
		String pp=read_val();
		n->AddAttribute(id,pp);		
	}
}

String XmlParser::read_val() //读取值
{
	if(*pcur=='"')
	{
		return read_str();
	}
	else
	{
		return read_id();
	}
}

String XmlParser::read_str() //读取字符串值
{
	cuint8ptr_t p1=pcur++;
	for(;*pcur!='"';pcur+=aCharAttribute[*pcur].sp_nt);
	cuint8ptr_t p2=++pcur;
	return get_id(p1,p2);

}

String XmlParser::read_id() //读取id
{
	cuint8ptr_t p1=pcur;
	for(;aCharAttribute[*pcur].is_id;pcur++);
	cuint8ptr_t p2=pcur;
	if(p1==p2) kerror(p1," id expected");
	return get_id(p1,p2);
}



bool XmlParser::LoadXml(const String& f)
{
	std::ifstream ifs;
	ifs.open(f.c_str(),std::ios::in|std::ios::binary);
	if(!ifs.good()) return false;
	ifs.seekg(0,std::ios_base::end);
	size=(size_t)ifs.tellg();		
	vContents.resize(size+100);
	ifs.seekg(0,std::ios_base::beg);
	ifs.read((char*)&vContents[0],size);
	ifs.close();

	const char* h="< -->\"/>?<> ";
	strcpy((char*)&vContents[size+1],h);
	return true;
}

bool XmlParser::SaveXml(XmlDocument& xmldoc,const String& s)
{
	std::ofstream ofs(s.c_str());
	if(!ofs.good()) return false;
	savenode(ofs,xmldoc.m_pRoot.get());
	return true;
}

void XmlParser::savenode(std::ofstream& ofs,XmlNode* node,int lv)
{

	tabindent(ofs,lv);
	ofs<< "<" << str(node->m_sName);
	for(size_t i=0;i<node->m_aAttribute.size();i++)
	{
		ofs<<" "<<str(node->m_aAttribute[i].m_sName)<<"="<<str(node->m_aAttribute[i].m_sValue);
	}

	if(node->m_pFirstChild==NULL)
	{
		if(!node->m_sContent.empty())
		{
			ofs<<" >"<<str(node->m_sContent)<<"</"<<str(node->m_sName)<<">"<<std::endl;
		}
		else
		{
			ofs<<" />"<<std::endl;
		}

	}
	else
	{
		ofs<<" >"<<std::endl;

		for(XmlNode* p=node->m_pFirstChild;p!=NULL;p=p->m_pNextSibling)
		{	
			savenode(ofs,p,lv+1);
		}

		tabindent(ofs,lv);
		ofs<<"</"<<str(node->m_sName)<<">"<<std::endl;
	}
}


VHWD_LEAVE
