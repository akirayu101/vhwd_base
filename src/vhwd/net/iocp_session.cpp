#include "vhwd/basic/system.h"
#include "vhwd/net/iocp_session.h"
#include "vhwd/net/iocp_pool.h"
#include "vhwd/threading/thread_mutex.h"

#ifndef _WIN32
#include <cerrno>
#endif

VHWD_ENTER

Session::Session()
{
	pbuf_manager.reset(&PerIO_manager::current());
}

Session::~Session()
{
	if(m_nBufferCount.get()!=0)
	{
		System::LogTrace("Session::m_nBufferCount>0 in destructor");
	}
}


#ifdef _WIN32
DWORD iocp_flag_recv=0;
#endif

void SessionTCP::AsyncSend(PerIO_buffer* perio_data)
{

	perio_data->type=Session::ACTION_TCP_SEND;
	perio_data->size=0;

#ifdef _WIN32
	int bRet=WSASend(
		sk_local.sock,
		&perio_data->dbuf,
		1,
		&perio_data->size,
		iocp_flag_recv,
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

void SessionTCP::AsyncRecv(PerIO_buffer* perio_data)
{

	perio_data->type=Session::ACTION_TCP_RECV;
	perio_data->dbuf=perio_data->hbuf;

#ifdef _WIN32
	int bRet=WSARecv(
		sk_local.sock,
		&perio_data->dbuf,
		1,
		&perio_data->size,
		&iocp_flag_recv,
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

void Session::OnSendComplted(PerIO_buffer* perio_data)
{
	PutBuffer(perio_data);
}

void Session::OnRecvComplted(PerIO_buffer* perio_data)
{
	PutBuffer(perio_data);
}

#ifndef _WIN32

void Session::ep_ctl(int f)
{


	struct epoll_event ev;
	ev.data.ptr=this;

	ev.events=f|EPOLLERR|EPOLLHUP|EPOLLET|EPOLLONESHOT;

	if(m_nAsyncPendingSend.load()>0)
	{
		ev.events|=EPOLLOUT;
	}
	if(m_nAsyncPendingRecv.load()>0)
	{
		ev.events|=EPOLLIN;
	}

	if(m_nLastEpoll_ctl!=(int)ev.events)
	{
		m_nLastEpoll_ctl=ev.events;
	}
	else if((m_nLastEpoll_ctl&EPOLLONESHOT)==0)
	{
		return;
	}

	int bRet=epoll_ctl(hiocp->native_handle(),EPOLL_CTL_MOD,sk_local.sock.native_handle(),&ev);
	if(bRet<0)
	{
        logger.LogError("epoll_ctl failed: %s",strerror(errno));
		//Disconnect();
	}

}
#endif


void SessionTCP::WaitForSend()
{
	PerIO_buffer* perio_data=&PerIO_manager::g_send_perio_data;
#ifdef _WIN32

	int bRet=WSASend(
		sk_local.sock,
		&perio_data->dbuf,
		1,
		&perio_data->size,
		iocp_flag_recv,
		&perio_data->olap,
		NULL
		);

	if(bRet!=0 && WSAGetLastError()!=WSA_IO_PENDING)
	{
		Disconnect();
		return;
	}
#else
	{
		PerIO_queue::LockType lock1(m_qSend);
		m_qSend.dq.push_back(perio_data);
	}
	ep_ctl(EPOLLOUT);

#endif

	m_nAsyncPending++;
	m_nAsyncPendingSend++;

}

void SessionTCP::WaitForRecv()
{

	PerIO_buffer* perio_data=&PerIO_manager::g_recv_perio_data;
#ifdef _WIN32

	int bRet=WSARecv(
		sk_local.sock,
		&perio_data->dbuf,
		1,
		&perio_data->size,
		&iocp_flag_recv,
		&perio_data->olap,
		NULL
		);

	if(bRet!=0 && WSAGetLastError()!=WSA_IO_PENDING)
	{
		Disconnect();
		return;
	}
#else
	{
		PerIO_queue::LockType lock1(m_qRecv);
		m_qRecv.dq.push_back(perio_data);
	}
	ep_ctl(EPOLLIN);

#endif

	m_nAsyncPendingRecv++;
	m_nAsyncPending++;
}




void Session::OnSendReady()
{

}

void Session::OnRecvReady()
{

}


void Session::OnConnected()
{

}

void Session::OnDisconnected()
{

}

PerIO_buffer* Session::GetBuffer(unsigned sz,int t)
{
	m_nBufferCount.fetch_add(1);
	return pbuf_manager->get(sz,t);
}

void Session::PutBuffer(PerIO_buffer* pdat)
{
	m_nBufferCount.fetch_sub(1);
	pbuf_manager->put(pdat);
}

void Session::SetIOCP(IOCPPool* p)
{
	hiocp.reset(p);
}

bool Session::IsConnected()
{
	return m_nBitFlags.load() & FLAG_CONNECTED;
}

void Session::Disconnect()
{
	if(!hiocp) return;

	AtomicInt32::type val=m_nBitFlags.load();
	if((val&(FLAG_REQEXIT|FLAG_CONNECTED))!=FLAG_CONNECTED)
	{
		return;
	}

	m_nAsyncPending++;

	if(!m_nBitFlags.compare_exchange(val,FLAG_REQEXIT|FLAG_CONNECTED))
	{
		m_nAsyncPending--;
		return;
	}

#ifdef _WIN32
	::PostQueuedCompletionStatus(hiocp->native_handle(),0,(ULONG_PTR)this,NULL);
#else
	sk_local.sock.Shutdown();
	ep_ctl(EPOLLIN|EPOLLOUT);
#endif

}


bool Session::TestTimeOut(const TimePoint& tp)
{
	return tlast<tp;
}

VHWD_LEAVE
