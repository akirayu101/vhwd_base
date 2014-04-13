#include "vhwd/xml/xml_attribute.h"
#include "vhwd/xml/xml_node.h"
#include "vhwd/xml/xml_document.h"

VHWD_ENTER
	
class XmlParser
{
public:

	XmlParser();

	typedef unsigned char uint8_t;
	typedef const uint8_t* cuint8ptr_t;

	arr_1t<uint8_t> vContents;

	bool LoadXml(const String& f);
	bool SaveXml(XmlDocument& xmldoc,const String& f);

	bool parse(XmlDocument& xmldoc);


	const String& str(const String& p)
	{
		return p;
	}

protected:
	cuint8ptr_t pcur;
	cuint8ptr_t pend; //current position
	size_t size;

	static void tabindent(std::ofstream& ofs,int lv)
	{
		for(int i=0;i<lv;i++) ofs<<"\t";
	}

	void savefile(XmlDocument& xmldoc);
	void savenode(std::ofstream& ofs,XmlNode* node,int lv=0);

	void kerror(cuint8ptr_t p1,const char* msg);

	inline void k_test(cuint8ptr_t p1,const char* msg)
	{
		if(pcur>=pend) kerror(p1,msg);
	}

	XmlNode* parse_node(); // 读取一个 xml节点
	void skip_header();
	void skip_comment();

	bool is_same(const String& p1,const String& p2);

	void read_sub(XmlNode* n);
	void read_prop(XmlNode* n);

	String read_val(); //读取值
	String read_str(); //读取字符串值
	void skip_sp();
	String read_id();
	String get_id(cuint8ptr_t p1,cuint8ptr_t p2);

};

VHWD_LEAVE
