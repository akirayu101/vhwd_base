#include "vhwd/basic/language.h"
#include "vhwd/basic/string.h"
#include "vhwd/basic/stringbuffer.h"
#include "vhwd/basic/system.h"
#include "vhwd/collection/indexer_map.h"
#include "vhwd/collection/linear_buffer.h"
#include "vhwd/ipc/shm.h"
#include "vhwd/basic/pointer.h"
#include "vhwd/basic/codecvt.h"


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


class LangData : public ObjectData
{
public:
	String m_sName;
	StringBuffer<char> m_aCont;


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


	bool LoadPo(const String& file)
	{

		m_sName=file;
		m_aCont.clear();

		StringBuffer<char> vt;
		if(!vt.load(file,FILE_TEXT))
		{
			return false;
		}

		size_t sz=vt.size();
		vt.insert(vt.end(),"\n\"\0",3);

		const char* p1=vt.data();
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

			m_aCont.append(tmp1.c_str(),tmp1.size()+1);
			m_aCont.append(tmp2.c_str(),tmp2.size()+1);
		}

		return true;
	}

	bool LoadMo(const String& file)
	{
		m_sName=file;
		return m_aCont.load(file);
	}
};


class LanguageImpl
{
public:

	arr_1t<DataPtrT<LangData> > aDatas;

	indexer_map<const char*,const char*,const_char_pointer_map> mapStrings;

	inline const String& Translate(const String& msg) const
	{
		int id=mapStrings.find(msg.c_str());
		if(id<0) return msg;
		return *(String*)&mapStrings.get(id).second;
	}


	bool AddCatalog(const String& file)
	{
		DataPtrT<LangData> pData(new LangData);

		if(file.size()>3 && file.substr(file.size()-3,3)!=".po")
		{
			if(!pData->LoadMo(file))
			{
				return false;
			}
		}
		else
		{
			if(!pData->LoadPo(file))
			{
				return false;
			}
		}

		return AddCatalog(pData);
	}



	bool AddCatalog(DataPtrT<LangData>& pData)
	{
		StringBuffer<char>& aCont(pData->m_aCont);

		StringBuffer<unsigned> vp;
		unsigned j=0;
		for(unsigned i=0;i<aCont.size();i++)
		{
			if(aCont[i]!='\0')
			{
				continue;
			}
			vp.push_back(j);
			j=i+1;
		}

		mapStrings.rehash(vp.size()/2);
		for(size_t i=0;i<vp.size();i+=2)
		{
			mapStrings[&aCont[vp[i]]]=&aCont[vp[i+1]];
		}

		aDatas.push_back(pData);
		return true;
	}



	bool SaveMo(const String& file)
	{
		StringBuffer<char> bt;
		size_t sz=mapStrings.size();
		for(size_t i=0;i<sz;i++)
		{
			const char* k=mapStrings.get(i).first;
			const char* v=mapStrings.get(i).second;
			bt.append(k,::strlen(k)+1);
			bt.append(v,::strlen(v)+1);
		}

		StringBuffer<char> bs;

		if(!IConv::ansi_to_utf8(bs,bt.data(),bt.size()))
		{
			return false;
		}

		return bs.save(file);
	}

};

bool Language::AddCatalog(const String& file)
{
	if(!impl)
	{
		impl=new LanguageImpl;
	}
	return ((LanguageImpl*)impl)->AddCatalog(file);
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
