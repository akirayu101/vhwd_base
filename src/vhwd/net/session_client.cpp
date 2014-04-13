#include "vhwd/net/iocp_session.h"
#include "vhwd/net/iocp_pool.h"

VHWD_ENTER

SessionClient::SessionClient()
{

}

SessionClient::~SessionClient()
{

}

void SessionClient::OnConnected()
{

}

void SessionClient::OnDisconnected()
{
	basetype::OnDisconnected();
}


bool SessionClient::Connect(const String& ip,int port)
{

	if(IsConnected())
	{
		return false;
	}

	if(!hiocp)
	{
		return false;
	}

	sk_local.peer.service(ip,port);

	if(!sk_local.sock.Connect(sk_local.peer))
	{
		logger.LogError("connect error");
		return false;
	}

	sk_local.sock.GetSockAddr(sk_local.addr);

    hiocp->svc_add(this);
	return true;

}

SessionEcho::SessionEcho()
{

}

SessionEcho::~SessionEcho()
{

}


void SessionEcho::OnSendReady()
{
	int r=sk_local.sock.Send(&mybuf[mypos],mylen-mypos);
	if(r<=0)
	{
		Disconnect();
		return;
	}

	mypos+=r;
	if(mypos<mylen)
	{
		WaitForSend();
		return;
	}

	WaitForRecv();
}

void SessionEcho::OnRecvReady()
{
	mylen=sk_local.sock.Recv(&mybuf[0],1024);

	if(mylen<=0)
	{
		Disconnect();
		return;
	}
	
	mypos=sk_local.sock.Send(&mybuf[0],mylen);
	if(mypos<0)
	{
		Disconnect();
		return;
	}

	if(mypos==mylen)
	{
		WaitForRecv();
	}
	else
	{
		WaitForSend();			
	}

}

void SessionEcho::OnConnected()
{
	mybuf.resize(1024);
	WaitForRecv();
	//AsyncRecv(GetBuffer(1024));
}

void SessionEcho::OnDisconnected()
{
	
}

void SessionEcho::OnSendComplted(PerIO_buffer* perio_data)
{
	AsyncRecv(perio_data);
}

void SessionEcho::OnRecvComplted(PerIO_buffer* perio_data)
{
	perio_data->dbuf.len=perio_data->size;
	AsyncSend(perio_data);
}



VHWD_LEAVE
