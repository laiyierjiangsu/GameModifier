#pragma once
#include "stdafx.h"

// ThreadInject 对话框

class ThreadInject 
{
public:
	void StartInject(std::wstring stExe, std::wstring strDll);
	void OnReleaseInject();
private:
	// 进程ID
	DWORD m_dwPid;
	std::wstring m_strDllPath;
	std::wstring m_strExePath;

};
