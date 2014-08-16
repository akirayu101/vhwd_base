#include "vhwd.h"


int main(int argc,char** argv)
{
	
	vhwd::System::SetLogFile("vhwd.log");
	vhwd::System::LogTrace("----  process enter   -------");

	vhwd::TestMgr::current().Run(argc,argv);

	//vhwd::ThreadManager::current().close();
	//vhwd::ThreadManager::current().wait();

	vhwd::System::LogTrace("----  process leave   -------");
	vhwd::Console::Pause();

	return 0;
};
