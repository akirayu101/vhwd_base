
#include"vhwd/serialization/serializer_socket.h"

VHWD_ENTER

SerializerSocket::SerializerSocket()
{

}

bool SerializerSocket::rd_sync()
{

	int32_t sz(0);
	if(sock.Recv((char*)&sz,4)!=4)
	{
		return false;
	}

	if(sz<0)
	{
		return false;
	}

	lbuf_rd.rewind(sz);
	if(sock.RecvMsg(lbuf_rd.gbeg(),sz)!=sz)
	{
		return false;
	}

	lbuf_rd.wr_flip(sz);
	return true;
}

bool SerializerSocket::rd_test()
{
	int32_t s;
	return sock.Peek((char*)&s,sizeof(s))>0;
}

bool SerializerSocket::wr_sync()
{
	int32_t sz=lbuf_wr.rd_free();

	if(sock.Send((char*)&sz,4)!=4)
	{
		return false;
	}

	if(sock.SendMsg((char*)lbuf_wr.gbeg(),sz)!=sz)
	{
		return false;
	}

	wr_skip();
	return true;
}

void SerializerSocket::wr_skip()
{
	lbuf_wr.rewind();
}

void SerializerSocket::recv(char* data,size_t size_)
{
	if(size_!=(size_t)lbuf_rd.recv(data,size_))
	{
		writer().errstr("SerializerSocket::recv error");
	}
}

void SerializerSocket::send(char* data,size_t size_)
{
	if(size_==(size_t)lbuf_wr.send(data,size_))
	{
		reader().errstr("SerializerSocket::send error");
	}
}

VHWD_LEAVE
