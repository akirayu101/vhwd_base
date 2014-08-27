#ifndef __H_VHWD_IOCP_SESSION__
#define __H_VHWD_IOCP_SESSION__


#include "vhwd/net/overlapped.h"
#include "vhwd/threading/thread.h"

#if defined(_WIN32)
#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#else
#include <sys/epoll.h>
#endif


VHWD_ENTER


class VHWD_DLLIMPEXP IOCPPool;

class VHWD_DLLIMPEXP Session : public ObjectData, public ISessionBase
{
public:
	friend class IOCPPool;

	typedef TempPtrT<MyOverLappedEx> MyOlapPtr;

	Session();
	virtual ~Session();

	// Call when AsyncSend/AsyncRecv is finished
	virtual void OnSendCompleted(MyOlapPtr& q);
	virtual void OnRecvCompleted(MyOlapPtr& q);

	virtual void OnSendReady();
	virtual void OnRecvReady();

	virtual void OnConnected();
	virtual void OnDisconnected();

	bool IsConnected();
	bool IsError();

	virtual void Disconnect();

	virtual bool TestTimeout(TimePoint& tp,MyOlapPtr& q);

protected:

	bool HasPending();


	typedef LockFreeQueue<MyOlapPtr> LKFQueue;
	LKFQueue lkfq_send;
	LKFQueue lkfq_recv;

	static LKFQueue lkfq_free;

	AtomicInt32 m_nPendingSend;
	AtomicInt32 m_nPendingRecv;


	LitePtrT<IOCPPool> hiocp;


#ifndef _WIN32
	void ep_ctl(int f);
	int m_nLastEpoll_ctl;
	int m_nTempEpoll_ctl;
	BitFlags m_nLastEpoll_def;
	AtomicInt32 m_nEpCtl;
	MyOlapPtr tmp_send;

#endif

};




class VHWD_DLLIMPEXP SessionTCP : public Session
{
public:

	friend class IOCPPool;

	SessionTCP();

	bool WaitForSend();
	bool WaitForRecv();

	bool AsyncSend(const char* data,size_t size);
	bool AsyncSend(MyOlapPtr& q);

	bool AsyncSend(IPacketEx& packet)
	{
		return AsyncSend((char*)&packet,packet.size);
	}

	bool AsyncRecv(MyOlapPtr& q);
	bool AsyncRecv();

protected:
	bool DoAsyncSend(MyOlapPtr& q);
	bool DoAsyncRecv(MyOlapPtr& q);

	void DoAsyncSend();
	void DoAsyncRecv();


};




class VHWD_DLLIMPEXP SessionUDP : public Session
{
public:

	friend class IOCPPool;

	SessionUDP();
	~SessionUDP();

	typedef Session basetype;


	bool Bind(IPAddress& addr);
	bool Bind(const String& ip,int port);
	bool Connect(const String& ip,int port);


	bool AsyncSend(const char* data,size_t size,IPAddress& addr);
	bool AsyncSend(const char* data,size_t size);
	bool AsyncSend(MyOlapPtr& q);

	bool AsyncRecv(MyOlapPtr& q);
	bool AsyncRecv();

protected:
	bool DoAsyncSend(MyOlapPtr& q);
	bool DoAsyncRecv(MyOlapPtr& q);

	void DoAsyncSend();
	void DoAsyncRecv();

protected:

	void OnConnected();
	void OnDisconnected();


};

class VHWD_DLLIMPEXP SessionClient : public SessionTCP
{
public:
	typedef SessionTCP basetype;

	SessionClient();
	~SessionClient();

	bool Connect(const String& ip,int port);

protected:
	void OnConnected();
	void OnDisconnected();
};


class VHWD_DLLIMPEXP SessionServer : public SessionTCP
{
public:
	typedef SessionTCP basetype;

#ifdef _MSC_VER
	LPFN_ACCEPTEX lpfnAcceptEx;
	LPFN_GETACCEPTEXSOCKADDRS lpfnGetAddrEx;
#endif

	SessionServer();

	bool Listen(const String& ip,int port);
	void Disconnect();
	void Close();

	bool WaitForAccept();

	virtual void NewSession(PerIO_socket& sk);

protected:

	void OnConnected();
	void OnDisconnected();

	virtual void OnRecvReady();

	void StartSession(Session*,IOCPPool* h=NULL);

	char tmp_buffer[1024];

	PerIO_socket sk_remote;
};



class VHWD_DLLIMPEXP SessionEcho : public SessionTCP
{
public:
	typedef SessionTCP basetype;

	SessionEcho();
	~SessionEcho();

private:

	virtual void OnConnected();
	virtual void OnDisconnected();

	virtual void OnSendReady();
	virtual void OnRecvReady();

	virtual void OnSendCompleted(MyOlapPtr& q);
	virtual void OnRecvCompleted(MyOlapPtr& q);

	arr_1t<char> mybuf;
	int mypos;
	int mylen;
};


class Q2Packet
{
public:

	Q2Packet(){q1=q2=0;}

	// process buffer, after update, you can call size() to get the number of cached packets.
	// NOTE: old cached packets will be drop!
	bool update(Session::MyOlapPtr& q);

	// n must less or equal VHWD_MAX_PACKET_SIZE
	bool update(const char* p,size_t n);

	// number of cached packets
	inline size_t size() const
	{
		return ap.size();
	}

	// get cached packet by index 0~size()-1
	inline IPacketEx& operator[](size_t i)
	{
		return *ap[i];
	}

	// drop cached packets but keep partial data
	void done();

	// drop cached packets and partial data
	void reset()
	{
		q1=q2=0;
		ap.clear();
	}

private:

	char pk[VHWD_MAX_PACKET_SIZE*2];
	arr_1t<IPacketEx*> ap;
	size_t q1,q2;

};

VHWD_LEAVE
#endif
