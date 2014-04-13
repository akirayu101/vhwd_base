
#include "vhwd/testing/test.h"
#include "vhwd/basic.h"
#include "vhwd/memory.h"
#include "vhwd/threading.h"


using namespace vhwd;

class ThreadTestMemPool : public ThreadMulti
{
public:

	Logger logger;

	ThreadTestMemPool()
	{
		MemPoolPaging::current();
	}

	void svc()
	{

		MemPoolPaging& pool(MemPoolPaging::current());

		arr_1t<void*> h;

		size_t n=1024*64;
		h.reserve(n*40);

		size_t memmax=256;

		logger.LogMessage("thread rank[%d] allocate memory",rank());

		for(size_t i=0;i<n*4;i++)
		{
			size_t sz=((size_t)::rand())%memmax;
			h.push_back(pool.allocate(sz));
		}

		logger.LogMessage("thread rank[%d] allocate/deallocate memory randomly",rank());

		for(size_t i=0;i<n*40;i++)
		{
			if(rand()%2==0)
			{
				size_t sz=((size_t)::rand()%memmax);
				h.push_back(pool.allocate(sz));
			}
			else if(!h.empty())
			{
				size_t kk=((size_t)::rand()%h.size());
				void *p=h[kk];
				h[kk]=h.back();
				h.pop_back();
				pool.deallocate(p);
			}
		}

		logger.LogMessage("thread rank[%d] deallocate memory",rank());

		for(size_t i=0;i<h.size();i++)
		{
			pool.deallocate(h[i]);
		}
	}
};

TEST_DEFINE(TEST_Mempool)
{

	int n=System::GetCpuCount();
	if(n<2) n=2;

	TestMgr::current().logger.LogMessage("testing MemPool, %d threads allocate/deallocate together",n);

	ThreadTestMemPool ht;
	ht.activate(n);
	ht.wait();

}


