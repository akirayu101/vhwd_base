
#include"vhwd/serialization/serializer.h"

VHWD_ENTER


template<typename A>
void serial_helper_func<A,String>::g(SerializerReader &ar,type &val)
{
	arr_1t<char> vect;
	ar >> vect;		
	val.assign(vect.data(),vect.size());
}

template<typename A>
void serial_helper_func<A,String>::g(SerializerWriter &ar,type &val)
{
	const char *s=val.c_str();
	int64_t _size=(int64_t)val.size();
	ar << _size;
	if(_size>0)
	{
		ar.send((char *)s,_size);
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
	m_aObjects.set_capacity(512);
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
	recv(&tmp,1);
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
	recv(&tmp[0],n+1);
	if(tmp[n]!='\0'||strcmp(msg,&tmp[0])!=0)
	{
		errstr("invalid_tag");
	}
	return *this;
}

int SerializerReader::local_version(int v)
{
	int32_t vh(-1);
	recv((char *)&vh,sizeof(int32_t));
	if(vh<0||vh>v)
	{
		errstr("invalid_version");
		return -1;
	}
	return vh;
}

SerializerWriter &SerializerWriter::tag(char ch)
{
	send(&ch,1);
	return *this;
}

SerializerWriter &SerializerWriter::tag(const char *msg)
{
	int n=::strlen(msg);
	send((char *)msg,n+1);
	return *this;
}

int SerializerWriter::local_version(int v)
{
	int32_t vh(v);
	return send((char *)&vh,sizeof(int32_t));
}

VHWD_LEAVE
