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
	void reset(){qp=0;ap.clear();}

	bool update(Session::MyOlapPtr& q)
	{
		ap.clear();

		char  *_pbuf=(char*)q->buffer;
		int _left=q->size;

		if(qp!=0)
		{
			if(qp<2)
			{
				memcpy(((char*)&pk)+qp,_pbuf,2);
			}

			if(pk.size>4096)
			{
				System::LogError("Invalid packet_size:%d",(int)pk.size);
				return -1;
			}

			if(pk.size<=qp+_left)
			{
				int _size=pk.size-qp;
				memcpy(((char*)&pk)+qp,_pbuf,_size);
				_left-=_size;
				_pbuf+=_size;
				ap.push_back(&pk);
				qp=0;
			}
			else
			{
				memcpy(((char*)&pk)+qp,_pbuf,_left);
				qp+=_left;
				_left=0;
			}
		}


		for(;_left>=2;)
		{
			IPacketEx* _packet=(IPacketEx*)_pbuf;
			if(_packet->size<=_left)
			{
				ap.push_back(_packet);
				_left-=_packet->size;
				_pbuf+=_packet->size;
			}
			else
			{
				break;
			}
		}

		if(_left>0)
		{
			memcpy(((char*)&pk)+qp,_pbuf,_left);
			qp+=_left;
		}

		return true;
	}

	size_t size(){return ap.size();}
	IPacketEx& operator[](size_t i){return *ap[i];}

private:
	IPacketEx pk;
	int qp;
	arr_1t<IPacketEx*> ap;
};

VHWD_LEAVE
#endif
