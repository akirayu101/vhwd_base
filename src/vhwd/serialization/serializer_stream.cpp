#include"vhwd/serialization/serializer_stream.h"

VHWD_ENTER

size_t SerializerReaderStream::recv(char* data,size_t size)
{
	fs.read(data,size);
	if(!fs.good())
	{
		errstr("read failed");
	}
	return fs.gcount();
}

size_t SerializerWriterStream::send(const char* data,size_t size)
{
	fs.write(data,size);
	if(!fs.good())
	{
		errstr("write failed");
	}
	return size;
}

VHWD_LEAVE
