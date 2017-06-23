// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
/*
 extern "C"使得在C++中使用C编译方式成为可能。在“C++”下定义“C”函数，需要加extern “C”关键词。
	 用extern "C"来指明该函数使用C编译方式。输出的“C”函数可以从“C”代码里调用
	使用微软专用的_declspec (dllexport)  
	cpp文件在编译为OBJ文件时要对函数进行重新命名，C语言会把函数name重新命名为_name,而C++会重新命名为_name@@decoration，
	extern "C"表示用C语言的格式将函数重命名
	要输出整个的类，对类使用_declspec(_dllexpot)；要输出类的成员函数，则对该函数使用_declspec(_dllexport)*/

#define EXPORTFUN extern "C" __declspec(dllexport) 

#include <Windows.h>

HWND g_hWinProcess;
HHOOK g_hHook;
char g_szDllPath[MAX_PATH];

EXPORTFUN LRESULT MyMessageProcess(int Code, WPARAM wParam, LPARAM lParam)
{
	//
	//你自己对消息的处理
	//
	return CallNextHookEx(g_hHook, Code, wParam, lParam);
}
EXPORTFUN void TestMessageBox()
{
	//
	//你自己对消息的处理
	MessageBoxW(NULL,L"Test",L"Test",MB_OK); 
}


BOOL APIENTRY DllMain(HANDLE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		//
		//加入你想在目标进程空间HOOK的代码
		//
	//	MessageBoxW(NULL, L"Inject Success!", L"Message", 0);
		OutputDebugString("Injectdd successfule DLL_PROCESS_ATTACH\n");
		break;
	case DLL_THREAD_ATTACH:
		OutputDebugString("Injectdd successfule DLL_THREAD_ATTACH\n");
		break;
	case DLL_THREAD_DETACH:
		OutputDebugString("Injectdd successfule DLL_THREAD_DETACH\n");
		break;
	case DLL_PROCESS_DETACH:
		OutputDebugString("Injectdd successfule DLL_PROCESS_DETACH\n");
		break;
	}
	return TRUE;
}


//钩子回调函数
LRESULT CALLBACK HookProc(
	int code,       // hook code
	WPARAM wParam,  // virtual-key code
	LPARAM lParam   // keystroke-message information
	)
{
	//当DLL注入的成功的时候，我们再加载我们真正要注入的DLL

	char szDllPath[MAX_PATH] = { 0 };
	BOOL bRet = 0;
		
	
	FILE* pFile = fopen("C:\\temp.temp", "r+");
	fread(szDllPath, 1, MAX_PATH, pFile);
	fclose(pFile);
	
	if (!LoadLibrary(szDllPath))
	{
		return 1;
	}
	return CallNextHookEx(NULL, code, wParam, lParam);;
}



EXPORTFUN void SetHook(DWORD  dwPid, char * szDllPath)
{
	THREADENTRY32 te32 = { 0 };
	HANDLE hThreadSnap = NULL;
	DWORD  dwThreadId = 0;
	
	memcpy(g_szDllPath, szDllPath, MAX_PATH);

	//创建线程快照查找目标程序主线程
	te32.dwSize = sizeof(te32);
	hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hThreadSnap == INVALID_HANDLE_VALUE)
	{
		return;
	}

	//遍历查询目标程序主线程ID
	TipBox(L"Thread32First");
	if (Thread32First(hThreadSnap, &te32))
	{
		do
		{
			if (dwPid == te32.th32OwnerProcessID)
			{
				dwThreadId = te32.th32ThreadID;
				break;
			}
		} while (Thread32Next(hThreadSnap, &te32));
	}

	HANDLE hFile = CreateFile("C:\\temp.temp", GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (!hFile)
	{
		TipBox(L"创建临时文件失败！！");
	}
	DWORD dwBytesWritten = 0;
	WriteFile(hFile, szDllPath, strlen(szDllPath) + 1, &dwBytesWritten, NULL);
	CloseHandle(hFile);

	if (dwThreadId != -1)
	{
		//安装键盘钩子
		g_hHook = SetWindowsHookExA(WH_KEYBOARD, HookProc, GetModuleHandle("InjectDll.dll"), dwThreadId);


	}
}

EXPORTFUN void UnHook()
{
	UnhookWindowsHookEx(g_hHook);
}

