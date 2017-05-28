// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"

#define EXPORTFUN extern "C" __declspec(dllexport) 

HWND g_hWinProcess;
HHOOK g_hHook;
char g_szDllPath[MAX_PATH];
BOOL APIENTRY DllMain(HANDLE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		MessageBoxW(NULL, L"Injectdd successfule DLL_PROCESS_ATTACH", L"Tip", MB_OK);
		break;
	case DLL_THREAD_ATTACH:
		MessageBoxW(NULL, L"Injectdd successfule DLL_THREAD_ATTACH", L"Tip", MB_OK);
		break;
	case DLL_THREAD_DETACH:
		MessageBoxW(NULL, L"Injectdd successfule DLL_THREAD_DETACH", L"Tip", MB_OK);
		break;
	case DLL_PROCESS_DETACH:
		MessageBoxW(NULL, L"Injectdd successfule DLL_PROCESS_DETACH", L"Tip", MB_OK);
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
	OutputDebugString("Thread32First");
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
		OutputDebugString("创建临时文件失败！！");
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

