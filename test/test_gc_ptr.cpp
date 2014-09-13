
#include "vhwd/testing/test.h"
#include "vhwd/memory/gc_ptr.h"
#include "vhwd/threading.h"
#include "vhwd/basic.h"

using namespace vhwd;

class A : public gc_obj
{
public:

	A(){val=-1;}


	gc_ptr<A> next;
	//LitePtrT<A> next;

	int val;

protected:

	void _gc_mark(gc_state::queue_type& q)
	{
		if(next) q.push_back(next);
	}

};


class ThreadGc : public ThreadMulti
{
public:

	void svc()
	{
		gc_ptr<A> p0;

		size_t m=1024*128;

		for(size_t i=0;i<m;i++)
		{

			gc_ptr<A> p1;
			gc_ptr<A> p2;
			gc_ptr<A> p3;


			p1=gc_new<A>();
			p2=gc_new<A>();
			p3=gc_new<A>();

			// making recursive reference
			p1->next=p2;
			p2->next=p3;
			p3->next=p1;

			// append new element to linked list
			gc_ptr<A> p4=gc_new<A>();
			p4->val=i;

			p4->next=p0;
			p0=p4;
		}

		A* p=p0.get();
		size_t n=0;
		while(p)
		{
			n++;
			wassert(p->val+n==m);
			p=p->next;
		}

		TEST_ASSERT(n==m);

	}
};

TEST_DEFINE(TEST_GarbageCollect)
{

	gc_state::nStepingDelta=1024*128;
	gc_state::flags.add(gc_state::FLAG_SHOW_GC_INFO);

	ThreadGc tgc;
	tgc.activate(3);
	tgc.wait();

	garbage_force_collect();

	TEST_ASSERT(gc_state::nNumberOfObject==0);

}


