
#include "vhwd/basic/codecvt.h"
#include "vhwd/basic/system.h"
#include "vhwd/basic/stringbuffer.h"

VHWD_ENTER

template<size_t ct> class MyCharType;
template<> class MyCharType<1>{public:typedef char type;};
template<> class MyCharType<2>{public:typedef unsigned short type;};
template<> class MyCharType<4>{public:typedef unsigned int type;};

//#ifdef _WIN32

template<typename T>
class CodeCvtImpl
{
public:
	typedef T char_wcs;
	typedef char char_mbs;

	CodeCvtImpl():loc(""){}

	static CodeCvtImpl& current(){static CodeCvtImpl gInstance;return gInstance;}

	bool s2ws(StringBuffer<char_wcs>& si_,const char_mbs* se_,size_t ln_)
	{

		typedef std::codecvt<char_wcs,char_mbs,std::mbstate_t> facet_type;

		const facet_type& facet_string = std::use_facet<facet_type>(loc);
		std::mbstate_t mystate = std::mbstate_t();

		size_t sn=(sizeof(char_mbs)>sizeof(char_wcs)?ln_*sizeof(char_mbs)/sizeof(char_wcs):ln_);
		si_.resize(sn+1);

		const char_mbs* pac;
		char_wcs* pwc=&si_[sn];

		typename facet_type::result myresult = facet_string.in (mystate,
			&se_[0],&se_[ln_], pac,
			&si_[0],&si_[sn], pwc);

		si_.resize(pwc-&si_[0]);

		if ( myresult != facet_type::ok )
		{
			System::LogTrace("CodeCvt::s2ws failed: ret=%d",(int)myresult);
			return false;
		}

		return true;
	}

	bool ws2s(StringBuffer<char_mbs>& pa_,const char_wcs* pw_,size_t ln_)
	{
		const char_wcs* pw=(const char_wcs*)pw_;
		typedef std::codecvt<char_wcs,char_mbs,std::mbstate_t> facet_type;

		const facet_type& facet_string = std::use_facet<facet_type>(loc);
		std::mbstate_t mystate = std::mbstate_t();

		size_t sn=(sizeof(char_wcs)>sizeof(char_mbs)?ln_*sizeof(char_wcs)/sizeof(char_mbs):ln_);
		pa_.resize(sn+1);

		char_mbs* pac=&pa_[sn];
		const char_wcs* pwc;

		typename facet_type::result myresult = facet_string.out (mystate,
			&pw[0],&pw[ln_], pwc,
			&pa_[0],&pa_[sn], pac);

		size_t kn=pac-&pa_[0];
		pa_.resize(kn);

		if ( myresult != facet_type::ok )
		{
			System::LogTrace("CodeCvt::ws2s failed: ret=%d",(int)myresult);
			return false;
		}

		return true;
	}

private:
	std::locale loc;
};
//
//#else
//
//template<typename T>
//class CodeCvtImpl;
//
//template<>
//class CodeCvtImpl<unsigned short>
//{
//public:
//	typedef unsigned short char_wcs;
//	typedef char char_mbs;
//
//	CodeCvtImpl(){}
//
//	static CodeCvtImpl& current(){static CodeCvtImpl gInstance;return gInstance;}
//
//	bool s2ws(StringBuffer<char_wcs>& pw_,const char_mbs* pa_,size_t ln_)
//	{
//
//		size_t sn=(sizeof(char_mbs)>sizeof(char_wcs)?ln_*sizeof(char_mbs)/sizeof(char_wcs):ln_);
//		pw_.resize(sn+1);
//
//		size_t result;
//		iconv_t env;
//		env = iconv_open("UCS-2-INTERNAL","UTF-8");
//		if (env==(iconv_t)-1)
//		{
//
//			return false;
//		}
//
//		char_wcs* pw_wcs=pw_.data();
//		size_t ln_wcs=pw_.size();
//		const char_mbs* pa_mbs=pa_;
//		size_t ln_mbs=ln_;
//
//		result = iconv(env,(char**)&pa_mbs,(size_t*)&ln_mbs,(char**)&pw_wcs,(size_t*)&ln_wcs);
//		if (result==(size_t)-1)
//		{
//			printf("iconv error%s %d\n",strerror(errno),errno) ;
//			return false;
//		}
//
//		iconv_close(env);
//		return true;
//
//	}
//
//	bool ws2s(StringBuffer<char_mbs>& pa_,const char_wcs* pw_,size_t ln_)
//	{
//
//
//		size_t sn=(sizeof(char_wcs)>sizeof(char_mbs)?ln_*sizeof(char_wcs)/sizeof(char_mbs):ln_);
//
//		pa_.resize(sn+1);
//		size_t result;
//		iconv_t env;
//		env = iconv_open("UTF-8","UCS-2-INTERNAL");
//		if (env==(iconv_t)-1)
//		{
//			return false;
//		}
//
//		char_mbs* pa_mbs=pa_.data();
//		size_t ln_mbs=pa_.size();
//		const char_wcs* pw_wcs=pw_;
//		size_t ln_wcs=ln_;
//
//		result = iconv(env,(char**)&pw_wcs,(size_t*)&ln_wcs,(char**)&pa_mbs,(size_t*)&ln_mbs);
//		if (result==(size_t)-1)
//		{
//			printf("iconv error%s %d\n",strerror(errno),errno) ;
//			return false;
//		}
//		iconv_close(env);
//		return true;
//	}
//};
//
//
//template<>
//class CodeCvtImpl<unsigned int>
//{
//public:
//	typedef unsigned int char_wcs;
//	typedef char char_mbs;
//
//	CodeCvtImpl(){}
//
//	static CodeCvtImpl& current(){static CodeCvtImpl gInstance;return gInstance;}
//
//	bool s2ws(StringBuffer<char_wcs>& pw_,const char_mbs* pa_,size_t ln_)
//	{
//
//		size_t sn=(sizeof(char_mbs)>sizeof(char_wcs)?ln_*sizeof(char_mbs)/sizeof(char_wcs):ln_);
//		pw_.resize(sn+1);
//
//		size_t result;
//		iconv_t env;
//		env = iconv_open("WCHAR_T","UTF-8");
//		if (env==(iconv_t)-1)
//		{
//			printf("iconv error%s %d\n",strerror(errno),errno) ;
//			return false;
//		}
//
//		char_wcs* pw_wcs=pw_.data();
//		size_t ln_wcs=pw_.size();
//		const char_mbs* pa_mbs=pa_;
//		size_t ln_mbs=ln_;
//
//		result = iconv(env,(char**)&pa_mbs,(size_t*)&ln_mbs,(char**)&pw_wcs,(size_t*)&ln_wcs);
//		if (result==(size_t)-1)
//		{
//			return false;
//		}
//		iconv_close(env);
//		return true;
//
//	}
//
//	bool ws2s(StringBuffer<char_mbs>& pa_,const char_wcs* pw_,size_t ln_)
//	{
//
//		size_t sn=(sizeof(char_wcs)>sizeof(char_mbs)?ln_*sizeof(char_wcs)/sizeof(char_mbs):ln_);
//
//		pa_.resize(sn+1);
//		size_t result;
//		iconv_t env;
//		env = iconv_open("UTF-8","WCHAR_T");
//		if (env==(iconv_t)-1)
//		{
//			return false;
//		}
//
//		char_mbs* pa_mbs=pa_.data();
//		size_t ln_mbs=pa_.size();
//		const char_wcs* pw_wcs=pw_;
//		size_t ln_wcs=ln_;
//
//		result = iconv(env,(char**)&pw_wcs,(size_t*)&ln_wcs,(char**)&pa_mbs,(size_t*)&ln_mbs);
//		if (result==(size_t)-1)
//		{
//			printf("iconv error%s %d\n",strerror(errno),errno) ;
//			return false;
//		}
//		iconv_close(env);
//		return true;
//	}
//};
//
//#endif



