#include"vhwd/serialization/serializer_stream.h"

VHWD_ENTER

void SerializerReaderStream::recv(char* data,size_t size)
{
	fs.read(data,size);
	if(!fs.good())
	{
		errstr("read failed");
	}
}

void SerializerWriterStream::send(char* data,size_t size)
{
	fs.write(data,size);
	if(!fs.good())
	{
		errstr("write failed");
	}
}

VHWD_LEAVE
