
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

	void _gc_mark(gc_mark_queue& q)
	{
		q.append(next);
	}

};

class ThreadGc : public ThreadMulti
{
public:

	void svc()
	{

		this_logger().LogMessage("thread rank %d enter",rank());

		// linked list header
		gc_ptr<A> p0;
		size_t m=1024*128;

		for(size_t i=0;i<m;i++)
		{
			gc_ptr<A> p1=gc_new<A>();
			gc_ptr<A> p2=gc_new<A>();
			gc_ptr<A> p3=gc_new<A>();

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

		this_logger().LogMessage("thread rank %d leave",rank());

	}
};

TEST_DEFINE(TEST_GarbageCollect)
{

	gc_state::current().steping_delta(1024*128);
	gc_state::current().flags.add(gc_state::FLAG_SHOW_GC_INFO);

	ThreadGc tgc;
	tgc.activate(3);
	tgc.wait();

	{

		// gc_ptr in container
		arr_1t<gc_ptr<A> > arr;

		for(size_t i=0;i<1024;i++)
		{
			arr.push_back(gc_new<A>());
			if(i>1)
			{
				arr[i]->next=arr[i-1];
			}
		}
		arr.back()->next=arr[0];

		gc_force_collect();

		TEST_ASSERT(gc_state::current().object_count()==1024);
	}

	gc_force_collect();

	TEST_ASSERT(gc_state::current().object_count()==0);

}


