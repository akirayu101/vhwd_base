#include "vhwd/basic/stringbuffer.h"
#include "vhwd/basic/codecvt.h"

#include <cstring>
#include <clocale>
#include <fstream>

VHWD_ENTER



template<typename T>
StringBuffer<T>::StringBuffer(const T* p1)
{
	assign(p1,std::char_traits<T>::length(p1));
}

template<typename T>
class StringBufferHelper
{
public:
	static void equal(StringBuffer<T>& h,const String& o)
	{
		CodeCvt<T>::s2ws(h,o.c_str(),o.size());
	}

	static void eqadd(StringBuffer<T>& h,const String& o)
	{
		StringBuffer<T> tmp;
		CodeCvt<T>::s2ws(tmp,o.c_str(),o.size());
		h+=tmp;
	}
};

template<>
class StringBufferHelper<char>
{
public:
	typedef char T;

	static void equal(StringBuffer<T>& h,const String& o)
	{
		h.assign(o.c_str(),o.size());
	}

	static void eqadd(StringBuffer<T>& h,const String& o)
	{
		h.append(o.c_str(),o.size());
	}
};


template<>
class StringBufferHelper<unsigned char>
{
public:
	typedef unsigned char T;

	static void equal(StringBuffer<T>& h,const String& o)
	{
		h.assign((T*)o.c_str(),o.size());
	}

	static void eqadd(StringBuffer<T>& h,const String& o)
	{
		h.append((T*)o.c_str(),o.size());
	}
};


template<typename T>
const StringBuffer<T>& StringBuffer<T>::operator=(const String& o)
{
	StringBufferHelper<T>::equal((*this),o);
	return *this;
}

template<typename T>
const StringBuffer<T>& StringBuffer<T>::operator+=(const String& o)
{
	StringBufferHelper<T>::eqadd((*this),o);
	return *this;
}

//TEXTFILE BOM
//UTF8 EF BB BF
//UTF-16 (BE)	FE FF
//UTF-16 (LE)	FF FE
//UTF-32 (BE)	00 00 FE FF
//UTF-32 (LE)	FF FE 00 00

static const unsigned char bom_utf8[]={0xEF,0xBB,0xBF};
static const unsigned char bom_utf16_le[]={0xFF,0xFE};
static const unsigned char bom_utf16_be[]={0xFE,0xFF};
static const unsigned char bom_utf32_le[]={0xFF,0xFE,0x00,0x00};
static const unsigned char bom_utf32_be[]={0x00,0x00,0xFE,0xFF};

template<typename T>
bool StringBuffer<T>::save(const String& file,int type)
{
	bool writebom=(type&FILE_TEXT_BOM)!=0;
	type=type&FILE_MASK;

	std::ofstream ofs(file.c_str());
	if(!ofs.good())
	{
		return false;
	}

	switch(type)
	{
	case FILE_BINARY:
		{
			ofs.write((char*)data(),sizeof(T)*size());
			return ofs.good();
		}
	case FILE_TEXT_ANSI:
		{
			if(sizeof(T)==1)
			{
				ofs.write((char*)data(),size());
				return ofs.good();
			}
			StringBuffer<char> sb;
			if(!CodeCvt<T>::ws2s(sb,data(),size()))
			{
				return false;
			}
			ofs.write((char*)sb.data(),sb.size());
			return ofs.good();
		}
		break;
	case FILE_TEXT:
	case FILE_TEXT_UTF8:
		{
#ifndef _WIN32
			if(sizeof(T)==1)
			{
				ofs.write((char*)data(),size());
				return ofs.good();
			}
#endif
			StringBuffer<char> sb;
			if(sizeof(T)==1)
			{
				if(!String::ansi_to_utf8(sb,*(StringBuffer<char>*)this))
				{
					return false;
				}
			}
			else if(sizeof(T)==sizeof(wchar_t))
			{
				if(!String::wstr_to_utf8(sb,*(StringBuffer<wchar_t>*)this))
				{
					return false;
				}
			}
			else
			{
				if(!CodeCvt<T>::ws2s(sb,data(),size()))
				{
					return false;
				}

				if(!String::ansi_to_utf8(sb,sb))
				{
					return false;
				}
			}

			if(writebom)
			{
				ofs.write((char*)bom_utf8,3);
			}

			ofs.write(sb.data(),sb.size());
			return ofs.good();
		}
		break;
	case FILE_TEXT_UTF16_BE:
	case FILE_TEXT_UTF16_LE:
		{

			StringBuffer<unsigned short> wb;
			if(sizeof(T)==1)
			{
				if(!CodeCvt<unsigned short>::s2ws(wb,(char*)data(),size()))
				{
					return false;
				}
			}
			else if(sizeof(T)==2)
			{
				wb.append((unsigned short*)data(),size());
			}
			else
			{
				return false;
			}


			if(type==FILE_TEXT_UTF16_BE)
			{
				char* p=(char*)wb.data();
				for(size_t i=0;i<wb.size();i++)
				{
					std::swap(p[2*i+0],p[2*i+1]);
				}
			}

			if(writebom)
			{
				ofs.write((char*)(type==FILE_TEXT_UTF16_BE?bom_utf16_be:bom_utf16_le),2);
			}

			ofs.write((char*)wb.data(),wb.size()*2);
			return ofs.good();

		}
		break;
	default:
		return false;
	};

    return true;
}


