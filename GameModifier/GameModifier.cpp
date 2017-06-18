// TestHook.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "TestSuite/TestMemHack.h"
#include "TestSuite/TestInjectDll.h"
int _tmain(int argc, _TCHAR* argv[])
{

	static int iTestType = 0;
	switch (iTestType)
	{
	case 0:
	{
			  TestInjectDll stInjectDll;
			  stInjectDll.Test();
			  break;
	}
	case 1:
	{
			  //Modify memory
			  TestMemhack stMemHack;
			  stMemHack.Test();
			  break;
	}
	default:
		break;
	}
	printf("wait for exiting!!!!!!");
	getchar();
	return 0;
}

