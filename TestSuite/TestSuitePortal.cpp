#include "TestSuitePortal.h"
#include <thread>
#include "TMemBase.h"
#include "Framework.h"
#include "GameGuard/MemProtect.h"
TMemBase _gTemBase;
MemProtect _gMemProtect;
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
		_gMemProtect.Test();
		//Sleep(10000);
		break;
	}
}
