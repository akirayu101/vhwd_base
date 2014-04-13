#include "vhwd/net/iocp_session.h"
#include "vhwd/net/iocp_pool.h"

VHWD_ENTER

extern PerIO_buffer g_send_perio_data;
extern PerIO_buffer g_recv_perio_data;

#ifdef _WIN32
extern DWORD iocp_flag_recv;
#endif

void SessionUDP::AsyncSend(PerIO_buffer* perio_data)
{
    perio_data->size=0;
	perio_data->type=Session::ACTION_UDP_SEND;
#ifdef _WIN32
	int bRet=WSASendTo(
		sk_local.sock,
		&perio_data->dbuf,
		1,
		&perio_data->size,
		iocp_flag_recv,
		perio_data->peer.sk_addr(),
		perio_data->peer.v4size(),
		&perio_data->olap,
		NULL
		);

	if(bRet!=0 && WSAGetLastError()!=WSA_IO_PENDING)
	{
		PutBuffer(perio_data);
		Disconnect();
		return;
	}
#else
	{
		PerIO_queue::LockType lock1(m_qSend);
		m_qSend.dq.push_back(perio_data);
		if(m_nAsyncPending.load()==0)
		{
			ep_ctl(EPOLLOUT);
		}
	}
#endif

	m_nAsyncPendingSend++;
	m_nAsyncPending++;
}

void SessionUDP::AsyncRecv(PerIO_buffer* perio_data)
{

	perio_data->type=Session::ACTION_UDP_RECV;
	perio_data->dbuf.len=perio_data->hbuf.len;

#ifdef _WIN32
	int bRet=WSARecvFrom(
		sk_local.sock,
		&perio_data->dbuf,
		1,
		&perio_data->size,
		&iocp_flag_recv,
		perio_data->peer.sk_addr(),
		perio_data->peer.sz_ptr(),
		&perio_data->olap,
		NULL
		);

	if(bRet!=0 && WSAGetLastError()!=WSA_IO_PENDING)
	{
		PutBuffer(perio_data);
		Disconnect();
		return;
	}
#else
	{

		PerIO_queue::LockType lock1(m_qRecv);
		m_qRecv.dq.push_back(perio_data);
		if(m_nAsyncPending.load()==0)
		{
			ep_ctl(EPOLLIN);
		}
	}
#endif

	m_nAsyncPendingRecv++;
	m_nAsyncPending++;
}


void SessionUDP::OnConnected()
{

}

void SessionUDP::OnDisconnected()
{

}

bool SessionUDP::Bind(const String& ip,int port)
{
	IPAddress addr(ip,port);
	return Bind(addr);
}


bool SessionUDP::Bind(IPAddress& addr)
{
	if(IsConnected()||!hiocp)
	{
		return false;
	}

	sk_local.sock.Ensure(Socket::UDP);
	if(!sk_local.sock.Bind(addr))
	{
		return false;
	}

	hiocp->svc_add(this);
	return true;
}


VHWD_LEAVE
