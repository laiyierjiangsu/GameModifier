// TestHook.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "src/APCInject.h"
#include "src/DebugInject.h"
int _tmain(int argc, _TCHAR* argv[])
{
	//APCInject stAPCInject;
	//stAPCInject.StartInject(L"ExeToHook.exe",L"E:\\GithubWorkspace\\GameModifier\\DllHijack\\TestHook\\Debug\\InjectDll.dll");
	
	DebugInject stDebugInject;
	stDebugInject.StartInject(L"ExeToHook.exe", L"E:\\GithubWorkspace\\GameModifier\\DllHijack\\TestHook\\Debug\\InjectDll.dll");
	return 0;
}

