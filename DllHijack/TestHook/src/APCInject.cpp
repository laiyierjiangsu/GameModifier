// APCInject.cpp : 实现文件
//

#include "APCInject.h"

void APCInject::StartInject(std::wstring stExe, std::wstring strDll)
{
	m_strExePath = stExe;
	m_strDllPath = strDll;

	// TODO:  在此添加控件通知处理程序代码
	DWORD dwRet = 0;
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	ZeroMemory(&pi, sizeof(pi));
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(STARTUPINFO);

	//以挂起的方式创建进程
	dwRet = CreateProcessW(m_strExePath.c_str(),
		NULL,
		NULL,
		NULL,
		FALSE,
		CREATE_SUSPENDED,
		NULL,
		NULL,
		&si,
		&pi);

	if (!dwRet)
	{
		MessageBoxW(NULL,L"CreateProcess失败！！",L"Tip",MB_OK);
		return;
	}

	PVOID lpDllName = VirtualAllocEx(pi.hProcess,
		NULL,
		m_strDllPath.size(),
		MEM_COMMIT,
		PAGE_READWRITE);



	if (lpDllName)
	{
		//将DLL路径写入目标进程空间
		if (WriteProcessMemory(pi.hProcess, lpDllName, m_strDllPath.c_str(), m_strDllPath.size(), NULL))
		{
			LPVOID nLoadLibrary = (LPVOID)GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "LoadLibraryA");
			//向远程APC队列插入LoadLibraryA
			if (!QueueUserAPC((PAPCFUNC)nLoadLibrary, pi.hThread, (DWORD)lpDllName))
			{
				MessageBoxW(NULL, L"QueueUserAPC失败！！", L"Tip", MB_OK);
				return;
			}
		}
		else
		{
			MessageBoxW(NULL, L"WriteProcessMemory失败！！", L"Tip", MB_OK);
			return;
		}
	}

	//恢复主线程
	ResumeThread(pi.hThread);
	MessageBoxW(NULL, L"APC注入成功！！", L"Tip", MB_OK);
}
