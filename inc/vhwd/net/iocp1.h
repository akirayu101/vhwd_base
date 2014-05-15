

#ifndef __H_VHWD_NET_IOCP1__
#define __H_VHWD_NET_IOCP1__

#include "vhwd/basic/platform.h"
#include "vhwd/net/session.h"
#include "vhwd/threading/thread.h"
#include "vhwd/threading/thread_event.h"
#include "vhwd/threading/thread_spin.h"
#include "vhwd/logging/logger.h"
#include "vhwd/basic/lockguard.h"


VHWD_ENTER

typedef arr_1t<DataPtrT<Session> > SessionArray;

class IocpCommand : public ObjectData
{
public:
	virtual void Handle(SessionArray& akey)=0;
};


class VHWD_DLLIMPEXP IOCPPool : public ThreadEx, private NonCopyable
{
public:
	typedef ThreadEx basetype;

	Logger logger;

	bool activate(int nWorker_=1);

	IOCPPool(int maxconn_=1024*16);
	~IOCPPool();

	void reqexit();
	void wait_for_all_session_exit();

	void DisconnectAll();

	KO_Policy_handle::type native_handle(){return hIOCPhandler;}

	int Register(Session* session);
	bool Execute(IocpCommand* cmd);

	const IOCPAccounter& GetAccounter() const {return accounter;}
protected:

	void ccc_update_info();
	void ccc_handle_sock();
	void ccc_execute_cmd();

#ifdef _WIN32
	void ccc_handle_iocp(Session* pkey,MyOverLapped* pdat);
#endif

	AutoPtrT<IOCPAccounter> m_pAccounterLast,m_pAccounterTemp;

	IOCPAccounter accounter;

	SessionArray m_aSessions;
	LockFreeQueue<int> m_lkfqSessionAvailable;
	LockFreeQueue<DataPtrT<IocpCommand>,LockFreeQueuePolicyObj<DataPtrT<IocpCommand> > > m_lkfqCommand;
	int m_nSessionMax;

	KO_Handle<KO_Policy_handle> hIOCPhandler;

	void svc_del(int n);

	void svc_worker();
	void svc_checker();

	Event m_nCanClose;

#ifndef _WIN32
    void HandleSend(Session& ikey);
    void HandleRecv(Session& ikey);
#endif

};


VHWD_LEAVE
#endif
