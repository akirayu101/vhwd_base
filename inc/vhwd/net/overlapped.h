#ifndef __H_VHWD_NET_OVERLAPPED__
#define __H_VHWD_NET_OVERLAPPED__

#include "vhwd/net/socket.h"
#include "vhwd/net/ipaddress.h"

#include "vhwd/basic/clock.h"
#include "vhwd/basic/bitflags.h"
#include "vhwd/basic/pointer.h"

#include "vhwd/collection/ring_buffer.h"
#include "vhwd/collection/lockfree_queue.h"

#define VHWD_MAX_PACKET_SIZE 4096

VHWD_ENTER

#ifndef _WIN32
class WSABUF
{
public:
    char*buf;
    int32_t len;
};
#endif

class VHWD_DLLIMPEXP PerIO_socket
{
public:

	Socket sock;
	IPAddress peer;
	IPAddress addr;

	void swap(PerIO_socket& sk);
};


class VHWD_DLLIMPEXP MyOverLapped
{
public:

	enum
	{
		ACTION_NOOP,
		ACTION_TCP_SEND,
		ACTION_TCP_RECV,
		ACTION_UDP_SEND,
		ACTION_UDP_RECV,
		ACTION_WAIT_SEND,
		ACTION_WAIT_RECV,
		ACTION_ACCEPT,
	};

	MyOverLapped(int t=ACTION_NOOP);

#ifdef _WIN32
	OVERLAPPED olap;
	DWORD size;
#else
	int32_t size;
#endif

	WSABUF dbuf[2];
	int32_t type;
	IPAddress peer;

};

class VHWD_DLLIMPEXP MyOverLappedRb : public MyOverLapped
{
public:

	AtomicInt32 flag;

	void done();
	void done_send(RingBufferBase& buff_send);
	void done_recv(RingBufferBase& buff_recv);
	bool init_send(RingBufferBase& buff_send);
	bool init_recv(RingBufferBase& buff_recv);

};


class VHWD_DLLIMPEXP MyOverLappedEx : public MyOverLapped, private NonCopyable
{
public:

	MyOverLappedEx();
	~MyOverLappedEx();

	char* buffer;
};





class VHWD_DLLIMPEXP IPacket
{
public:

	unsigned short size;
	unsigned short type;
	int32_t kcrc;
	TimePoint stamp;
	int32_t tags;
	int32_t seqid;
	int32_t arg1;
	int32_t arg2;

	IPacket()
	{
		stamp=Clock::now();
		size=sizeof(IPacket);
		type=0;
	}
};


class VHWD_DLLIMPEXP IPacketEx : public IPacket
{
public:
	char data[VHWD_MAX_PACKET_SIZE-sizeof(IPacket)];
};


template<>
class LockFreeQueuePolicy<TempPtrT<MyOverLappedEx> >
{
public:
	typedef TempPtrT<MyOverLappedEx> T;

	typedef unsigned int size_type;

	static inline size_type timeout(){return 1024*1024*16;}

	static inline T invalid_value(){return T();}

	static inline void move_value(T& a,T& b)
	{
		a.swap(b);
	}

	static inline void fill(T* p,size_type n)
	{
		xmem<T>::uninitialized_fill_n(p,n,T());
	}

	static inline void destroy(T* p,size_type n)
	{
		xmem<T>::destroy(p,n);
	}

	static inline void noop()
	{
		//Thread::yield();
	}

};


class VHWD_DLLIMPEXP ISessionBase
{
public:

	enum
	{
		STATE_READY,
		STATE_INIT,
		STATE_OK,
		STATE_DISCONNECT,
	};

	enum
	{
		FLAG_TIMEOUT=1<<0,
	};


	ISessionBase()
	{
		state=STATE_READY;
		flags.add(FLAG_TIMEOUT);
	}

	PerIO_socket sk_local;


	TimePoint tpLast;
	TimeSpan tsTimeout;

	AtomicInt32 state;
	BitFlags flags;

};

class VHWD_DLLIMPEXP ISession : public ISessionBase
{
public:

	MyOverLappedRb olap_send;
	MyOverLappedRb olap_recv;

	bool pending()
	{
		return olap_send.flag.get()!=0 || olap_recv.flag.get()!=0;
	}

	bool InitSend(RingBufferBase& s);
	bool InitRecv(RingBufferBase& s);

};

class VHWD_DLLIMPEXP IOCPAccounter
{
public:

	TimePoint	tTimeStamp;
	AtomicInt32 nSendCount;
	AtomicInt32 nRecvCount;
	AtomicInt64 nSendBytes;
	AtomicInt64 nRecvBytes;
	AtomicInt32 nSessionCount;
};

VHWD_LEAVE


#endif
