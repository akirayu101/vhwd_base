#include "vhwd.h"
#include <typeinfo>
#include <set>
#include <map>

using namespace vhwd;

template<typename G,typename T>
void test_container(G& g,std::vector<T>& p)
{

	Console::WriteLine(typeid(G).name());
	ElapsedTimer timer;

	size_t n=p.size();
	Console::WriteLine(String::Format("size:%d",(int)n));

	timer.tick();
	for(size_t i=0;i<n;i++)
	{
		g.insert(p[i]);
	}
	for(size_t i=0;i<n;i++)
	{
		g.insert(p[i]);
	}
	for(size_t i=0;i<n;i++)
	{
		g.insert(p[i]);
	}
	timer.tack();
	Console::WriteLine(String::Format("t1:%g",timer.get()));

	timer.tick();
	for(size_t i=0;i<n;i++)
	{
		g.erase(p[i]);
	}
	timer.tack();
	Console::WriteLine(String::Format("t2:%g",timer.get()));

}

bst_set<int> get_set()
{
	bst_set<int> rb;
	return rb;
}

int main(int argc,char** argv)
{

	vhwd::System::SetLogFile("vhwd.log");
	vhwd::System::LogTrace("----  process enter   -------");

	std::vector<int> p;

	size_t n=1024*1024;
	for(size_t i=0;i<n;i++)
	{
		p.push_back(i);
	}

	size_t h=0x131;
	for(size_t i=0;i<n;i++)
	{
		h=((h*0x12345+0x54321)>>3)^h;
		std::swap(p[i],p[h%n]);
	}

	//{
	//	indexer_set<int> g;
	//	test_container(g,p);
	//}

	{
		bst_multiset<int,std::less<int>,def_allocator> g;
		test_container(g,p);
	}
	{
		std::multiset<int,std::less<int>,def_allocator> g;
		test_container(g,p);
	}






	//{
	//	std::set<int> g;
	//	test_container(g,p);
	//}



	vhwd::System::LogTrace("----  process leave   -------");
	vhwd::Console::Pause();

	return 0;
};


int main2(int argc,char** argv)
{

	vhwd::System::SetLogFile("vhwd.log");
	vhwd::System::LogTrace("----  process enter   -------");

	vhwd::TestMgr::current().Run(argc,argv);

	vhwd::ThreadManager::current().close();
	vhwd::ThreadManager::current().wait();

	vhwd::System::LogTrace("----  process leave   -------");
	vhwd::Console::Pause();

	return 0;
};
