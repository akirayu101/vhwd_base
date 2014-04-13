
#include"vhwd/serialization/serializer_buffer.h"

VHWD_ENTER

SerializerBuffer::SerializerBuffer()
{
	m_nGpos=0;
	m_nGend=0;
	m_nLast=0;
	m_pBuff=NULL;
}

bool SerializerBuffer::skip()
{
	bool flag=m_nGpos==m_nGend;
	m_nGpos=m_nGend=0;
	reader().clear();
	writer().clear();
	return flag;
}

// assign m_pBuffer
void SerializerBuffer::assign(void* pbuf,size_t size)
{
	m_pBuff=(char*)pbuf;
	m_nLast=size;
	m_nGpos=m_nGend=0;
}

// allocate buffer
void SerializerBuffer::alloc(size_t bufsize)
{
	m_aBuff.resize(bufsize);
	m_aBuff.shrink_to_fit();
	m_pBuff=m_aBuff.data();
	m_nLast=m_aBuff.size();
}

bool SerializerBuffer::_grow(size_t _newsize)
{
	if(m_pBuff!=m_aBuff.data())
	{
		return false;
	}
	m_aBuff.resize(_newsize);
	m_pBuff=m_aBuff.data();
	m_nLast=m_aBuff.size();
	return true;	
}

bool SerializerBuffer::send(char* data,size_t size)
{
	if(m_nGend+size>m_nLast && !_grow(m_nGend+size))
	{
		writer().errstr("no_buffer");
		return false;
	}

	memcpy(m_pBuff+m_nGend,data,size);
	m_nGend+=size;
	return true;
}

bool SerializerBuffer::recv(char* data,size_t size)
{
	if(m_nGpos+size>m_nGend)
	{
		reader().errstr("eof");
		return false;
	}
	memcpy(data,m_pBuff+m_nGpos,size);
	m_nGpos+=size;
	return true;
}

VHWD_LEAVE
