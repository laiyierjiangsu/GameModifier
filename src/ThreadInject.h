#pragma once
#include "..\stdafx.h"

// ThreadInject 对话框

class ThreadInject 
{
public:
	// 进程ID
	DWORD m_dwPid;
	std::string m_strDllPath;
	void OnBnClickedButton2();
	void OnBnClickedButton3(std::string path);
	void OnBnClickedButton4();
};
