#include "TestSuitePortal.h"
#include <thread>
#include "TMemBase.h"
#include "Framework.h"
TMemBase _gTemBase;
void TestSuitePortal::Test()
{
	std::thread thread = std::thread(&TestSuitePortal::TestSuiteLogic, this);
	thread.detach();
	
}

void TestSuitePortal::TestSuiteLogic()
{
	while (true)
	{
		_gTemBase.Test();
		Sleep(10000);
	}
}
