// TestHook.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "src/APCInject.h"

int _tmain(int argc, _TCHAR* argv[])
{
	APCInject stAPCInject;
	stAPCInject.StartInject(L"ExeToHook.exe",L"InjectDll.dll");
	
	return 0;
}

