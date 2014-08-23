#include "vhwd/serialization/serializer.h"
#include "vhwd/basic/stringbuffer.h"
#include "vhwd/basic/codecvt.h"

VHWD_ENTER


template<typename A>
void serial_helper_func<A,String>::g(SerializerReader &ar,type &val)
{
	intarr_t n(0);
	serial_pod<A,intarr_t>::g(ar,n);
	arr_1t<char> vect;
	vect.resize(n);

	ar.checked_recv(vect.data(),n);

	if(ar.flags.get(Serializer::STRCVT_UTF8))
	{
		StringBuffer<char> sb;
		if(!IConv::utf8_to_ansi(sb,vect.data(),vect.size()))
		{
			ar.errstr("invalid string");
		}
		val=sb;
	}
	else
	{
		val.assign(vect.data(),n);
	}

}

template<typename A>
void serial_helper_func<A,String>::g(SerializerWriter &ar,type &val)
{
	const char *p;
	intarr_t n;

	if(ar.flags.get(Serializer::STRCVT_UTF8))
	{
		StringBuffer<char> sb;
		if(!IConv::ansi_to_utf8(sb,val.c_str(),val.size()))
		{
			ar.errstr("invalid string");
			return;
		}
		p=sb.data();
		n=(intarr_t)sb.size();
	}
	else
	{
		p=val.c_str();
		n=(intarr_t)val.size();
	}

	serial_pod<A,intarr_t>::g(ar,n);
	if(n>0)
	{
		ar.checked_send((char *)p,n);
	}

}

template class VHWD_DLLIMPEXP serial_helper_func<SerializerWriter,String>;
template class VHWD_DLLIMPEXP serial_helper_func<SerializerReader,String>;

Serializer::Serializer(int t)
	:type(t)
{
	gver=0;
}


SerializerEx::SerializerEx(int t)
	:Serializer(t)
	,object_creator(ObjectCreator::current())
{

}

serializer_cached_objects::serializer_cached_objects()
{
	m_aObjects.insert(NULL);
}

serializer_cached_objects::~serializer_cached_objects()
{

}

int32_t serializer_cached_objects::get(Object* dat)
{
	return get((void*)dat);
}

int32_t serializer_cached_objects::put(Object* dat)
{
	return put((void*)dat);
}

void *serializer_cached_objects::get(int32_t idx)
{
	if(idx<0||idx>=(int)m_aObjects.size())
	{
		return NULL;
	}

	return m_aObjects.get(idx);
}

int32_t serializer_cached_objects::get(void *dat_)
{
	return m_aObjects.find(dat_);
}

int32_t serializer_cached_objects::put(void *dat_)
{
	return m_aObjects.insert(dat_);
}

void serializer_cached_objects::clear()
{
	m_aObjects.clear();
	m_aObjects.insert(NULL);
}



SerializerReader &SerializerReader::tag(char ch)
{
	char tmp;
	checked_recv(&tmp,1);
	if(tmp==ch)
	{
		errstr("invalid_tag");
	}
	return *this;
}

SerializerReader &SerializerReader::tag(const char *msg)
{
	int n=::strlen(msg);
	std::vector<char> tmp;
	tmp.resize(n+1);
	checked_recv(&tmp[0],n+1);
	if(tmp[n]!='\0'||strcmp(msg,&tmp[0])!=0)
	{
		errstr("invalid_tag");
	}
	return *this;
}

int SerializerReader::local_version(int v)
{
	int32_t vh(-1);
	checked_recv((char *)&vh,sizeof(int32_t));
	if(vh<0||vh>v)
	{
		errstr("invalid_version");
		return -1;
	}
	return vh;
}

SerializerWriter &SerializerWriter::tag(char ch)
{
	checked_send(&ch,1);
	return *this;
}

SerializerWriter &SerializerWriter::tag(const char *msg)
{
	int n=::strlen(msg);
	checked_send((char *)msg,n+1);
	return *this;
}

int SerializerWriter::local_version(int v)
{
	int32_t vh(v);
	checked_send((char *)&vh,sizeof(int32_t));
	return v;
}

VHWD_LEAVE
