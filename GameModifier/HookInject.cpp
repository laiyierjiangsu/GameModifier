// HookInject.cpp : 实现文件
//

#include "stdafx.h"
#include "HookInject.h"


typedef void(*LPFUN)();
typedef void(*LPFUN2)(DWORD dwPid, char * szDllPath);
HINSTANCE g_hDll;
LPFUN2 g_pfnSetHook = NULL;
LPFUN g_pfnUnHook = NULL;

void HookInject::StartInject(std::wstring stExe, std::string strDll)
{
	m_strDllPath = strDll;
	m_dwPid = GetCurrentProcessId();
	// TODO:  在此添加控件通知处理程序代码
	g_hDll = LoadLibrary(L"InjectDll.dll");

	if (g_hDll != NULL)
	{
		g_pfnSetHook = (LPFUN2)GetProcAddress(g_hDll, "SetHook");
		g_pfnUnHook = (LPFUN)GetProcAddress(g_hDll, "UnHook");
	}
	else
	{
		TipBox(L"加载DLL失败！");
		return;
	}


	//安装钩子函数
	if (g_pfnSetHook != NULL)
	{
		g_pfnSetHook(m_dwPid, (char *)m_strDllPath.c_str());
	}
	else
	{
		TipBox(L"安装钩子失败！");
		return;
	}
}


void HookInject::OnBnClickedUnhook()
{
	// TODO:  在此添加控件通知处理程序代码
	if (g_hDll != NULL)
	{
		//卸载钩子函数
		g_pfnUnHook();
		//抹掉DLL
		FreeLibrary(g_hDll);
	}
}
