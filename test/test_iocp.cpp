

#include "vhwd.h"

#define N_TCP_CLIENT 500
#define N_UDP_CLIENT 500

using namespace vhwd;


class SessionUDPEchoClt : public SessionUDP
{
public:

	String msg;
	SessionUDPEchoClt()
	{
		msg="hello from udp client";
	}

	virtual void OnSendComplted(PerIO_buffer* perio_data)
	{
		AsyncRecv(perio_data);
	}

	virtual void OnRecvComplted(PerIO_buffer* perio_data)
	{
		perio_data->dbuf.len=perio_data->size;
		AsyncSend(perio_data);
	}

	void OnConnected()
	{
		PerIO_buffer* buf=GetBuffer(2048,Session::ACTION_UDP_RECV);
		buf->peer=sk_local.peer;
		memcpy(buf->dbuf.buf,msg.c_str(),msg.size()+1);
		buf->dbuf.len=msg.size()+1;
		buf->size=0;
		AsyncSend(buf);
	}
};


class SessionUDPEchoSvr : public SessionUDP
{
public:

	String msg;

	SessionUDPEchoSvr(){}

	virtual void OnSendComplted(PerIO_buffer* perio_data)
	{
		AsyncRecv(perio_data);
	}

	virtual void OnRecvComplted(PerIO_buffer* perio_data)
	{
		perio_data->dbuf.len=perio_data->size;
		AsyncSend(perio_data);
	}

	void OnConnected()
	{
		PerIO_buffer* buf=GetBuffer(2048,Session::ACTION_UDP_RECV);
		AsyncRecv(buf);
	}

	virtual bool TestTimeOut(const TimePoint&){return false;} // server never timeout

};


class SessionClientEcho : public SessionClient
{
public:

	String my_msg;

	void Send(const String& s)
	{
		PerIO_buffer* perio_data=GetBuffer(1024);
		memcpy(perio_data->dbuf.buf,s.c_str(),s.length()+1);
		perio_data->dbuf.len=s.length()+1;
		AsyncSend(perio_data);
	}

	void OnConnected()
	{

	}

	void OnSendReady()
	{
		if(sk_local.sock.Send(my_msg.c_str(),my_msg.length()+1)<=0)
		{
			Disconnect();
			return;
		}
		WaitForRecv();
	}

	void OnRecvReady()
	{
		char buff[1024]={0};
		if(sk_local.sock.Recv(buff,1024)<=0)
		{
			Disconnect();
			return;
		}
		WaitForSend();
	}


	virtual void OnSendComplted(PerIO_buffer* perio_data)
	{
		AsyncRecv(perio_data);
	}

	virtual void OnRecvComplted(PerIO_buffer* perio_data)
	{
		perio_data->dbuf.len=perio_data->size;
		AsyncSend(perio_data);
	}

};


class SessionServerEcho : public SessionServer
{
public:
	SessionServerEcho(){}

	void NewSession(PerIO_socket& sk)
	{
		sk.sock.Block(false);
		SessionEcho* echo=new SessionEcho;
		echo->sk_local.swap(sk);
		StartSession(echo,hiocp);
	}

};

void begin_send(Session* ses)
{
	SessionClientEcho* pEcho=dynamic_cast<SessionClientEcho*>(ses);
	if(pEcho) 
	{
		pEcho->Send("SessionClientEcho message");
	}
}



TEST_DEFINE(TEST_IOCP_TCP)
{
	IOCPPool hiocp_client;
	IOCPPool hiocp_server;

	hiocp_client.bind_cpu(0);
	hiocp_client.bind_cpu(1);
	hiocp_server.bind_cpu(2);
	hiocp_server.bind_cpu(3);

	hiocp_server.SetTimeout(60);
	hiocp_client.activate(1,false);
	hiocp_server.activate(1,true);

	Logger logger;
	logger.LogMessage("testing tcp ----------------");

	DataPtrT<SessionServerEcho> pEchoServer(new SessionServerEcho);
	pEchoServer->SetIOCP(&hiocp_server);
	if(pEchoServer->Listen("127.0.0.1",10241))
	{
		logger.LogMessage("echo server ready");
	}
	else
	{
		logger.LogError("echo server failed");
		return;
	}

	TimePoint tk1=Clock::now();
	int nCount=0;
	for(int i=0;i<N_TCP_CLIENT;i++)
	{
		DataPtrT<SessionClientEcho> pEcho1(new SessionClientEcho);
		pEcho1->SetIOCP(&hiocp_client);

		bool f1=pEcho1->Connect("127.0.0.1",10241);
		if(f1)
		{
			nCount++;
			if(nCount%1000==0)
			{
				Console::WriteLine(String::Format("%d connections",nCount));
			}
		}
	}
	TimePoint tk2=Clock::now();
	logger.LogMessage("%g seconds, %d connections",(tk2-tk1)/TimeSpan::Seconds(1),nCount);

	hiocp_client.callfunc(&begin_send);


	for(int i=0;i<8;i++)
	{
		Thread::sleep_for(1000);
	}

	logger.LogMessage("disconnecting all sessions");

	{
		hiocp_server.DisconnectAll();
		hiocp_client.DisconnectAll();
	}

	logger.LogMessage("wait_session_exit");
    hiocp_server.wait_for_all_session_exit();

	logger.LogMessage("hiocp2 exit");

	hiocp_client.wait_for_all_session_exit();
    logger.LogMessage("wait_session_done");

	
}



TEST_DEFINE(TEST_IOCP_UDP)
{

	IOCPPool hiocp_client;
	IOCPPool hiocp_server;

	hiocp_client.bind_cpu(0);
	hiocp_client.bind_cpu(1);
	hiocp_server.bind_cpu(2);
	hiocp_server.bind_cpu(3);

	hiocp_server.SetTimeout(60);
	hiocp_client.activate(1,false);
	hiocp_server.activate(1,true);

	Logger logger;
	logger.LogMessage("testing udp ----------------");

	DataPtrT<SessionUDPEchoSvr> udp_svr(new SessionUDPEchoSvr);
	udp_svr->SetIOCP(&hiocp_server);
	IPAddress svr_addr("127.0.0.1",10211);
	udp_svr->Bind(svr_addr);

	for(int i=0;i<N_UDP_CLIENT;i++)
	{
		DataPtrT<SessionUDPEchoClt> udp_clt(new SessionUDPEchoClt);
		udp_clt->SetIOCP(&hiocp_client);
		udp_clt->sk_local.peer=svr_addr;
		udp_clt->Bind("127.0.0.1",10312+i);
	}

	for(int i=0;i<8;i++)
	{
		Thread::sleep_for(1000);
	}

	logger.LogMessage("disconnecting all sessions");

	{
		hiocp_server.DisconnectAll();
		hiocp_client.DisconnectAll();
	}

	logger.LogMessage("wait_session_exit");
    hiocp_server.wait_for_all_session_exit();
	hiocp_client.wait_for_all_session_exit();
    logger.LogMessage("wait_session_done");

	
}
