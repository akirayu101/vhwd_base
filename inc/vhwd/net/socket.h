#ifndef __H_VHWD_NET_SOCKET__
#define __H_VHWD_NET_SOCKET__


#include "vhwd/basic/platform.h"

#ifdef _MSC_VER
#pragma comment(lib, "Ws2_32.lib")
#endif



#ifndef VHWD_WINDOWS
#include <sys/socket.h>
#include <netinet/in.h>
#endif


VHWD_ENTER

class VHWD_DLLIMPEXP IPAddress;

class VHWD_DLLIMPEXP KO_Policy_socket
{
public:
#ifdef VHWD_WINDOWS
	typedef SOCKET type;
	static type invalid_value()
	{
		return INVALID_SOCKET;
	}
#else
	typedef int type;
	static type invalid_value()
	{
		return -1;
	}
#endif

	typedef type const_reference;
	static void destroy(type& o);
};

class VHWD_DLLIMPEXP Socket
{
public:

	enum
	{
		DEF	=0,
		TCP	=1,
		UDP	=2,
	};

	class impl_type : public KO_Handle<KO_Policy_socket>
	{
	public:
		impl_type()
		{
			protocol=TCP;
		}
		int protocol;
	};

	Socket();
	~Socket();

	void Ensure(int t=TCP);

	bool Bind(const IPAddress& addr);
	bool Bind(const String& ip,int port);

	// server
	bool Listen();
	bool Listen(const IPAddress& addr);
	bool Listen(const String& ip,int port);

	bool Accept(Socket& sock);
	bool Accept(Socket& sock,IPAddress& addr);

	// client
	bool Connect(const String& ip,int port);
	bool Connect(const IPAddress& addr);


	int Send(const char* buf,int len);
	int Recv(char* buf,int len);

	int Peek(char* buf,int len);

	int SendMsg(const char* buf,int len);
	int RecvMsg(char* buf,int len);

	// udp send/recv
	int Send(const char* buf,int len,const IPAddress& addr);
	int Recv(char* buf,int len,IPAddress& addr);

	bool IsConnected();

	bool GetPeerAddr(IPAddress& addr);
	bool GetSockAddr(IPAddress& addr);

	void Shutdown();
	void Close();

	void Block(bool f);
	void ReuseAddress(bool f);

	operator impl_type::const_reference()
	{
		return impl;
	}

	void Reset(impl_type::const_reference s)
	{
		impl.reset(s);
	}

	void swap(Socket& s)
	{
		impl.swap(s.impl);
	}

	void Reset();

	impl_type::type native_handle()
	{
		return impl;
	}

private:
	impl_type impl;
};

VHWD_LEAVE
#endif
