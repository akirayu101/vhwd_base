
#include"vhwd/serialization/serializer_buffer.h"

VHWD_ENTER

SerializerBuffer::SerializerBuffer()
{

}

bool SerializerBuffer::skip()
{
	return lbuf.skip();
}

// assign m_pBuffer
void SerializerBuffer::assign(char* pbuf,size_t size)
{
	lbuf.assign(pbuf,size);
}

// allocate buffer
void SerializerBuffer::alloc(size_t bufsize)
{
	lbuf.alloc(bufsize);
}


bool SerializerBuffer::send(char* data,size_t size)
{
	if(lbuf.send(data,size)==size)
	{
		return true;
	}
	else
	{
		writer().errstr("no_buffer");
		return false;
	}
}

bool SerializerBuffer::recv(char* data,size_t size)
{
	if(lbuf.recv(data,size)==size)
	{
		return true;
	}
	else
	{
		reader().errstr("eof");
		return false;
	}
}

VHWD_LEAVE
