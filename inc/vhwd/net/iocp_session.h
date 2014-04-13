#ifndef __H_VHWD_IOCP_SESSION__
#define __H_VHWD_IOCP_SESSION__

#include "vhwd/net/socket.h"
#include "vhwd/threading/thread.h"
#include "vhwd/logging/logger.h"
#include "vhwd/net/iocp_periodata.h"

#ifdef _WIN32
	#include <WinSock2.h>
	#include <ws2tcpip.h>
	#include <mswsock.h>
#else
	#include <sys/epoll.h>
#endif


VHWD_ENTER

class VHWD_DLLIMPEXP IOCPPool;

class VHWD_DLLIMPEXP Session : public ObjectData, private NonCopyable
{
public:
	friend class IOCPPool;

	Session();
	virtual ~Session();

	LitePtrT<Session> pNext;
	LitePtrT<Session> pPrev;

	PerIO_socket sk_local;

	// Call when AsyncSend/AsyncRecv is finished, default action: put perio_data back to PerIO_manager.
	virtual void OnSendComplted(PerIO_buffer* perio_data);
	virtual void OnRecvComplted(PerIO_buffer* perio_data);

	virtual void OnSendReady();
	virtual void OnRecvReady();

	virtual void OnConnected();
	virtual void OnDisconnected();

	bool IsConnected();


	enum
	{
		ACTION_NOOP,
		ACTION_WAIT_SEND,
		ACTION_WAIT_RECV,
		ACTION_TCP_SEND,
		ACTION_TCP_RECV,
		ACTION_UDP_SEND,
		ACTION_UDP_RECV,

		FLAG_CONNECTED	=1<<0,
		FLAG_REQEXIT	=1<<1,
	};

	virtual void SetIOCP(IOCPPool* p);
	virtual void Disconnect();

	PerIO_buffer* GetBuffer(unsigned sz,int t=0);
	void PutBuffer(PerIO_buffer* pdat);

	Logger logger;

protected:

	virtual bool TestTimeOut(const TimePoint& tp);

	TimePoint tlast; // last active timepoint

	LitePtrT<IOCPPool> hiocp;
	LitePtrT<PerIO_manager> pbuf_manager;

	AtomicInt32 m_nAsyncPendingSend;
	AtomicInt32 m_nAsyncPendingRecv;

	AtomicInt32 m_nAsyncPending;
	AtomicInt32 m_nBufferCount;
	AtomicInt32 m_nBitFlags;

#ifndef _WIN32
	PerIO_queue m_qSend;
	PerIO_queue m_qRecv;

	void ep_ctl(int f);
	int m_nLastEpoll_ctl;
#endif
	
};

class VHWD_DLLIMPEXP SessionTCP : public Session
{
public:

	void WaitForSend();
	void WaitForRecv();

	void AsyncSend(PerIO_buffer* perio_data);
	void AsyncRecv(PerIO_buffer* perio_data);

};


class VHWD_DLLIMPEXP SessionUDP : public Session
{
public:

	typedef Session basetype;

	void AsyncSend(PerIO_buffer* perio_data);
	void AsyncRecv(PerIO_buffer* perio_data);

	void OnConnected();
	void OnDisconnected();

	bool Bind(IPAddress& addr);
	bool Bind(const String& ip,int port);
};

class VHWD_DLLIMPEXP SessionClient : public SessionTCP
{
public:
	typedef SessionTCP basetype;

	SessionClient();
	~SessionClient();

	void OnConnected();
	void OnDisconnected();

	bool Connect(const String& ip,int port);

};


class VHWD_DLLIMPEXP SessionServer : public SessionTCP
{
public:
	typedef SessionTCP basetype;

#ifdef _WIN32
	LPFN_ACCEPTEX lpfnAcceptEx;
	LPFN_GETACCEPTEXSOCKADDRS lpfnGetAddrEx;
#endif

	SessionServer();

	bool Listen(const String& ip,int port);
	void Disconnect();
	void Close();

	void WaitForAccept();

	virtual void NewSession(PerIO_socket& sk);

	void OnConnected();
	void OnDisconnected();

protected:

	virtual bool TestTimeOut(const TimePoint&){return false;} // server never timeout

	virtual void OnRecvReady();

	void StartSession(Session*,IOCPPool* h=NULL);

	class PerIO_accept : public PerIO_buffer
	{
	public:
		PerIO_socket sk_accept;
		PerIO_accept():PerIO_buffer(128,Session::ACTION_WAIT_RECV){}
	};

	PerIO_accept sk_remote;
};



class VHWD_DLLIMPEXP SessionEcho : public SessionTCP
{
public:
	typedef SessionTCP basetype;

	SessionEcho();
	~SessionEcho();

	virtual void OnConnected();
	virtual void OnDisconnected();

	virtual void OnSendReady();
	virtual void OnRecvReady();

	virtual void OnSendComplted(PerIO_buffer* perio_data);
	virtual void OnRecvComplted(PerIO_buffer* perio_data);

private:
	arr_1t<char> mybuf;
	int mypos;
	int mylen;
};



VHWD_LEAVE
#endif
