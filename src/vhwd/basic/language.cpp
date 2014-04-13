#include "vhwd/basic/language.h"
#include "vhwd/basic/string.h"
#include "vhwd/basic/stringbuffer.h"
#include "vhwd/basic/system.h"
#include "vhwd/collection/indexer_map.h"
#include "vhwd/ipc/shm.h"
#include <fstream>

VHWD_ENTER


class const_char_pointer_map : public indexer_map_trait<const char*,const char*,int>
{
public:
	typedef indexer_map_trait<const char*,const char*,int>::pair_type pair_type;

	static int hashcode_key(const char* v)
	{
		hash_t<char*> h;
		return h(v);
	}

	static int hashcode_key(const pair_type &v)
	{
		return hashcode_key(v.first);
	}

	static bool compare(const char* k,const pair_type& v)
	{
		return strcmp(k,v.first)==0;
	}
};

class LanguageImpl
{
public:


	StringBuffer<char> vContents;
	indexer_map<const char*,const char*,const_char_pointer_map> mapStrings;


	inline const String& Translate(const String& msg) const
	{
		int id=mapStrings.find(msg.c_str());
		if(id<0) return msg;
		return *(String*)&mapStrings.get(id);
	}


	void SkipWhite(const char* p1,size_t& i)
	{
		while(p1[i]==' '||p1[i]=='\t'||p1[i]=='\r'||p1[i]=='\n') i++;
	}

	bool ReadString(String& s,const char* p1,size_t& i)
	{
		while(p1[i++]!='\"');
		const char* tmp1_p1=p1+i;
		while(p1[i++]!='\"');
		const char* tmp1_p2=p1+i-1;

		s=String(tmp1_p1,tmp1_p2);

		for(;;)
		{
			SkipWhite(p1,i);
			if(p1[i]!='\"')
			{
				break;
			}

			i++;
			const char* tmp1_p1=p1+i;
			while(p1[i++]!='\"');
			const char* tmp1_p2=p1+i-1;

			s+=String(tmp1_p1,tmp1_p2);

		}

		return true;
	}

	bool Init(const String& file)
	{

		if(file.size()>3 && file.substr(file.size()-3,3)!=".po")
		{
			return LoadMo(file);
		}
		else
		{
			return LoadPo(file);
		}
	}

	bool LoadPo(const String& file)
	{

		SharedMem shm;
		if(!shm.OpenFile(file,0,SharedMem::FLAG_RD))
		{
			return false;
		}

		size_t sz=shm.size();
		char* pa=shm.addr();
		if((unsigned char)pa[0]==0xEF&&(unsigned char)pa[1]==0xBB&&(unsigned char)pa[2]==0xBF)
		{
			pa+=3;
			sz-=3;
		}

		StringBuffer<char> vt;

		if(!String::utf8_to_ansi(vt,pa,sz))
		{
			System::LogTrace("LoadPo: utf8_to_ansi failed!");
			return false;
		}

		shm.Close();
		mapStrings.clear();
		vContents.clear();

		sz=vt.size();
		vt.insert(vt.end(),"\n\"\0",3);

		StringBuffer<unsigned> vp;

		const char* p1=&vt[0];
		String tmp1,tmp2;

		for(size_t i=0;i<sz;)
		{
			if(memcmp(p1+i,"msgid",5)!=0)
			{
				while(p1[i++]!='\n');
				continue;
			}

			i+=6;
			ReadString(tmp1,p1,i);
			if(memcmp(p1+i,"msgstr",5)!=0)
			{
				continue;
			}

			i+=7;
			ReadString(tmp2,p1,i);

			if(tmp1==tmp2 || tmp1.empty() || tmp2.empty())
			{
				continue;
			}

			vp.push_back(vContents.size());
			vContents.append(tmp1.c_str(),tmp1.size()+1);
			vp.push_back(vContents.size());
			vContents.append(tmp2.c_str(),tmp2.size()+1);
		}

		mapStrings.set_capacity(vp.size()/2);
		for(size_t i=0;i<vp.size();i+=2)
		{
			mapStrings[&vContents[vp[i]]]=&vContents[vp[i+1]];
		}

		return true;
	}


	bool LoadMo(const String& file)
	{
		std::ifstream fs;
		fs.open(file.c_str(),std::ios::in|std::ios::binary);
		if(!fs.good())
		{
			System::LogTrace("Cannot open file %s for reading",file);
			return false;
		}

		fs.seekg(0,std::ios::end);
		size_t len=fs.tellg();
		fs.seekg(0,std::ios::beg);

		StringBuffer<char> vutf8;
		vutf8.resize(len);
		fs.read(&vutf8[0],len);
		mapStrings.clear();

		StringBuffer<char> vansi;
		if(!String::utf8_to_ansi(vansi,vutf8.data(),vutf8.size()))
		{
			System::LogTrace("LoadMo: utf8_to_ansi failed!");
			return false;
		}

		mapStrings.clear();
		vContents=vansi;

		StringBuffer<unsigned> vp;
		unsigned j=0;
		for(unsigned i=0;i<vContents.size();i++)
		{
			if(vContents[i]!='\0')
			{
				continue;
			}
			vp.push_back(j);
			j=i+1;
		}

		mapStrings.set_capacity(vp.size()/2);
		for(size_t i=0;i<vp.size();i+=2)
		{
			mapStrings[&vContents[vp[i]]]=&vContents[vp[i+1]];
		}
		return true;

	}

	bool SaveMo(const String& file)
	{
		std::ofstream fs;
		fs.open(file.c_str(),std::ios::out|std::ios::binary);
		if(!fs.good())
		{
			System::LogTrace("Cannot open file %s for writing",file);
			return false;
		}

		if(vContents.empty())
		{
			return false;
		}

		StringBuffer<char> va;

		if(!String::ansi_to_utf8(va,vContents.data(),vContents.size()))
		{
			System::LogTrace("SaveMo: ansi_to_utf8 failed!");
			return false;
		}

		fs.write(&va[0],va.size());

		fs.close();
		return fs.good();
	}

};

bool Language::Init(const String& file)
{
	if(!impl)
	{
		impl=new LanguageImpl;
	}
	return ((LanguageImpl*)impl)->Init(file);
}

bool Language::Save(const String& file)
{
	if(!impl)
	{
		impl=new LanguageImpl;
	}
	return ((LanguageImpl*)impl)->SaveMo(file);
}

Language::Language()
{
	impl=NULL;
}

Language::~Language()
{
	delete ((LanguageImpl*)impl);
}

const String& Language::Translate(const String& msg) const
{
	if(!impl)
	{
		return msg;
	}
	else
	{
		return ((const LanguageImpl*)impl)->Translate(msg);
	}
}

Language& Language::current()
{
	static Language gInstance;
	return gInstance;
}


const String& VHWD_DLLIMPEXP Translate(const String& msg)
{
	return Language::current().Translate(msg);
}

VHWD_LEAVE
