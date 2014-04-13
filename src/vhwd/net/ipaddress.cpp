
#include "vhwd/net/ipaddress.h"
#include "vhwd/memory/mempool.h"
#include "vhwd/basic/string.h"

#ifndef _WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>
#endif

VHWD_ENTER

int IPAddress::v4size() const
{
    return nsize;
}

struct sockaddr* IPAddress::sk_addr()
{
	return (sockaddr*)pimpl;
}

struct sockaddr_in* IPAddress::sk_addr_in()
{
	return pimpl;
}

const struct sockaddr* IPAddress::sk_addr() const
{
	return (sockaddr*)pimpl;
}

const struct sockaddr_in* IPAddress::sk_addr_in() const
{
	return pimpl;
}

int* IPAddress::sz_ptr()
{
	return &nsize;
}


IPAddress::IPAddress()
{
	pimpl=new struct sockaddr_in;
	memset(pimpl,0,sizeof(struct sockaddr_in));
	service("127.0.0.1",-1);
}

IPAddress::IPAddress(const IPAddress& o)
{
	pimpl=(struct sockaddr_in*)new struct sockaddr_in;
	memcpy(pimpl,o.pimpl,sizeof(struct sockaddr_in));
	nsize=o.nsize;
}

const IPAddress& IPAddress::operator=(const IPAddress& o)
{
	memcpy(pimpl,o.pimpl,sizeof(struct sockaddr_in));
	nsize=o.nsize;
	return *this;
}

IPAddress::IPAddress(const String& ip,int port)
{
	pimpl=(struct sockaddr_in*)new struct sockaddr_in;
	memset(pimpl,0,sizeof(struct sockaddr_in));
	service(ip,port);
}

IPAddress::~IPAddress()
{
	delete pimpl;
}

void IPAddress::service(const String& ip,int port)
{
	nsize=sizeof(struct sockaddr_in);
	pimpl->sin_family = AF_INET;
	if(ip.size()==0)
	{
        pimpl->sin_addr.s_addr = htons(INADDR_ANY);
	}
	else
	{
		pimpl->sin_addr.s_addr= inet_addr(ip.c_str());
	}
	pimpl->sin_port = htons(port);
}

String IPAddress::host()
{
    char* ipv4_addr=(char*)&pimpl->sin_addr;
	return String::Format("%d.%d.%d.%d",ipv4_addr[0],ipv4_addr[1],ipv4_addr[2],ipv4_addr[3]);
}

int IPAddress::port()
{
	return ntohs(pimpl->sin_port);
}

void IPAddress::swap(IPAddress& addr)
{
	std::swap(pimpl,addr.pimpl);
}


VHWD_LEAVE
