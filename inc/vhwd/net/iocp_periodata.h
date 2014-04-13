#ifndef __H_VHWD_IOCP_BUFFER__
#define __H_VHWD_IOCP_BUFFER__

#include <deque>

#include "vhwd/net/ipaddress.h"
#include "vhwd/net/socket.h"
#include "vhwd/memory/mempool.h"
#include "vhwd/threading/lockguard.h"
#include "vhwd/collection/array.h"
#include "vhwd/basic/pointer.h"



VHWD_ENTER

#ifndef _WIN32
class WSABUF
{
public:
	char* buf;
	size_t len;
};
#endif

class VHWD_DLLIMPEXP Session;
class VHWD_DLLIMPEXP PerIO_socket
{
public:

	Socket sock;
	IPAddress peer;
	IPAddress addr;

	void swap(PerIO_socket& sk);

};


class VHWD_DLLIMPEXP PerIO_buffer
{
protected:
	PerIO_buffer(const PerIO_buffer&);
	const PerIO_buffer& operator=(const PerIO_buffer&);
	PerIO_buffer(int sz=4096,int t=0);
	~PerIO_buffer();
public:

	enum
	{
		DELETE_BUFFER=1<<0,
	};

	friend class PerIO_manager;

	// allocate buffer with given size
	void reset(int32_t s);

	// associate buffer with given starting address and size
	void reset(char* p,int32_t s);

#ifdef _WIN32
	OVERLAPPED olap;
	DWORD size;
#else
	LitePtrT<Session> pHandler;
	int32_t size;
#endif

	WSABUF dbuf;
	WSABUF hbuf; // allocated buffer

	int32_t type;
	int32_t flag;

	IPAddress peer;

	LitePtrT<PerIO_buffer> pNext;
	//LitePtrT<PerIO_buffer> pPrev;

};

class VHWD_DLLIMPEXP PerIO_manager : public Object
{
public:
	static PerIO_manager& current()
	{
		static PerIO_manager gInstance;
		return gInstance;
	}

	static PerIO_buffer g_send_perio_data;
	static PerIO_buffer g_recv_perio_data;

	~PerIO_manager();

	virtual PerIO_buffer* get(unsigned sz,int t=0);
	virtual void put(PerIO_buffer* pdat);

	class PerIO_Buffer_Link
	{
	public:
		LitePtrT<PerIO_buffer> head;
		AtomicInt32 key;
	};

	void clear();

protected:

	PerIO_manager();
	void _destroy_link(PerIO_Buffer_Link& link);
	arr_1t<PerIO_Buffer_Link> aCache;

};


class VHWD_DLLIMPEXP PerIO_queue
{
public:

	typedef std::deque<PerIO_buffer*> DqueType;
	typedef LockGuard<AtomicInt32> LockType;

	DqueType dq;
	AtomicInt32 key;

	operator AtomicInt32&(){return key;}

	void swap(DqueType& q)
	{
		LockType lock(*this);
		q.swap(dq);
	}
};



VHWD_LEAVE
#endif
