
#include"vhwd/serialization/serializer_stream.h"

VHWD_ENTER

bool SerializerReaderStream::recv(char* data,size_t size)
{
	fs.read(data,size);
	return fs.good();
}

bool SerializerWriterStream::send(char* data,size_t size)
{
	fs.write(data,size);
	return fs.good();
}

VHWD_LEAVE
