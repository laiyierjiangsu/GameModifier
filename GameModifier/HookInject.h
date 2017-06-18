#pragma once

#include <windows.h>
// HookInject 对话框

class HookInject 
{
public:
	// 进程ID
	DWORD m_dwPid;
	std::wstring m_strDllPath;
	 void OnBnClickedButton2();
	 void StartInject(std::wstring stExe, std::wstring strDll);
	 void OnBnClickedUnhook();
};
