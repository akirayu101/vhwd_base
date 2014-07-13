
#include "vhwd/testing/test.h"
#include "vhwd/threading.h"
#include "vhwd/basic.h"

using namespace vhwd;

bool hello_run=false;
bool a_my_run=false;

void hello()
{
	hello_run=true;
}

class AAA
{
public:
	void my_run()
	{
		a_my_run=true;
	}
};

class ThreadTest2 : public ThreadMulti
{
public:

	void svc()
	{
		TEST_ASSERT(&this_thread()==this);
		while(!test_destroy())
		{
			Thread::sleep_for(100);
		}
	}
};

bool thread_a_svc=false;

class ThreadA : public Thread
{
public:
	void svc()
	{
		thread_a_svc=true;
	}
};

TEST_DEFINE(TEST_Thread)
{
    Logger logger;
    logger.LogMessage("thread test enter");
	ThreadA thrd1;
	thrd1.activate();
	thrd1.wait();
	TEST_ASSERT(thread_a_svc);

	ThreadTest2 thrd2;
	thrd2.activate(10);
	TEST_ASSERT(thrd2.alive());
	TEST_ASSERT(thrd2.count()==10);
	TEST_ASSERT(!thrd2.activate(3));
	thrd2.flags().add(Thread::FLAG_DYNAMIC);
	TEST_ASSERT(thrd2.activate(3));
	TEST_ASSERT(thrd2.count()==13);

	thrd2.reqexit();
	thrd2.wait();
    logger.LogMessage("thrd2 test leave");

	TEST_ASSERT(!thrd2.alive());
	TEST_ASSERT(thrd2.count()==0);

	AAA aa;
	ThreadEx thrd3;
	thrd3.flags().add(Thread::FLAG_DYNAMIC);
	thrd3.activate(hello);
	thrd3.activate(&AAA::my_run,&aa);
	thrd3.wait();

	TEST_ASSERT(a_my_run);
	TEST_ASSERT(hello_run);


}



class Product : public Object
{
public:
	Product(int v=0):value(v){}
	int value;
};

#ifdef VHWD_USE_COROUTINE

class RoutineProducer : public Coroutine
{
public:

	LitePtrT<Coroutine> Consumer;

	void sink(int val)
	{
		::printf("produce %d, yielding to consumer\n",val);
		m_pExtraParam.reset(new Product(val));
		yield(Consumer);
	}

	int val;

	RoutineProducer()
	{
		val=1;
	}

	void svc()
	{
		TEST_ASSERT(Coroutine::main_coroutine().state()==Coroutine::STATE_PAUSED);
		TEST_ASSERT(&Coroutine::this_coroutine()==this);
		TEST_ASSERT(state()==Coroutine::STATE_RUNNING);

		sink(val++);
		sink(val++);
		sink(val++);
		sink(0);

		// return from entry point, yield to main;
		return;
	}

};



class RoutineConsumer : public Coroutine
{
public:

	void svc()
	{
		for(;;)
		{
			Product* p=dynamic_cast<Product*>(m_pExtraParam.get());
			if(p)
			{
				::printf("consume %d\n",p->value);
			}
			yield_last();
		}
	}

};



TEST_DEFINE(TEST_Coroutine)
{
	RoutineProducer producer;
	RoutineConsumer consumer;

	TEST_ASSERT(producer.state()==Coroutine::STATE_STOPPED);
	TEST_ASSERT(consumer.state()==Coroutine::STATE_STOPPED);
	TEST_ASSERT(Coroutine::main_coroutine().state()==Coroutine::STATE_RUNNING);
	TEST_ASSERT(&Coroutine::main_coroutine()==&Coroutine::this_coroutine());

	producer.Consumer=&consumer;

	Coroutine::spawn(&producer);
	Coroutine::spawn(&consumer);

	TEST_ASSERT(producer.state()==Coroutine::STATE_PAUSED);
	TEST_ASSERT(consumer.state()==Coroutine::STATE_PAUSED);

	Coroutine::yield(&producer);

	TEST_ASSERT(consumer.state()==Coroutine::STATE_PAUSED);
	TEST_ASSERT(producer.state()==Coroutine::STATE_STOPPED);

	TEST_ASSERT(!Coroutine::yield(&producer));

	Coroutine::spawn(&producer);
	TEST_ASSERT(Coroutine::yield(&producer));

}
#endif


class MyTask : public ITask
{
public:

	void svc(void* p)
	{
		Thread::sleep_for(12);
	}
};

TEST_DEFINE(TEST_ThreadPool)
{
	ThreadPool tpool;
	tpool.activate();

	DataPtrT<MyTask> task(new MyTask);
	for(int i=0;i<1000;i++)
	{
		tpool.putq(task.get(),NULL);
		Thread::sleep_for(5);
	}

	Thread::sleep_for(2000);
	tpool.reqexit();
	tpool.wait();

}



class EventThread : public ThreadMulti
{
public:


	Semaphore hSem;
	SpinLock spin;
	AtomicSpin atom;
	Mutex mutex;

	size_t count;
	size_t type;
	size_t result;

	EventThread()
	{

		hSem.post();
		count=4;
	}

	void test(int n,const String& s)
	{
		TimePoint tp1=Clock::now();
		type=n;
		result=0;
		activate(count);
		wait();
		TimePoint tp2=Clock::now();
		Console::WriteLine(String::Format("test:%s:%g",s,(tp2-tp1)/TimeSpan::MilliSeconds(1)));
	}

	void svc()
	{
		if(type==1)
		{
			for(size_t i=0;i<1024*1024/count;i++)
			{
				hSem.wait();
				result++;
				hSem.post();
			}
		}
		else if(type==2)
		{
			for(size_t i=0;i<1024*1024/count;i++)
			{
				spin.lock();
				result++;
				spin.unlock();
			}
		}
		else if(type==3)
		{
			for(size_t i=0;i<1024*1024/count;i++)
			{
				atom.lock();
				result++;
				atom.unlock();
			}
		}
		else if(type==4)
		{
			for(size_t i=0;i<1024*1024/count;i++)
			{
				mutex.lock();
				result++;
				mutex.unlock();
			}
		}

	}
};

TEST_DEFINE(TEST_ThreadOther)
{
	EventThread thrd;

	thrd.test(1,"sem");
	TEST_ASSERT(thrd.result==1024*1024);

	thrd.test(2,"spin");
	TEST_ASSERT(thrd.result==1024*1024);

	thrd.test(3,"atomic");
	TEST_ASSERT(thrd.result==1024*1024);

	thrd.test(4,"mutex");
	TEST_ASSERT(thrd.result==1024*1024);
	
}