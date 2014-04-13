
#include"vhwd/serialization/serializer_socket.h"

VHWD_ENTER

SerializerReaderSocket::SerializerReaderSocket()
{

}

bool SerializerReaderSocket::sync()
{

	int n=abuf.last-abuf.gend;

	if(n>0)
	{
		memcpy(&abuf.buff[0],&abuf.buff[abuf.gend],n);
		abuf.gend=n;
	}
	else
	{
		abuf.last=abuf.gend=0;
		abuf.buff.resize(1024);
	}


	while(abuf.gend<4)
	{
		int k=sock.Recv((char*)&abuf.buff[abuf.gend],1024-abuf.gend);
		if(k<=0) return false;
		abuf.gend+=k;
	}

	size_t s=(size_t) *(int32_t*)&abuf.buff[0];

	if(abuf.gend>s)
	{
		abuf.last=abuf.gend;
		abuf.gpos=4;
		abuf.gend=s;
		return true;
	}

	if(s>abuf.buff.size())
	{
		abuf.buff.resize(s);
	}

	while(abuf.gend<s)
	{
		int k=sock.Recv((char*)&abuf.buff[n],s-abuf.gend);
		if(k<=0) return false;
		abuf.gend+=k;
	}

	abuf.gpos=4;
	abuf.gend=s;
	abuf.last=s;

	return true;
}

bool SerializerReaderSocket::test()
{
	int32_t s;
	return sock.Peek((char*)&s,sizeof(s))>0;
}

bool SerializerWriterSocket::sync()
{
	*(int32_t*)&abuf.buff[0]=abuf.buff.size();
	bool flag=sock.SendMsg((char*)&abuf.buff[0],abuf.buff.size())>0;
	abuf.buff.resize(4);
	clear();
	return flag;
}

SerializerWriterSocket::SerializerWriterSocket()
{
	skip();
}

void SerializerWriterSocket::skip()
{
	abuf.buff.resize(4);
}

VHWD_LEAVE
