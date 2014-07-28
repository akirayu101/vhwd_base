#ifndef __H_VHWD_THREAD_POOL__
#define __H_VHWD_THREAD_POOL__

#include "vhwd/collection/intrusive_list.h"
#include "vhwd/threading/thread.h"


VHWD_ENTER



class VHWD_DLLIMPEXP ITask : public ObjectData
{
public:

	virtual void svc(void* pdat)=0;

	// call if task canceled!
	virtual void del(void* pdat) {}
};


class VHWD_DLLIMPEXP ThreadPool : public Thread
{
public:
	typedef Thread basetype;

	ThreadPool();

	void putq(ITask* hjob,void* pdat);


	bool activate();

	void reqexit();

	void set_worker_min(int n);
	void set_worker_max(int n);

	int get_worker_min();
	int get_worker_max();
	int get_worker_num();

protected:

	class TaskItem : public NonCopyable
	{
	public:
		TaskItem():next(NULL) {}
		TaskItem(ITask* hjob_,void* pdat_):hjob(hjob_),pdat(pdat_),next(NULL) {}

		DataPtrT<ITask> hjob;
		void* pdat;
		TaskItem* next;
	};

	Mutex m_tMutex;
	Condition m_tCond;
	intrusive_list<TaskItem> m_tTaskQueue;

	int m_nWorkerWait;
	int m_nWorkerNum;
	int m_nWorkerMin;
	int m_nWorkerMax;
	int m_nTaskHint;

	TaskItem* getq();

	void svc();


};


VHWD_LEAVE
#endif
