#include "vhwd/net/overlapped.h"
#include "vhwd/memory/mempool.h"


VHWD_ENTER

MyOverLapped::MyOverLapped(int t):type(t)
{
#ifdef _WIN32
	memset(&olap,0,sizeof(olap));
#endif
	dbuf[0].buf=NULL;
	dbuf[0].len=0;
	dbuf[1].buf=NULL;
	dbuf[1].len=0;
}

void MyOverLappedRb::done()
{
	if(flag.exchange(0)!=1)
	{
		System::LogTrace("flag!=1 in done");
	}
}

void MyOverLappedRb::done_send(RingBufferBase& buff_send)
{
	if(size>=0)
	{
		buff_send.rd_flip(size);
	}
	else
	{
		System::LogTrace("size<0 in done_send");
	}

	done();
}

void MyOverLappedRb::done_recv(RingBufferBase& buff_recv)
{
		
	if(size>=0)
	{
		buff_recv.wr_flip(size);
	}
	else
	{
		System::LogTrace("size<0 in done_recv");
	}

	done();
}

bool MyOverLappedRb::init_send(RingBufferBase& buff_send)
{
	if(flag.exchange(1)!=0)
	{
		return false;
	}

	MyOverLapped& olap_send(*this);

	int32_t mask=buff_send.pHeader->mask;
	int32_t rd=buff_send.pHeader->rd_pos;
	int32_t wr=buff_send.pHeader->wr_pos;
	int32_t sz=(wr-rd)&mask;

	if(sz==0)
	{
		done();
		return false;
	}

	if(sz+rd>mask)
	{
		olap_send.dbuf[0].buf=buff_send.pBuffer+rd;
		olap_send.dbuf[0].len=mask+1-rd;
		olap_send.dbuf[1].buf=buff_send.pBuffer;
		olap_send.dbuf[1].len=sz+rd-mask-1;
	}
	else
	{
		olap_send.dbuf[0].buf=buff_send.pBuffer+rd;
		olap_send.dbuf[0].len=sz;
		olap_send.dbuf[1].buf=NULL;
	}

	return true;
}

bool MyOverLappedRb::init_recv(RingBufferBase& buff_recv)
{
	if(flag.exchange(1)!=0)
	{
		return false;
	}

	MyOverLapped& olap_recv(*this);

	int32_t mask=buff_recv.pHeader->mask;
	int32_t rd=buff_recv.pHeader->rd_pos;
	int32_t wr=buff_recv.pHeader->wr_pos;
	int32_t sz=(rd-wr-1)&mask;

	if(sz==0)
	{
		done();
		return false;
	}

	if(sz>1024*4)
	{
		sz=1024*4;
	}

	if(sz+wr>mask)
	{
		olap_recv.dbuf[0].buf=buff_recv.pBuffer+wr;
		olap_recv.dbuf[0].len=mask+1-wr;
		olap_recv.dbuf[1].buf=buff_recv.pBuffer;
		olap_recv.dbuf[1].len=sz+wr-mask-1;
	}
	else
	{
		olap_recv.dbuf[0].buf=buff_recv.pBuffer+wr;
		olap_recv.dbuf[0].len=sz;
		olap_recv.dbuf[1].buf=NULL;
	}

	return true;
}

MyOverLappedEx::MyOverLappedEx()
{
	buffer=(char*)MemPoolPaging::current().allocate(4096);
	if(!buffer)
	{
		Exception::XBadAlloc();
	}
	dbuf[0].buf=buffer;
	dbuf[0].len=0;
	dbuf[1].buf=NULL;
	dbuf[1].len=0;
}

MyOverLappedEx::~MyOverLappedEx()
{
	MemPoolPaging::current().deallocate(buffer);
}


void PerIO_socket::swap(PerIO_socket& sk)
{
	sock.swap(sk.sock);
	peer.swap(sk.peer);
	addr.swap(sk.addr);
}

VHWD_LEAVE
