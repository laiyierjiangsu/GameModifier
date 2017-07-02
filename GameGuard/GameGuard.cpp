// ExeToHook.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Framework.h"
#include "WindowDetector.h"

#include "TestSuitePortal.h"
#include "DetectorProtal.h"
DetectorPortal _globalDetector;
TestSuitePortal _gTestSuite;
char * a = nullptr;
int _tmain(int argc, _TCHAR* argv[])
{
 
	HMODULE hModule = GetModuleHandle(NULL);
	int b = 0;
	int c  = 0;
//	printf("------b = %p, c = %p\n",&b,&c);
	a = new char[2];
	//printf("----main a---:%p, %p\n",a,&a);
	static int i = 0;
	while (true)
	{
		//Sleep(3000);
		//bool bLive = _globalDetector.IsLive(timeGetTime());
		////printf("detecor is live £¿£¿ %s\n",bLive ?"true":"false" );
		//printf("wait for hacking %d\n", i++);
		_globalDetector.Init();
		//printf("Test suite pointer: %p\n", &_gTestSuite);
		//_gTestSuite.Test();
		break;
	}
	getchar();
	return 0;
}

