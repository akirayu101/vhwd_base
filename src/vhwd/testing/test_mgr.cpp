#include "vhwd/testing/test.h"
#include "vhwd/basic/string.h"
#include "vhwd/basic/system.h"
#include "vhwd/threading/thread.h"
#include "vhwd/logging/logger.h"
#include "vhwd/threading/thread.h"
#include "vhwd/threading/thread_mutex.h"
#include "vhwd/threading/lockguard.h"
#include "vhwd/logging/logtarget.h"

VHWD_ENTER


void TestMgr::Tested(bool v,const char* msg,const char* file,int line)
{
	static Mutex spin;

	LockGuard<Mutex> lock1(spin);
	if(!m_pCurrentTest)
	{
		return;
	}

	if(v)
	{
		m_pCurrentTest->m_nPassed+=1;
	}
	else
	{
		m_pCurrentTest->m_nFailed+=1;
		logger.LogError("%s at %s:%d",msg,file,line);
	}
}


TestMgr::TestMgr()
{
	m_pCurrentTest=NULL;
}

TestMgr::~TestMgr()
{

}
	
void TestMgr::AddTest(Test* t)
{
	if(t==NULL) return;
	tests.push_back(t);
}


void TestMgr::Run(int argc,char** argv)
{
	(void)&argc;
	(void)&argv;

	System::LogTrace("TestMgr::Run");

	logger.reset(new LogConsole);

	m_nUnitPassed=0;
	m_nUnitFailed=0;
	m_nTestPassed=0;
	m_nTestFailed=0;

	logger.LogMessage("TestMgr: %d Tests",tests.size());
	logger.Printf("---------------------------------------");

	for(arr_1t<Test*>::iterator it=tests.begin();it!=tests.end();++it)
	{
		m_pCurrentTest=(*it);

		(*it)->Run();
		m_nUnitPassed+=(*it)->m_nPassed;
		m_nUnitFailed+=(*it)->m_nFailed;
		if((*it)->m_nFailed==0)
		{
			m_nTestPassed+=1;
		}
		else
		{
			m_nTestFailed+=1;
		}

		m_pCurrentTest=NULL;

		logger.Printf("---------------------------------------");

	}


	if(m_nTestFailed>0)
	{
		logger.LogError("%d Tests, %d Pass, %d Failed",m_nTestPassed+m_nTestFailed,m_nTestPassed,m_nTestFailed);
	}
	else
	{
		logger.LogMessage("%d Tests, %d Pass, %d Failed",m_nTestPassed+m_nTestFailed,m_nTestPassed,m_nTestFailed);
	}

	logger.reset(NULL);

}

VHWD_LEAVE
