
#include "vhwd/net/iocp_session.h"
#include "vhwd/logging/logger.h"

VHWD_ENTER

PerIO_buffer PerIO_manager::g_send_perio_data(0,Session::ACTION_WAIT_SEND);
PerIO_buffer PerIO_manager::g_recv_perio_data(0,Session::ACTION_WAIT_RECV);

PerIO_manager::PerIO_manager()
{
	aCache.resize((4096/16)+1);
}

void PerIO_manager::_destroy_link(PerIO_Buffer_Link& link)
{
	LockGuard<AtomicInt32> lock1(link.key);
	//LockGuard<SpinLock> lock1(g_spinPerIOData_list);
	PerIO_buffer* p=link.head;
	while(p)
	{
		PerIO_buffer* t=p;
		p=p->pNext;
		delete t;
	}
	link.head.reset(NULL);
}

void PerIO_manager::clear()
{
	for(size_t i=0;i<aCache.size();i++)
	{
		_destroy_link(aCache[i]);
	}
}

PerIO_manager::~PerIO_manager()
{
	clear();
}

PerIO_buffer* PerIO_manager::get(unsigned sz,int t)
{
	PerIO_buffer* pdat=NULL;
	switch(sz)
	{
	case 16:
	case 128:
	case 256:
	case 512:
	case 1024:
	case 2048:
	case 4096:
		{
			int cc=(sz+15)>>4;
			LockGuard<AtomicInt32> lock1(aCache[cc].key);
			pdat=aCache[cc].head;
			if(pdat)
			{
				aCache[cc].head=pdat->pNext;
				pdat->type=t;
				return pdat;
			}
		}
		break;
	default:
		//pdat=NULL;
		break;
	};

	pdat=new PerIO_buffer(sz,t);
	return pdat;
}

void PerIO_manager::put(PerIO_buffer* pdat)
{
	int sz=pdat->hbuf.len;
	switch(sz)
	{
	case 16:
	case 128:
	case 256:
	case 512:
	case 1024:
	case 2048:
	case 4096:
		{
			int cc=(sz+15)>>4;
			LockGuard<AtomicInt32> lock1(aCache[cc].key);
			pdat->pNext=aCache[cc].head;
			aCache[cc].head=pdat;
			return;
		}
		break;
	default:
		break;
	};

	delete pdat;

}



void PerIO_socket::swap(PerIO_socket& sk)
{
	sock.swap(sk.sock);
	peer.swap(sk.peer);
	addr.swap(sk.addr);
}

PerIO_buffer::PerIO_buffer(int sz,int t)
{
#ifdef _WIN32
	memset(&olap,0,sizeof(olap));
#else 
	pHandler=NULL;
#endif

	dbuf.buf=NULL;
	dbuf.len=0;
	hbuf.buf=NULL;
	hbuf.len=0;

	type=t;
	flag=0;
	reset(sz);
}

void PerIO_buffer::reset(int32_t s)
{
	if((flag&DELETE_BUFFER)!=0)
	{
		MemPoolPaging::current().deallocate(hbuf.buf);
	}

	hbuf.buf=(char*)MemPoolPaging::current().allocate(s);
	hbuf.len=s;
	flag|=DELETE_BUFFER;

	dbuf=hbuf;
}


void PerIO_buffer::reset(char* p,int32_t s)
{
	if((flag&DELETE_BUFFER)!=0)
	{
		MemPoolPaging::current().deallocate(hbuf.buf);
	}

	flag&=~DELETE_BUFFER;
	hbuf.buf=p;
	hbuf.len=s;
	dbuf=hbuf;
}



PerIO_buffer::~PerIO_buffer()
{
	reset(NULL,0);
}


VHWD_LEAVE
