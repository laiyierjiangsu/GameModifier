#pragma once

#include <windows.h>
// HookInject 对话框

class HookInject 
{
public:
	// 进程ID
	DWORD m_dwPid;
	std::string m_strDllPath;
	 void OnBnClickedButton2();
	 void StartInject(std::wstring stExe, std::string strDll);
	 void OnBnClickedUnhook();
};
