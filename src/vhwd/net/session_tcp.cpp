#include "vhwd/basic/system.h"
#include "vhwd/basic/console.h"
#include "vhwd/net/session.h"
#include "vhwd/net/iocp1.h"
#include "vhwd/threading/thread_mutex.h"


#ifndef _WIN32
#include <cerrno>
#endif

VHWD_ENTER

SessionTCP::SessionTCP()
{

}

bool SessionTCP::AsyncSend(const char* data,size_t size)
{
	if(size>4096)
	{
		System::LogTrace("size>4096 in %s",__FUNCTION__);
		return false;
	}

	TempPtrT<MyOverLappedEx> q=lkfq_free.getq();
	if(!q)
	{
		q.reset(new MyOverLappedEx);
	}

	q->size=size;
	memcpy(q->buffer,data,size);
	return AsyncSend(q);
}

bool SessionTCP::AsyncSend(MyOlapPtr& q)
{
	if(!q)
	{
		return false;
	}

	MyOverLappedEx &idat(*q);
	idat.type=MyOverLapped::ACTION_TCP_SEND;
	idat.dbuf[0].buf=idat.buffer;
	idat.dbuf[0].len=idat.size;

	return DoAsyncSend(q);

}

bool SessionTCP::DoAsyncSend(MyOlapPtr& q)
{

	if(!lkfq_send.putq(q))
	{
		System::LogTrace("lkfq_send is full, packet dropped!");
		return false;
	}
	else
	{
		if(m_nPendingSend++==0)
		{
			DoAsyncSend();
		}
		return true;
	}
}

bool SessionTCP::DoAsyncRecv(MyOlapPtr& q)
{
	if(!lkfq_recv.putq(q))
	{
		System::LogTrace("lkfq_send is full, packet dropped!");
		return false;
	}
	else
	{
		if(m_nPendingRecv++==0)
		{
			DoAsyncRecv();
		}
		return true;
	}
}

bool SessionTCP::AsyncRecv(MyOlapPtr& q)
{
	if(!q)
	{
		return false;
	}

	MyOverLappedEx &idat(*q);
	idat.type=MyOverLapped::ACTION_TCP_RECV;
	idat.dbuf[0].buf=idat.buffer;
	idat.dbuf[0].len=4096;

	return DoAsyncRecv(q);

}

bool SessionTCP::AsyncRecv()
{
	TempPtrT<MyOverLappedEx> q=lkfq_free.getq();
	if(!q)
	{
		q.reset(new MyOverLappedEx);
	}
	return AsyncRecv(q);
}




bool SessionTCP::WaitForSend()
{

	TempPtrT<MyOverLappedEx> q=lkfq_free.getq();
	if(!q)
	{
		q.reset(new MyOverLappedEx);
	}
	MyOverLappedEx &idat(*q);
	idat.type=MyOverLapped::ACTION_WAIT_SEND;
	idat.dbuf[0].buf=idat.buffer;
	idat.dbuf[0].len=0;

	return DoAsyncSend(q);
}

bool SessionTCP::WaitForRecv()
{

	TempPtrT<MyOverLappedEx> q=lkfq_free.getq();
	if(!q)
	{
		q.reset(new MyOverLappedEx);
	}
	MyOverLappedEx &idat(*q);
	idat.type=MyOverLapped::ACTION_WAIT_RECV;
	idat.dbuf[0].buf=idat.buffer;
	idat.dbuf[0].len=0;

	return DoAsyncRecv(q);

}

#ifdef _WIN32
static DWORD iocp_flag_recv=0;
#endif


void SessionTCP::DoAsyncSend()
{

#ifdef _WIN32

	TempPtrT<MyOverLappedEx> q=lkfq_send.getq();
	if(!q)
	{
		return;
	}

	MyOverLappedEx &idat(*q);

	int bRet=WSASend(sk_local.sock,
			&idat.dbuf[0],
			idat.dbuf[1].buf?2:1,
			&idat.size,
			0,
			&idat.olap,
			NULL);

	if(bRet!=0 && WSAGetLastError()!=WSA_IO_PENDING)
	{
		--m_nPendingSend;
		Disconnect();
	}
	else
	{
		q.release();
	}

#else

     ep_ctl(EPOLLOUT);

#endif
}


void SessionTCP::DoAsyncRecv()
{

#ifdef _WIN32

	TempPtrT<MyOverLappedEx> q=lkfq_recv.getq();
	if(!q)
	{
		return;
	}

	MyOverLappedEx &idat(*q);

	int bRet=WSARecv(sk_local.sock,
		&idat.dbuf[0],
		idat.dbuf[1].buf?2:1,
		&idat.size,
		&iocp_flag_recv,
		&idat.olap,
		NULL);

	if(bRet!=0 && WSAGetLastError()!=WSA_IO_PENDING)
	{
		--m_nPendingRecv;
		Disconnect();
		return;
	}
	else
	{
		q.release();
	}

#else

    ep_ctl(EPOLLIN);

#endif
}



VHWD_LEAVE
