
#include "vhwd/net/iocp_session.h"
#include "vhwd/net/iocp_pool.h"
#include "vhwd/basic/system.h"

VHWD_ENTER

SessionServer::SessionServer()
{

}

bool SessionServer::Listen(const String& ip,int port)
{
	if(IsConnected())
	{
		logger.LogError("Listen failed! Server is already connected!");
		return false;
	}

	if(!hiocp)
	{
		logger.LogError("Listen failed! IOCPPool is null!");
		return false;
	}

	sk_local.addr.service(ip,port);
	if(!sk_local.sock.Listen(sk_local.addr))
	{
		logger.LogError("Listen failed on %s:%d",sk_local.addr.host(),sk_local.addr.port());
		return false;
	}

	sk_local.sock.Block(false);

#ifdef _WIN32
	DWORD dwBytes;
	DWORD iResult;

	GUID GuidAcceptEx = WSAID_ACCEPTEX;

	iResult = WSAIoctl(sk_local.sock, SIO_GET_EXTENSION_FUNCTION_POINTER,
				&GuidAcceptEx, sizeof (GuidAcceptEx),
				&lpfnAcceptEx, sizeof (lpfnAcceptEx),
				&dwBytes, NULL, NULL);

	if (iResult == SOCKET_ERROR)
	{
		int ret=WSAGetLastError();
		System::LogTrace("WSAIoctl(WSAID_ACCEPTEX) failed with error %d",ret);
		return false;
	}

	GUID GuidGetAddrEx = WSAID_GETACCEPTEXSOCKADDRS;

	iResult = WSAIoctl(sk_local.sock, SIO_GET_EXTENSION_FUNCTION_POINTER,
				&GuidGetAddrEx, sizeof (GuidGetAddrEx),
				&lpfnGetAddrEx, sizeof (lpfnGetAddrEx),
				&dwBytes, NULL, NULL);

	if (iResult == SOCKET_ERROR)
	{
		int ret=WSAGetLastError();
		System::LogTrace("WSAIoctl(WSAID_GETACCEPTEXSOCKADDRS) failed with error %d",ret);
		return false;
	}

#endif

	hiocp->svc_add(this);

	return true;
}

void SessionServer::OnConnected()
{
	WaitForAccept();
}

void SessionServer::OnDisconnected()
{
	basetype::OnDisconnected();
}

void SessionServer::Disconnect()
{
	basetype::Disconnect();
}

void SessionServer::Close()
{
	sk_local.sock.Close();
}


void SessionServer::StartSession(Session* ses,IOCPPool* h)
{
	if(!h)
	{
		h=hiocp;
	}
	ses->SetIOCP(h);
	h->svc_add(ses);
}

void SessionServer::WaitForAccept()
{
#ifdef _WIN32
	PerIO_accept* perio_data=(PerIO_accept*)&sk_remote;

	perio_data->sk_accept.sock.Ensure();
	perio_data->type=Session::ACTION_WAIT_RECV;

	int bRet = lpfnAcceptEx(
		sk_local.sock,
		perio_data->sk_accept.sock,
		perio_data->dbuf.buf,
		0,
		sizeof(sockaddr_in) + 16,
		sizeof(sockaddr_in) + 16,
		&perio_data->size,
		&perio_data->olap);

	if(bRet!=0 && WSAGetLastError()!=WSA_IO_PENDING)
	{
		int ret=WSAGetLastError();
		System::LogTrace("AcceptEx failed with error %d",ret);
		Disconnect();
		return;
	}
	m_nAsyncPending++;

#else
	WaitForRecv();
#endif

}


void SessionServer::OnRecvReady()
{

#ifndef _WIN32

	sk_remote.sk_accept.sock.Ensure();
	if(sk_local.sock.Accept(sk_remote.sk_accept.sock,sk_remote.sk_accept.peer))
	{
		NewSession(sk_remote.sk_accept);
	}

#else

	PerIO_buffer* perio_data=&sk_remote;

	sockaddr_in* addr=NULL;
	sockaddr_in* peer=NULL;
	int len1=0;
	int len2=0;

	lpfnGetAddrEx(
		perio_data->dbuf.buf,
		0,
		sizeof(sockaddr_in) + 16,
		sizeof(sockaddr_in) + 16,
		(LPSOCKADDR*)&addr,
		&len1,
		(LPSOCKADDR*)&peer,
		&len2);

	if(addr) memcpy(sk_remote.sk_accept.addr.sk_addr_in(),addr,len1);
	if(peer) memcpy(sk_remote.sk_accept.peer.sk_addr_in(),peer,len2);

	NewSession(sk_remote.sk_accept);

#endif

	WaitForAccept();
}


void SessionServer::NewSession(PerIO_socket& sk)
{
	sk.sock.Shutdown();
}




VHWD_LEAVE
