#include "vhwd/testing/test.h"
#include "vhwd/basic/string.h"
#include "vhwd/threading/thread.h"
#include "vhwd/logging/logger.h"
#include "vhwd/threading/thread.h"


VHWD_ENTER


Test::Test(const String& s):m_sName(s)
{
	TestMgr::current().AddTest(this);
}

void Test::Run()
{

	m_nPassed=0;
	m_nFailed=0;

	TestMgr::current().logger.LogMessage("Running Test: %s",m_sName);

	try
	{
		RunTest();
	}
	catch(...)
	{
		TestMgr::current().logger.LogError("unexpected exception");
		m_nFailed++;
	}
	
	if(m_nFailed>0)
	{
		TestMgr::current().logger.LogError("%d units, %d passed, %d failed",m_nPassed+m_nFailed,m_nPassed,m_nFailed);
	}
	else
	{
		TestMgr::current().logger.LogMessage("%d units, %d passed, %d failed",m_nPassed+m_nFailed,m_nPassed,m_nFailed);
	}

}


VHWD_LEAVE
