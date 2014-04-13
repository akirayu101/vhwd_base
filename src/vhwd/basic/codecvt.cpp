
#include "vhwd/basic/codecvt.h"
#include "vhwd/basic/system.h"
#include "vhwd/basic/stringbuffer.h"

VHWD_ENTER


template<typename T>
CodeCvt<T>::CodeCvt():loc("")
{

}

template<typename T>
void CodeCvt<T>::Init(const String& loc_name)
{
	loc=std::locale(loc_name.c_str());
}

template<typename T>
CodeCvt<T>& CodeCvt<T>::current()
{
	static CodeCvt<T> gInstance;
	return gInstance;
}

CodeCvt<char>& CodeCvt<char>::current()
{
	static CodeCvt<char> gInstance;
	return gInstance;
}

CodeCvt<unsigned char>& CodeCvt<unsigned char>::current()
{
	static CodeCvt<unsigned char> gInstance;
	return gInstance;
}



template<typename T>
bool CodeCvt<T>::s2ws(StringBuffer<intern_type>& si,const extern_type* se,size_t ln)
{
	const facet_type& facet_string = std::use_facet<facet_type>(loc);
	std::mbstate_t mystate = std::mbstate_t();

	size_t sn=(sizeof(extern_type)>sizeof(intern_type)?ln*sizeof(extern_type)/sizeof(intern_type):ln);
	si.resize(sn+1);

	const extern_type* pac;
	intern_type* pwc=&si[sn];

	typename facet_type::result myresult = facet_string.in (mystate,
		&se[0],&se[ln], pac,
		&si[0],&si[sn], pwc);

	si.resize(pwc-&si[0]);

	if ( myresult != facet_type::ok )
	{
		System::LogTrace("CodeCvt::s2ws failed: ret=%d",(int)myresult);
		return false;
	}

	return true;

}

template<typename T>
bool CodeCvt<T>::ws2s(StringBuffer<extern_type>& pa,const intern_type* pw,size_t ln)
{
	const facet_type& facet_string = std::use_facet<facet_type>(loc);
	std::mbstate_t mystate = std::mbstate_t();

	size_t sn=(sizeof(intern_type)>sizeof(extern_type)?ln*sizeof(intern_type)/sizeof(extern_type):ln);
	pa.resize(sn+1);

	extern_type* pac=&pa[sn];
	const intern_type* pwc;

	typename facet_type::result myresult = facet_string.out (mystate,
		&pw[0],&pw[ln], pwc,
		&pa[0],&pa[sn], pac);

	size_t kn=pac-&pa[0];
	pa.resize(kn);

	if ( myresult != facet_type::ok )
	{		
		System::LogTrace("CodeCvt::ws2s failed: ret=%d",(int)myresult);
		return false;
	}	

	return true;
}


template class CodeCvt<int>;
template class CodeCvt<unsigned int>;
template class CodeCvt<short>;
template class CodeCvt<unsigned short>;

template class CodeCvt<wchar_t>;

VHWD_LEAVE
