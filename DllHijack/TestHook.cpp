// TestHook.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "src/APCInject.h"
#include "src/DebugInject.h"
#include "src/BlookHook.h"
#include "src/HookInject.h"
int _tmain(int argc, _TCHAR* argv[])
{

	static int iTestType = 0;
	switch (iTestType)
	{
	case 0:
	{
			  APCInject stAPCInject;
			  stAPCInject.StartInject(L"ExeToHook.exe", L"E:\\GithubWorkspace\\GameModifier\\DllHijack\\TestHook\\Debug\\InjectDll.dll");
	}
		break;
	case 1:
	{
			  DebugInject stDebugInject;
			  stDebugInject.StartInject(L"ExeToHook.exe", L"E:\\GithubWorkspace\\GameModifier\\DllHijack\\TestHook\\Debug\\InjectDll.dll");
	}
		break;
	case 2:
	{
			  BlookHook stBlockHook;
			  stBlockHook.OnBlockHook();
	}
		break;
	case 3:
	{
			  HookInject stHookInject;
			  stHookInject.StartInject(L"ExeToHook.exe", "E:\\GithubWorkspace\\GameModifier\\DllHijack\\TestHook\\Debug\\InjectDll.dll");
	}
		break;
	default:
		break;
	}
	printf("wait for exiting");
	getchar();
	return 0;
}

