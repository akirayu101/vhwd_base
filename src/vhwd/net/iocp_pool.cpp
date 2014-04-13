
#include "vhwd/net/iocp_session.h"
#include "vhwd/net/iocp_pool.h"
#include "vhwd/basic/system.h"

#ifndef _WIN32
#include <sys/epoll.h>
#include <cerrno>
#endif

VHWD_ENTER

IOCPPool::IOCPPool()
{
	flags().add(FLAG_DYNAMIC);
	m_tOut=TimeSpan::Seconds(10);

	m_nCanClose.reset();

#ifdef _WIN32
	HANDLE h1 = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if(h1!=NULL)
	{
		hIOCPhandler.reset(h1);
	}
	else
	{
		System::LogTrace("CreateIoCompletionPort failed");
	}
#else
	int h1=epoll_create(1024);
	if(h1!=-1)
	{
		hIOCPhandler.reset(h1);
	}
	else
	{
		System::LogTrace("epoll_create failed");
	}
#endif
}

bool IOCPPool::activate(int n,bool checktimeout)
{
	m_tNow=Clock::now();

	ThreadEx::InvokerGroup g;
	for(int i=0;i<n;i++)
	{
		g.append(&IOCPPool::svc_worker,this);
	}
	if(checktimeout)
	{
		g.append(&IOCPPool::svc_checker,this);
	}
	return basetype::activate(g);
}


void IOCPPool::svc_checker()
{
	m_tNow=Clock::now();
	TimePoint tlast=Clock::now();
	TimeSpan tspan=TimeSpan::MilliSeconds(2000);

	int32_t _nLastSendCount=m_nPacketSendCount.get();
	int32_t _nLastRecvCount=m_nPacketRecvCount.get();

	while(!test_destroy())
	{
		tlast+=tspan;
		if(tlast<Clock::now())
		{
			System::LogTrace("tlast<Clock::now in svc_checker");
			tlast=Clock::now()+tspan;
		}

		_nLastSendCount=m_nPacketSendCount.get();
		_nLastRecvCount=m_nPacketRecvCount.get();

		TimePoint t1=Clock::now();
		Thread::sleep_until(tlast);
		CheckTimeout();

		m_tNow=Clock::now();
		uint32_t _sc=(uint32_t)(m_nPacketSendCount.get()-_nLastSendCount);
		uint32_t _rc=(uint32_t)(m_nPacketRecvCount.get()-_nLastRecvCount);
		double _ts=(m_tNow-t1)/TimeSpan::Seconds(1);
		double _pps=double(_sc+_rc)/_ts;

		if(_pps>5.0e3)
		{
			logger.LogMessage("Session_count: %d,packet_per_second: %gW",m_nSessionCount.get(),_pps/1.0e4);
		}
		else
		{
			logger.LogMessage("Session_count: %d,packet_per_second: %g",m_nSessionCount.get(),_pps);
		}

	}
}

IOCPPool::~IOCPPool()
{
	reqexit();
	wait();
}

void IOCPPool::reqexit()
{
	DisconnectAll();

	LockGuard<Mutex> lock1(m_thrd_mutex);
	m_nState=STATE_CANCEL;
	m_cond_state_changed.notify_all();
}

void IOCPPool::DisconnectAll()
{
	LockGuard<SpinLock> lock1(m_tSessionSpin);
	for(Session* ses=m_pSessionList;ses;ses=ses->pNext)
	{
		ses->Disconnect();
	}
}

void IOCPPool::CheckTimeout()
{
	int nTimeout=0;

	LockGuard<SpinLock> lock1(m_tSessionSpin);

	TimePoint _tOut=m_tNow-m_tOut;

	for(Session* ses=m_pSessionList;ses;ses=ses->pNext)
	{
		if(ses->TestTimeOut(_tOut))
		{
			ses->Disconnect();
			nTimeout++;
		}
	}

	if(nTimeout>0) logger.LogMessage("%d sessions timeout",nTimeout);
}

void IOCPPool::wait_for_all_session_exit()
{
	m_nCanClose.wait();
	reqexit();
	wait();
}

