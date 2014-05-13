
#include "vhwd/net/ipaddress.h"
#include "vhwd/memory/mempool.h"
#include "vhwd/basic/string.h"
#include "vhwd/basic/system.h"


#ifndef _WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>
#endif

VHWD_ENTER

void IPAddress::ensure() const
{
	if(pimpl) return;
	pimpl=(struct sockaddr_in*)MemPoolPaging::current().allocate(sizeof(struct sockaddr_in));
	if(!pimpl)
	{
		System::LogTrace("IPAddress::ensure failed!");
		Exception::XBadAlloc();
	}
	else
	{
		memset(pimpl,0,sizeof(struct sockaddr_in));
	}
}

int IPAddress::v4size() const
{
    return nsize;
}

struct sockaddr* IPAddress::sk_addr()
{
	ensure();
	return (sockaddr*)pimpl;
}

struct sockaddr_in* IPAddress::sk_addr_in()
{
	ensure();
	return pimpl;
}

const struct sockaddr* IPAddress::sk_addr() const
{
	ensure();
	return (sockaddr*)pimpl;
}

const struct sockaddr_in* IPAddress::sk_addr_in() const
{
	ensure();
	return pimpl;
}

int* IPAddress::sz_ptr()
{
	return &nsize;
}


IPAddress::IPAddress()
{
	pimpl=NULL;
	nsize=sizeof(struct sockaddr_in);
}

IPAddress::IPAddress(const IPAddress& o)
{
	pimpl=NULL;
	nsize=o.nsize;

	if(o.pimpl)
	{
		ensure();
		memcpy(pimpl,o.pimpl,sizeof(struct sockaddr_in));
	}

}

IPAddress::IPAddress(const String& ip,int port)
{
	pimpl=NULL;
	nsize=sizeof(struct sockaddr_in);

	ensure();

	memset(pimpl,0,sizeof(struct sockaddr_in));
	service(ip,port);
}

const IPAddress& IPAddress::operator=(const IPAddress& o)
{

	if(o.pimpl)
	{
		ensure();
		memcpy(pimpl,o.pimpl,sizeof(struct sockaddr_in));
	}
	else if(pimpl)
	{
		MemPoolPaging::current().deallocate(pimpl);
		pimpl=NULL;
	}

	nsize=o.nsize;
	return *this;
}


IPAddress::~IPAddress()
{
	if(pimpl)
	{
		MemPoolPaging::current().deallocate(pimpl);
	}
}

void IPAddress::service(const String& ip,int port)
{
	ensure();

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
	if(!pimpl) return "";

    char* ipv4_addr=(char*)&pimpl->sin_addr;
	return String::Format("%d.%d.%d.%d",ipv4_addr[0],ipv4_addr[1],ipv4_addr[2],ipv4_addr[3]);
}

int IPAddress::port()
{
	if(!pimpl) return -1;
	return ntohs(pimpl->sin_port);
}

void IPAddress::swap(IPAddress& addr)
{
	std::swap(pimpl,addr.pimpl);
	std::swap(nsize,addr.nsize);
}


VHWD_LEAVE
