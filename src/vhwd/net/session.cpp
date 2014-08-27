#include "vhwd/net/session.h"
#include "vhwd/basic/system.h"
#include "vhwd/basic/console.h"
#include "vhwd/net/iocp1.h"
#include "vhwd/threading/thread_mutex.h"
#include "vhwd/memory/mempool.h"

#ifndef _WIN32
#include <cerrno>
#endif

VHWD_ENTER

Session::LKFQueue Session::lkfq_free(2048,Session::LKFQueue::QUEUE_NONBLOCK);


Session::Session()
{
#ifndef _WIN32
	m_nLastEpoll_def.clr(EPOLLERR|EPOLLHUP);
	m_nTempEpoll_ctl=0;
#endif

	lkfq_send.reset(1024);
	lkfq_recv.reset(1024);
	lkfq_send.flags().add(LKFQueue::QUEUE_NONBLOCK);
	lkfq_recv.flags().add(LKFQueue::QUEUE_NONBLOCK);

	tsTimeout=TimeSpan::Seconds(60);

}


Session::~Session()
{

}

bool Session::HasPending()
{
#ifdef _WIN32
	if(m_nPendingRecv.get()!=0 || m_nPendingSend.get()!=0)
	{
		return true;
	}
	return false;
#else
	return false;
#endif

}

void Session::OnSendCompleted(MyOlapPtr& q)
{
	lkfq_free.putq(q);
}

void Session::OnRecvCompleted(MyOlapPtr& q)
{
	lkfq_free.putq(q);
}


void Session::OnSendReady()
{

}

void Session::OnRecvReady()
{

}


void Session::OnConnected()
{

}

void Session::OnDisconnected()
{

}


bool Session::IsConnected()
{
	return state.get()==STATE_OK;
}

bool Session::IsError()
{
	return state.get()==STATE_DISCONNECT;
}

bool Session::TestTimeout(TimePoint& tp,MyOlapPtr&)
{
	if(tp-tpLast>tsTimeout)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void Session::Disconnect()
{

	int val=STATE_OK;
	if(!state.compare_exchange(val,STATE_DISCONNECT))
	{
		return;
	}

#ifdef _WIN32
	sk_local.sock.Close();
	m_nPendingRecv.fetch_add(1);
	PostQueuedCompletionStatus(hiocp->native_handle(),0,(ULONG_PTR)this,NULL);
#else
	sk_local.sock.Shutdown();
#endif

	MyOlapPtr q;
	for(;;)
	{
		q=lkfq_recv.getq();
		if(!q) break;
		lkfq_free.putq(q);
		m_nPendingRecv--;
	}

	for(;;)
	{
		q=lkfq_send.getq();
		if(!q) break;
		lkfq_free.putq(q);
		m_nPendingSend--;
	}

}

#ifndef _WIN32

void Session::ep_ctl(int f)
{

	if(state.get()!=STATE_OK)
	{
		return;
	}

	int _def_ctl=EPOLLERR|EPOLLHUP|EPOLLET;//|EPOLLONESHOT;
	int _tmp_ctl=_def_ctl;//|EPOLLONESHOT;//|EPOLLONESHOT;//m_nLastEpoll_def.val();

	if(m_nPendingSend.get()>0)
	{
		_tmp_ctl=_tmp_ctl|EPOLLOUT;
	}

	if(m_nPendingRecv.get()>0)
	{
		_tmp_ctl=_tmp_ctl|EPOLLIN;
	}

	if(_tmp_ctl==_def_ctl)
	{
		return;
	}

	m_nLastEpoll_ctl=_tmp_ctl;

	struct epoll_event ev;
	ev.data.ptr=this;
	ev.events=_tmp_ctl;


	int bRet=epoll_ctl(hiocp->native_handle(),EPOLL_CTL_MOD,sk_local.sock.native_handle(),&ev);
	if(bRet<0)
	{
		this_logger().LogError("epoll_ctl failed: %s",strerror(errno));
		Disconnect();
	}


}
#endif

bool Q2Packet::update(Session::MyOlapPtr& q)
{
	return update(q->buffer,q->size);
}

void Q2Packet::done()
{

	if(q1==0) return;

	ap.clear();
	q2-=q1;
	for(size_t i=0;i<q2;i++)
	{
		pk[i]=pk[q1+i];
	}
	q1=0;		
}

bool Q2Packet::update(const char* p1,size_t n)
{
	if(n==0)
	{
		return true;
	}

	if(n>VHWD_MAX_PACKET_SIZE)
	{
		System::LogTrace("buffer too large");
		return false;
	}

	done();

	const char* p2=p1+n;
	size_t n0=q2-q1;

	if(n0==0)
	{
		pk[0]=p1[0];
		if(p2-p1==1)
		{
			q2=1;
			return true;
		}
		pk[1]=p1[1];
		q2=2;
		p1+=2;
	}
	else if(n0==1)
	{
		pk[1]=p1[0];
		q2=2;
		p1+=1;
	}

	while(1)
	{

		size_t sz_real=*(uint16_t*)(pk+q1);
		size_t sz_left=q2-q1;

		if(sz_real>sz_left)
		{
			if(sz_real>VHWD_MAX_PACKET_SIZE)
			{
				System::LogTrace("invalid packet size");
				return false;
			}

			size_t sz_more=sz_real-sz_left;
			size_t n1=p2-p1;

			if(sz_more>n1)
			{
				memcpy(pk+q2,p1,n1);
				q2+=n1;
				return true;
			}

			memcpy(pk+q2,p1,sz_more);

			p1+=sz_more;
			q2+=sz_more;
		}

		ap.push_back((IPacketEx*)(pk+q1));
		q2=(q2+sizeof(void*)-1)&~(sizeof(void*)-1); // adjust q1,q2;
		q1=q2;

		if(p2-p1>2)
		{
			pk[q2+0]=p1[0];
			pk[q2+1]=p1[1];

			q2+=2;
			p1+=2;

		}
		else
		{
			if(p2-p1==1)
			{
				pk[q2]=p1[0];
				q2=q2+1;
			}
			return true;
		}
	}


	// never goes here
	return true;

}

VHWD_LEAVE