void IOCPPool::svc_del(Session* pHandler)
{
#ifndef _WIN32

	{
		PerIO_queue::LockType lock1(pHandler->m_qSend);

		for(size_t i=0;i<pHandler->m_qSend.dq.size();i++)
		{
			PerIO_buffer* q=pHandler->m_qSend.dq[i];
			switch(q->type)
			{
			case Session::ACTION_WAIT_SEND:
			case Session::ACTION_WAIT_RECV:
				break;
			default:
				pHandler->PutBuffer(q);
			}
		}
		pHandler->m_qSend.dq.clear();
	}
	{

		PerIO_queue::LockType lock2(pHandler->m_qRecv);

		for(size_t i=0;i<pHandler->m_qRecv.dq.size();i++)
		{
			PerIO_buffer* q=pHandler->m_qRecv.dq[i];
			switch(q->type)
			{
			case Session::ACTION_WAIT_SEND:
			case Session::ACTION_WAIT_RECV:
				break;
			default:
				pHandler->PutBuffer(q);
			}
		}
		pHandler->m_qRecv.dq.clear();
	}

	pHandler->m_nAsyncPending.store(0);

	struct epoll_event ev;
	epoll_ctl(hIOCPhandler,EPOLL_CTL_DEL,pHandler->sk_local.sock,&ev);
#endif

	{
		LockGuard<SpinLock> lock2(m_tSessionSpin);
		if(pHandler->m_nAsyncPending.load()>0)
		{
			System::LogTrace("Pending>0 in svc_del");
			return;
		}

		if(pHandler->pPrev)
		{
			pHandler->pPrev->pNext=pHandler->pNext;
		}
		else
		{
			m_pSessionList=pHandler->pNext;
		}

		if(pHandler->pNext)
		{
			pHandler->pNext->pPrev=pHandler->pPrev;
		}


	}

	pHandler->OnDisconnected();
	pHandler->DecRef();
	if(--m_nSessionCount==0)
	{
		m_nCanClose.set();
	}

}

#ifdef _WIN32

void IOCPPool::svc_add(Session* pHandler)
{
	pHandler->tlast=m_tNow;

	if(m_nSessionCount++==0)
	{
		m_nCanClose.reset();
	}

	pHandler->IncRef();
	CreateIoCompletionPort(*(HANDLE*)&pHandler->sk_local.sock, hIOCPhandler, (ULONG_PTR)pHandler, 0);

	pHandler->m_nBitFlags.store(Session::FLAG_CONNECTED);
	pHandler->m_nAsyncPending.store(0);
	pHandler->m_nAsyncPendingSend.store(0);
	pHandler->m_nAsyncPendingRecv.store(0);
	pHandler->OnConnected();

	LockGuard<SpinLock> lock1(m_tSessionSpin);
	pHandler->pNext=m_pSessionList;
	pHandler->pPrev=NULL;
	if(m_pSessionList)
	{
		m_pSessionList->pPrev=pHandler;
	}
	m_pSessionList=pHandler;
}

