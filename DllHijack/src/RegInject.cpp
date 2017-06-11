// RegInject.cpp : 实现文件
//
#include "RegInject.h"





void RegInject::OnBnClickedButton2(std::string path)
{
	m_strDllPath = path;
	// TODO:  在此添加控件通知处理程序代码
	BOOL bRet = FALSE;
	HKEY hKey = NULL;
	LONG nReg;
	char szDllPath[MAX_PATH] = { 0 };

	//打开HKEY_LOCAL_MACHINE/Software/Microsoft/WindowsNT/CurrentVersion/Windows
	nReg = RegOpenKeyExA(HKEY_LOCAL_MACHINE,
		"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Windows",
		0,
		KEY_ALL_ACCESS,
		&hKey);

	if (nReg != ERROR_SUCCESS)
	{
		TipBox("打开注册表失败");
		RegCloseKey(hKey);
		return;
	}

	//设置AppInit_DLLs的键值为我们的Dll
	nReg = RegSetValueExA(hKey,
		"AppInit_DLLs",
		0,
		REG_SZ,
		//(byte *)m_strDllPath.GetBuffer(0),
		NULL,
		m_strDllPath.size()
		);
	if (nReg != ERROR_SUCCESS)
	{
		TipBox("设置注册表失败！");
		RegCloseKey(hKey);
		return;
	}

	RegCloseKey(hKey);
	return;
}
