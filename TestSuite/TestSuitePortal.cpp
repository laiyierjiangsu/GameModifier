#include "TestSuitePortal.h"
#include <thread>
#include "TMemBase.h"
#include "Framework.h"
#include "GameGuard/MemProtect.h"
#include "GameGuard/TestObjPool.h"
TMemBase _gTemBase;
MemProtect _gMemProtect;
TestObjPool _gTestObjPool;
void TestSuitePortal::Test()
{
	std::thread thread = std::thread(&TestSuitePortal::TestSuiteLogic, this);
	thread.detach();
	
}

void TestSuitePortal::TestSuiteLogic()
{
	while (true)
	{
		//_gTemBase.Test();
		//_gMemProtect.Test();
		_gTestObjPool.Test();
		//Sleep(10000);
		break;
	}
}