void IOCPPool::svc_worker()
{

	m_tNow=Clock::now();

	DWORD BytesTransferred;

	Session* pHandler=NULL;
	PerIO_buffer* perio_data;
	BOOL bRet;

	while(!test_destroy())
	{
		bRet = GetQueuedCompletionStatus(hIOCPhandler, &BytesTransferred, (PULONG_PTR)&pHandler, (LPOVERLAPPED*)&perio_data,2000);

		if(bRet == 0)
		{
			int r=::WSAGetLastError();

			if(r==WAIT_TIMEOUT)
			{
				m_tNow=Clock::now();
				continue;
			}

			if(perio_data==NULL)
			{
				break;
			}

			if(r!=995 && r!=64)
			{
				System::LogTrace("GetQueuedCompletionStatus Error: %d",r);
			}

			pHandler->Disconnect();
		}

		if(!perio_data)
		{
			if(pHandler->m_nAsyncPending.load()==1)
			{
				pHandler->m_nAsyncPending--;
				svc_del(pHandler);
			}
			else
			{
				pHandler->sk_local.sock.Shutdown();
				CancelIo((HANDLE)pHandler->sk_local.sock.native_handle());
				::PostQueuedCompletionStatus(hIOCPhandler,0,(ULONG_PTR)pHandler,NULL);
			}
			continue;
		}

		perio_data->size=BytesTransferred;
		pHandler->tlast=m_tNow;

		switch(perio_data->type)
		{
		case Session::ACTION_UDP_RECV:
		case Session::ACTION_TCP_RECV:
			pHandler->m_nAsyncPendingRecv--;
			m_nPacketRecvCount++;

			if(perio_data->size<=0)
			{
				pHandler->Disconnect();
				pHandler->PutBuffer(perio_data);
				pHandler->m_nAsyncPending--;
			}
			else if((pHandler->m_nBitFlags.load()&Session::FLAG_REQEXIT)==0)
			{
				pHandler->OnRecvComplted(perio_data);
				pHandler->m_nAsyncPending--;
			}
			else
			{
				pHandler->PutBuffer(perio_data);
				if(--pHandler->m_nAsyncPending==0)
				{
					svc_del(pHandler);
				}
			}
			break;
		case Session::ACTION_TCP_SEND:
		case Session::ACTION_UDP_SEND:
			pHandler->m_nAsyncPendingSend--;
			m_nPacketSendCount++;

			if(perio_data->size<=0)
			{
				pHandler->Disconnect();
				pHandler->PutBuffer(perio_data);
				pHandler->m_nAsyncPending--;
			}
			if((pHandler->m_nBitFlags.load()&Session::FLAG_REQEXIT)==0)
			{
				pHandler->OnSendComplted(perio_data);
				pHandler->m_nAsyncPending--;
			}
			else
			{
				pHandler->PutBuffer(perio_data);
				if(--pHandler->m_nAsyncPending==0)
				{
					svc_del(pHandler);
				}
			}
			break;
		case Session::ACTION_WAIT_RECV:
			m_nPacketRecvCount++;
			pHandler->m_nAsyncPendingRecv--;
			if((pHandler->m_nBitFlags.load()&Session::FLAG_REQEXIT)==0)
			{
				pHandler->OnRecvReady();
				pHandler->m_nAsyncPending--;
			}
			else if(--pHandler->m_nAsyncPending==0)
			{
				svc_del(pHandler);
			}
			break;
		case Session::ACTION_WAIT_SEND:
			m_nPacketSendCount++;
			pHandler->m_nAsyncPendingSend--;
			if((pHandler->m_nBitFlags.load()&Session::FLAG_REQEXIT)==0)
			{
				pHandler->OnSendReady();
				pHandler->m_nAsyncPending--;
			}
			else if(--pHandler->m_nAsyncPending==0)
			{
				svc_del(pHandler);
			}
			break;
		default:
			System::LogTrace("Invalid perio_data_type %d",perio_data->type);
			pHandler->Disconnect();
			break;
		}
	}
}


#else


