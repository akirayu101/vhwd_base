

#ifndef __H_VHWD_IOCP_BASE__
#define __H_VHWD_IOCP_BASE__


#include "vhwd/threading/thread.h"
#include "vhwd/threading/thread_event.h"
#include "vhwd/threading/thread_spin.h"
#include "vhwd/logging/logger.h"
#include "vhwd/net/iocp_session.h"


VHWD_ENTER


class VHWD_DLLIMPEXP IOCPPool : public ThreadEx, private NonCopyable
{
public:
	typedef ThreadEx basetype;

	Logger logger;

	bool activate(int nWorker_=1,bool checktimeout=false);

	IOCPPool();
	~IOCPPool();

	void reqexit();
	void svc_add(Session*);
	void wait_for_all_session_exit();

	void DisconnectAll();

	KO_Policy_handle::type native_handle(){return hIOCPhandler;}

	void SetTimeout(int n){m_tOut=TimeSpan::Seconds(n);}

	template<typename T>
	void callfunc(T f)
	{
		LockGuard<SpinLock> lock1(m_tSessionSpin);
		for(Session* ses=GetFirstSession();ses;ses=ses->pNext)
		{
			f(ses);
		}
	}

protected:

	KO_Handle<KO_Policy_handle> hIOCPhandler;

	SpinLock m_tSessionSpin;
	Session* GetFirstSession(){return m_pSessionList;}

	void CheckTimeout();

	void svc_del(Session*);
	void svc_worker();
	void svc_checker();

	LitePtrT<Session> m_pSessionList;
	AtomicInt32 m_nSessionCount;

	AtomicInt32 m_nPacketSendCount;
	AtomicInt32 m_nPacketRecvCount;

	Event m_nCanClose;

	TimePoint m_tNow;
	TimeSpan m_tOut;

};


VHWD_LEAVE
#endif
