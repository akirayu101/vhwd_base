#include "vhwd.h"


int main(int argc,char** argv)
{

	vhwd::System::LogFile("vhwd.log");
	vhwd::System::LogTrace("----  process enter   -------");

	vhwd::TestMgr::current().Run(argc,argv);

	vhwd::ThreadPool::current().close();
	vhwd::ThreadPool::current().wait();

	vhwd::System::LogTrace("----  process leave   -------");

	vhwd::Console::Pause();
	return 0;
};