void IOCPPool::svc_worker()
{
	m_tNow=Clock::now();

	struct epoll_event evts[20];
	if(!hIOCPhandler.ok()) return;

	while (!test_destroy())
	{
		int nfds=epoll_wait(hIOCPhandler,evts,1,1000);

		if(nfds<0)
		{
			m_tNow=Clock::now();
			logger.LogError("ERROR:%s",strerror(errno));
			continue;
		}

		for(int i=0;i<nfds;i++)
		{
            int nevt=evts[i].events;
			Session* pHandler=(Session*)evts[i].data.ptr;

			if(!pHandler)
			{
				logger.LogError("ERROR: NO PSB");
				continue;
			}

			if((nevt&(EPOLLERR|EPOLLHUP))!=0)
			{
				svc_del(pHandler);
				continue;
			}

			PerIO_queue::DqueType dq;
			pHandler->tlast=m_tNow;

			if(nevt&EPOLLOUT)
			{
				m_nPacketSendCount++;
				pHandler->m_qSend.swap(dq);

				size_t i=0;
				for(;i<dq.size();i++)
				{
					PerIO_buffer* perio_data=dq[i];
					if(perio_data->type==Session::ACTION_TCP_SEND)
					{
						int ret=pHandler->sk_local.sock.Send(perio_data->dbuf.buf+perio_data->size,perio_data->dbuf.len-perio_data->size);

						if(ret<0)
						{
							pHandler->Disconnect();
							break;
						}

						perio_data->size+=ret;
						if(perio_data->size==(int)perio_data->dbuf.len)
						{
                            pHandler->OnSendComplted(perio_data);
						}
						else
						{
							break;
						}
					}
					else if(perio_data->type==Session::ACTION_UDP_SEND)
					{
						int ret=pHandler->sk_local.sock.Send(perio_data->dbuf.buf+perio_data->size,perio_data->dbuf.len-perio_data->size,perio_data->peer);

						if(ret<0)
						{
							pHandler->Disconnect();
							break;
						}

						perio_data->size+=ret;
						if(perio_data->size==(int)perio_data->dbuf.len)
						{
                            perio_data->size=perio_data->dbuf.len;
                            pHandler->OnSendComplted(perio_data);
						}
						else
						{
							break;
						}

					}
					else if(perio_data->type==Session::ACTION_WAIT_SEND)
					{
						pHandler->OnSendReady();
					}
					pHandler->m_nAsyncPendingSend--;
					pHandler->m_nAsyncPending--;
				}

				if(i!=dq.size())
				{
					PerIO_queue::LockType lock1(pHandler->m_qSend);
//                    for(size_t j=dq.size()-1;j>=i;j++)
//					{
//						pHandler->m_qSend.dq.push_front(dq[i]);
//					}
					pHandler->m_qSend.dq.insert(pHandler->m_qSend.dq.begin(),dq.begin()+i,dq.end());
				}

			}
			else if(nevt&EPOLLIN)
			{
				m_nPacketRecvCount++;

				pHandler->m_qRecv.dq.swap(dq);

				size_t i=0;
				for(;i<dq.size();i++)
				{
					PerIO_buffer* perio_data=dq[i];
					if(perio_data->type==Session::ACTION_TCP_RECV)
					{
						int ret=pHandler->sk_local.sock.Recv(perio_data->dbuf.buf,perio_data->dbuf.len);

						if(ret>0)
						{
                            perio_data->size=ret;
                            pHandler->OnRecvComplted(perio_data);
						}
						else if(ret==0)
						{
							break;
						}
						else
						{
							pHandler->Disconnect();
							break;
						}

					}

					else if(perio_data->type==Session::ACTION_UDP_RECV)
					{
						int ret=pHandler->sk_local.sock.Recv(perio_data->dbuf.buf,perio_data->dbuf.len,perio_data->peer);

						if(ret>0)
						{
                            perio_data->size=ret;
                            pHandler->OnRecvComplted(perio_data);
						}
						else if(ret==0)
						{
							break;
						}
						else
						{
							pHandler->Disconnect();
							break;
						}

					}
					else if(perio_data->type==Session::ACTION_WAIT_RECV)
					{
						pHandler->OnRecvReady();
					}

					pHandler->m_nAsyncPendingRecv--;
					pHandler->m_nAsyncPending--;
				}

				if(i!=dq.size())
				{
					PerIO_queue::LockType lock1(pHandler->m_qRecv);
					//for(size_t j=dq.size()-1;j>=i;j++)
					//{
					//	pHandler->m_qSend.dq.push_front(dq[i]);
					//}
					pHandler->m_qRecv.dq.insert(pHandler->m_qRecv.dq.begin(),dq.begin()+i,dq.end());

				}
			}
			else
			{
				continue;
			}

			if((pHandler->m_nBitFlags.load()&Session::FLAG_REQEXIT)!=0)
			{
				svc_del(pHandler);
				continue;
			}

			pHandler->ep_ctl(0);

		}
	}
}

void IOCPPool::svc_add(Session* pHandler)
{
	pHandler->tlast=m_tNow;
	pHandler->IncRef();

	struct epoll_event ev;
    ev.data.ptr=pHandler;
    ev.events=EPOLLERR|EPOLLHUP|EPOLLONESHOT|EPOLLET;
    int bRet=epoll_ctl(hIOCPhandler,EPOLL_CTL_ADD,pHandler->sk_local.sock,&ev);
	if(bRet==-1)
	{
        logger.LogError("epoll_ctl failed:%s",strerror(errno));
		pHandler->DecRef();
		return;
	}

	if(m_nSessionCount++==0)
	{
		m_nCanClose.reset();
	}

	pHandler->m_nBitFlags.store(Session::FLAG_CONNECTED);
	pHandler->m_nAsyncPending.store(0);
	pHandler->m_nAsyncPendingSend.store(0);
	pHandler->m_nAsyncPendingRecv.store(0);
	pHandler->OnConnected();

	LockGuard<SpinLock> lock1(m_tSessionSpin);
	pHandler->pNext=m_pSessionList;
	pHandler->pPrev=NULL;
	if(m_pSessionList)
	{
		m_pSessionList->pPrev=pHandler;
	}
	m_pSessionList=pHandler;

}

#endif


VHWD_LEAVE
