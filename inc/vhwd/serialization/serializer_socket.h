#ifndef __H_VHWD_SERIALIZER_SOCKET__
#define __H_VHWD_SERIALIZER_SOCKET__

#include "vhwd/serialization/serializer.h"
#include "vhwd/net/ipaddress.h"
#include "vhwd/net/socket.h"

VHWD_ENTER



class VHWD_DLLIMPEXP serializer_buffer
{
public:
	
	bool send(char* data,size_t size)
	{
		buff.append(data,size);
		return true;
	}

	bool recv(char* data,size_t size)
	{
		if(gpos+size>gend)
		{
			return false;
		}
		memcpy(data,&buff[gpos],size);
		gpos+=size;
		return true;
	}

	serializer_buffer()
	{
		gpos=0;
		gend=0;
		last=0;
		buff.reserve(1024);
	}

	arr_1t<char> buff;
	size_t gpos;
	size_t gend;
	size_t last;
};

// Usage:
// SerializerReaderSocket ar;
// // initialize ar
// if(ar.sync()) // recv data from socket and store to buffer.
// {
//	 ar & obj & ...; // read data from buffer.
//	 // call ar.test() to test read position reach the end of the buffer.
// }

class VHWD_DLLIMPEXP SerializerReaderSocket : public SerializerReader
{
public:

	SerializerReaderSocket();

	void close(){sock.Close();}

	// read data from socket and store to buffer.
	bool sync();

	// test if read position reach the end of the buffer
	bool test();

	Socket sock;

protected:
	serializer_buffer abuf;
	bool recv(char* data,size_t size){return abuf.recv(data,size);}
};


// Usage:
// SerializerWriterSocket ar;
// // initialize ar
// ar & obj & ...; // write data to buffer.
// ar.sync() // send buffer to socket and clear buffer.

class VHWD_DLLIMPEXP SerializerWriterSocket : public SerializerWriter
{
public:

	SerializerWriterSocket();

	void close(){sock.Close();}

	// send buffer to socket and clear buffer.
	bool sync();

	// clear buffer
	void skip();

	Socket sock;
protected:
	serializer_buffer abuf;
	bool send(char* data,size_t size){return abuf.send(data,size);}
};


VHWD_LEAVE
#endif

