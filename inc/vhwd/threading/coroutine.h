// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_THREADING_COROUTINE__
#define __H_VHWD_THREADING_COROUTINE__

#include "vhwd/basic/bitflags.h"
#include "vhwd/basic/functor.h"


VHWD_ENTER



class VHWD_DLLIMPEXP Coroutine;
class VHWD_DLLIMPEXP CoroutineEx;
class VHWD_DLLIMPEXP CoroutineMain;
class VHWD_DLLIMPEXP CoroutineContext;


class VHWD_DLLIMPEXP Coroutine : public Object, private NonCopyable
{
public:

	friend class CoroutineContext;

	// status of coroutine.
	// STATE_STOPPED: not spawned, cannot be a yield target, can be a spawn target.
	// STATE_RUNNING: spawned and running, can call yield* to yield to other coroutines.
	// STATE_PAUSED: spawned but not running, can be a yield target.
	enum enum_COROUTINE_STATE
	{
		STATE_STOPPED,
		STATE_RUNNING,
		STATE_PAUSED,
	};

	// default stack size
	static const int DEFAULT_STACK_SIZE=1024*1024;

	// stksize_ will be adjust to pagesize*n
	Coroutine(size_t stksize_=DEFAULT_STACK_SIZE);
	~Coroutine();

	// yield to coroutine that yielded to this
	static bool yield_last(ObjectData* extra_=NULL);

	// yield to coroutine main
	static bool yield_main(ObjectData* extra_=NULL);

	// yield to another coroutine with extra data
	// require: pcort_->state()==STATE_PAUSED
	// ensure: paused this_coroutine and yield to pcort_
	static bool yield(Coroutine* pcort_,ObjectData* extra_=NULL);

	// get extra data that last coroutine yileded with
	ObjectData* extra();

	// spawn a coroutine
	// require: pcort_->state()==STATE_STOPPED
	// ensure: pcort_->state()==STATE_PAUSED
	static bool spawn(Coroutine* pcort_);

	// get state of coroutine.
	// state can be STATE_STOPPED, STATE_RUNNING, STATE_PAUSED.
	int state() const;

	// get stack size of coroutine
	size_t stack_size() const;

	// return active coroutine
	static Coroutine& this_coroutine();

	// return main coroutine
	static CoroutineMain& main_coroutine();


protected:

	static bool _yield_phase1(CoroutineContext& callee,CoroutineContext& caller);
	static void _yield_phase2(CoroutineContext& caller);

	// coroutine entry point, this function will be called after this coroutine is yielded to.
	// you should NOT call this function in your code.
	virtual void svc() {}

	CoroutineContext* m_pContext;


};


// coroutine that use functor as entry point.
class VHWD_DLLIMPEXP CoroutineEx : public Coroutine
{
public:

	CoroutineEx(size_t stksize_=DEFAULT_STACK_SIZE):Coroutine(stksize_) {}

	void reset(Functor<void()> func)
	{
		m_tEntry=func;
	}

	template<typename X0>
	void bind(X0 x0)
	{
		m_tEntry.bind(x0);
	}

	template<typename X0,typename X1>
	void bind(X0 x0,X1 x1)
	{
		m_tEntry.bind(x0,x1);
	}

	template<typename X0,typename X1,typename X2>
	void bind(X0 x0,X1 x1,X2 x2)
	{
		m_tEntry.bind(x0,x1,x2);
	}

	template<typename X0,typename X1,typename X2,typename X3>
	void bind(X0 x0,X1 x1,X2 x2,X3 x3)
	{
		m_tEntry.bind(x0,x1,x2,x3);
	}

protected:

	void svc()
	{
		if(m_tEntry)
		{
			m_tEntry();
		}
	}
	Functor<void()> m_tEntry;
};


// a borned running coroutine stand for current thread.
class VHWD_DLLIMPEXP CoroutineMain : public Coroutine
{
public:

	friend class Coroutine;
	friend class CoroutineContext;
	friend class ThreadImpl;

protected:
	CoroutineMain();
	Coroutine* m_pThisRoutine;	// current coroutine
	Coroutine* m_pLastRoutine;	// last coroutine that yield to current
	Coroutine* m_pVoidRoutine;	// coroutine return from svc

};


VHWD_LEAVE
#endif

