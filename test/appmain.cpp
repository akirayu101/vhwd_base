#include "vhwd.h"

//using namespace vhwd;

#ifdef _MSC_VER
#pragma comment(lib,"vhwd.lib")
#endif


int main(int argc,char** argv)
{

	vhwd::System::SetLogFile("vhwd.log");

	vhwd::Logger logger;

	vhwd::System::LogTrace("----  process enter   -------");

	vhwd::TestMgr::current().Run(argc,argv);

	vhwd::ThreadManager::current().close();
	vhwd::ThreadManager::current().wait();

	vhwd::System::LogTrace("----  process leave   -------");

	vhwd::Console::Pause();
	return 0;
};

