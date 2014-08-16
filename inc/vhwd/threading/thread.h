// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_THREAD__
#define __H_VHWD_THREAD__

#include "vhwd/basic/bitflags.h"
#include "vhwd/threading/thread_mutex.h"
#include "vhwd/threading/thread_cond.h"
#include "vhwd/collection/array.h"
#include "vhwd/threading/thread_manager.h"
#include "vhwd/collection/lockfree_queue.h"
#include "vhwd/basic/functor.h"

VHWD_ENTER

class VHWD_DLLIMPEXP ThreadManager;
class VHWD_DLLIMPEXP Logger;
class VHWD_DLLIMPEXP ThreadImpl;

class VHWD_DLLIMPEXP Thread : public Object
{
public:
	friend class ThreadManager;
	friend class ThreadImpl;

	Thread();

	// Copy construct and assign operator, NOTE these methods do NOT really copy any running threads.
	Thread(const Thread& o);
	Thread& operator=(const Thread&)
	{
		return *this;
	}

	virtual ~Thread();

	static Thread& main_thread();
	static Thread& this_thread();
	static ThreadImpl& this_data();

	// Return current thread rank, from 0 to count()-1
	static int rank();

	// Set current thread priority, 0 lowest 100 highest
	static void set_priority(int n);

	// Bind current thread to core n
	static void set_affinity(int n);

	// Start one thread calling entry point
	virtual bool activate();

	// Default entry point
	virtual void svc();

	// Return true if thread exit flag is set
	// Block if thread pause flag is set
	virtual bool test_destroy();

	// Sleep for a number of milliseconds.
	static void sleep_for(int milliseconds);

	// Sleep for a timespan.
	static void sleep_for(const TimeSpan& rel_time);

	// Sleep until timepoint.
	static void sleep_until(const TimePoint& abs_time);

	// A hint to the scheduler to let another thread run.
	static void yield();

	// Wait until thread terminates.
	virtual void wait();

	// Wait until thread terminates or timeout.
	bool wait_for(int milliseconds);

	// Wait until thread terminates or timeout.
	bool wait_for(const TimeSpan& rel_time);

	// Wait until thread terminates or timepoint reached.
	bool wait_until(const TimePoint& abs_time);

	// Set the thread exit flag, causing member function test_destroy() return true;
	virtual void reqexit();

	// Set the thread pasue flag, causing member function test_destroy() block.
	void pause();

	// Unset the thread pause flag.
	void resume();

	// Called while thread throws unhandled exception. can use try{throw;}catch... to rethrow the excetpion.
	virtual void on_exception();

	enum
	{
		STATE_PAUSED	=1<<0,	// thread pause flag
		STATE_CANCEL	=1<<1,	// thread exit flag
		STATE_ANY		=STATE_PAUSED|STATE_CANCEL,
	};

	enum
	{
		FLAG_DYNAMIC	=1<<0, // can start threads when alive()==true
	};

	// Running?
	bool alive();

	// Return running thread count.
	int count();

	// Thread flags, currently only support FLAG_DYNAMIC.
	// If FLAG_DYNAMIC is set, then new threads can be activated while alive.
	BitFlags& flags()
	{
		return m_nFlags;
	}


	void bind_cpu(int c1)
	{
		m_aBindCpu.push_back(c1);
	}

	static uintptr_t id();

protected:

	volatile int m_nState;
	volatile int m_nAlive;

	Mutex m_thrd_mutex;
	Condition m_cond_state_changed;
	Condition m_cond_thrd_empty;

	BitFlags m_nFlags;

	arr_1t<int> m_aBindCpu;

	void _init();

};

class VHWD_DLLIMPEXP ThreadMulti : public Thread
{
public:

	ThreadMulti();


	// Start one thread calling entry point
	virtual bool activate();

	// Start N threads calling entry point
	virtual bool activate(int n);
};

class VHWD_DLLIMPEXP ThreadEx : public Thread
{
public:

	ThreadEx() {}

	typedef Functor<void()> factor_type;

	// A group of thread entry point
	class InvokerGroup
	{
	public:

		arr_1t<factor_type> m_aInvokers;

		size_t size()
		{
			return m_aInvokers.size();
		}
		factor_type& operator[](size_t n)
		{
			return m_aInvokers[n];
		}

		void append(factor_type& fac)
		{
			m_aInvokers.push_back(fac);
		}

		template<typename T>
		void append(T func)
		{
			factor_type fac;
			fac.bind(func);
			m_aInvokers.push_back(fac);
		}

		template<typename T,typename P1>
		void append(T func,P1 p1)
		{
			factor_type fac;
			fac.bind(func,p1);
			m_aInvokers.push_back(fac);
		}
	};

	// Start one thread calling fac().
	bool activate(factor_type& fac)
	{
		InvokerGroup g;
		g.append(fac);
		return activate(g);
	}

	// Start a group of threads.
	bool activate(InvokerGroup& g);

	template<typename T>
	bool activate(T func)
	{
		InvokerGroup g;
		g.append(func);
		return activate(g);
	}

	template<typename T,typename P1>
	bool activate(T func,P1 p1)
	{
		InvokerGroup g;
		g.append(func,p1);
		return activate(g);
	}

};

class VHWD_DLLIMPEXP ThreadCustom : public ThreadEx
{
public:
	typedef ThreadEx basetype;
	ThreadCustom() {}

	// Start a group of threads calling entry points defined in m_aThreads.
	virtual bool activate();

protected:
	InvokerGroup m_aThreads;
};


VHWD_LEAVE

#endif