template<typename T>
bool StringBuffer<T>::load(const String& file,int type)
{
	std::ifstream ifs;

	type=type&FILE_MASK;

	if(type==FILE_BINARY)
	{
		ifs.open(file.c_str(),std::ios::in|std::ios::binary);
		if(!ifs.good()) return false;
		ifs.seekg(0,std::ios_base::end);
		size_t sz=(size_t)ifs.tellg();
		ifs.seekg(0,std::ios_base::beg);

		if(sz%sizeof(T)!=0)
		{
			System::LogTrace("invalid filesize:%u",sz);
			return false;
		}

		size_t size=sz/sizeof(T);
		resize(size);
		ifs.read((char*)data(),sz);
		return ifs.good();
	}


	ifs.open(file.c_str(),std::ios::in|std::ios::binary);
	if(!ifs.good()) return false;
	ifs.seekg(0,std::ios_base::end);
	size_t sz=(size_t)ifs.tellg();
	ifs.seekg(0,std::ios_base::beg);

	unsigned char bom[4]={1,1,1,1};
	ifs.read((char*)bom,4);

	StringBuffer<char> sb;
	if(bom[0]==0xEF && bom[1]==0xBB && bom[2]==0xBF) // UTF8
	{
		size_type df=3;
		ifs.seekg(df,std::ios_base::beg);
		size_type nz=sz-df;
		StringBuffer<char> kb;kb.resize(nz);
		ifs.read((char*)kb.data(),nz);

		if(!String::utf8_to_ansi(sb,kb.data(),nz))
		{
			return false;
		}

		if(sizeof(T)==1)
		{
			sb.swap(*(StringBuffer<char>*)this);
			return true;
		}

	}
	else if( (bom[0]==0xFE && bom[1]==0xFF)||(bom[0]==0xFF && bom[1]==0xFE))
	{
		if((sz&0x1)!=0)
		{
			System::LogTrace("invalid UTF-16 filesize:%u",sz);
			return false;
		}

		size_type df=2;
		ifs.seekg(df,std::ios_base::beg);
		size_type nz=sz-df;
		StringBuffer<unsigned short> kb;kb.resize(nz>>1);
		ifs.read((char*)kb.data(),nz);

		// CE D2
		// 11 62;
		// BD 70

		unsigned short tag=*(unsigned short*)bom;
		if(tag==0xFFFE) //BE
		{
			char *pc=(char*)kb.data();
			for(size_type i=0;i<nz;i+=2)
			{
				std::swap(pc[i],pc[i+1]);
			}
		}

		if(sizeof(T)==2)
		{
			kb.swap(*(StringBuffer<unsigned short>*)this);
			return true;
		}

		if(!CodeCvt<unsigned short>::ws2s(sb,kb.data(),nz>>1))
		{
			return false;
		}
	}
	else if( (bom[0]==0xFF && bom[1]==0xFE && bom[2]==0 && bom[3]==0)||(bom[0]==0 && bom[1]==0 && bom[2]==0xFE && bom[3]==0xFF))
	{
		if((sz&0x3)!=0)
		{
			System::LogTrace("invalid UTF-32 filesize:%u",sz);
			return false;
		}

		size_type df=4;
		ifs.seekg(df,std::ios_base::beg);
		size_type nz=sz-df;
		StringBuffer<unsigned int> kb;kb.resize(nz>>2);
		ifs.read((char*)kb.data(),nz);

		uint32_t tag=*(uint32_t*)bom;

		if(tag==0xFFFE0000) //BE
		{
			char* pc=(char*)kb.data();
			for(size_type i=0;i<nz;i+=4)
			{
				std::swap(pc[i+0],pc[i+3]);
				std::swap(pc[i+1],pc[i+2]);
			}
		}

		if(sizeof(T)==4)
		{
			kb.swap(*(StringBuffer<unsigned int>*)this);
			return true;
		}

		if(!CodeCvt<unsigned int>::ws2s(sb,kb.data(),nz>>2))
		{
			return false;
		}
	}
	else
	{
		size_type df=0;
		ifs.seekg(df,std::ios_base::beg);
		size_type nz=sz-df;
		sb.resize(nz);
		ifs.read((char*)sb.data(),nz);

		unsigned char* p=(unsigned char*)sb.data();
		int t=0;
		int n=0;

		for(size_type i=0;i<nz;i++)
		{
			unsigned c=p[i];
			if(c<0x80) continue;
			for(n=0;((c<<=1)&0x80)>0;n++);
			if(n>3) {t=-1;break;}

			if(i+n>=nz)
			{
				t=-1;break;
			}

			t=1;
			for(int j=1;j<=n;j++)
			{
				if(((p[i+j])&0xC0)!=0x80)
				{
					t=-1;
					break;
				}
			}
			i+=n;
		}

		if(t==1)
		{
			if(!String::utf8_to_ansi(sb,sb))
			{
				return false;
			}
		}
	}

	if(sizeof(T)==1)
	{
		sb.swap(*(StringBuffer<char>*)this);
	}
	else
	{
		if(!CodeCvt<T>::s2ws(*this,sb.data(),sb.size()))
		{
			return false;
		}
	}
	return true;

}





template class StringBuffer<char>;
template class StringBuffer<unsigned char>;
template class StringBuffer<short>;
template class StringBuffer<unsigned short>;
template class StringBuffer<int>;
template class StringBuffer<unsigned int>;

template class StringBuffer<wchar_t>;





VHWD_LEAVE