template<>
class CodeCvtImpl<char>
{
public:
	typedef char char_wcs;
	typedef char char_mbs;

	static CodeCvtImpl& current(){static CodeCvtImpl gInstance;return gInstance;}

	bool s2ws(StringBuffer<char_wcs>& sb,const char_mbs* p1,size_t ln_)
	{
		sb.assign(p1,ln_);
		return true;
	}
	bool ws2s(StringBuffer<char_mbs>& sb,const char_wcs* p1,size_t ln_)
	{
		sb.assign(p1,ln_);
		return true;
	}
};

template<typename T>
bool CodeCvt<T>::s2ws(StringBuffer<char_wcs>& si_,const char_mbs* se_,size_t ln_)
{
	typedef typename MyCharType<sizeof(char_wcs)>::type mychar_w;
	typedef typename MyCharType<sizeof(char_mbs)>::type mychar_s;
	return CodeCvtImpl<mychar_w>::current().s2ws((StringBuffer<mychar_w>&)si_,(const mychar_s*)se_,ln_);
}

template<typename T>
bool CodeCvt<T>::ws2s(StringBuffer<char_mbs>& pa_,const char_wcs* pw_,size_t ln_)
{
	typedef typename MyCharType<sizeof(char_wcs)>::type mychar_w;
	typedef typename MyCharType<sizeof(char_mbs)>::type mychar_s;
	return CodeCvtImpl<mychar_w>::current().ws2s((StringBuffer<mychar_s>&)pa_,(const mychar_w*)pw_,ln_);
}


template class CodeCvt<char>;
template class CodeCvt<unsigned char>;
template class CodeCvt<wchar_t>;
template class CodeCvt<short>;
template class CodeCvt<int>;
template class CodeCvt<unsigned short>;
template class CodeCvt<unsigned int>;


VHWD_LEAVE
