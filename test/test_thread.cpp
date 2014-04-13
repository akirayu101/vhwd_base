
#include "vhwd/testing/test.h"
#include "vhwd/threading.h"

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
