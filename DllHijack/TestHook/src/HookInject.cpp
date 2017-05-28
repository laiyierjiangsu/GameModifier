// HookInject.cpp : 实现文件
//

#include "stdafx.h"
#include "MyInjectTool.h"
#include "HookInject.h"
#include "afxdialogex.h"


// HookInject 对话框

IMPLEMENT_DYNAMIC(HookInject, CDialogEx)

HookInject::HookInject(CWnd* pParent /*=NULL*/)
	: CDialogEx(HookInject::IDD, pParent)
	, m_dwPid(0)
	, m_strDllPath(_T(""))
{

}

HookInject::~HookInject()
{
}

void HookInject::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_dwPid);
	DDX_Text(pDX, IDC_EDIT4, m_strDllPath);
}


BEGIN_MESSAGE_MAP(HookInject, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON2, &HookInject::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_INJECT, &HookInject::OnBnClickedInject)
	ON_BN_CLICKED(IDC_UNHOOK, &HookInject::OnBnClickedUnhook)
END_MESSAGE_MAP()


// HookInject 消息处理程序


void HookInject::OnBnClickedButton2()
{
	// TODO:  在此添加控件通知处理程序代码
	char szFilter[] = "动态链接库|*.dll";
	CFileDialog fileDlg(TRUE, "dll", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
	UpdateData(TRUE);
	if (fileDlg.DoModal() == IDOK)
	{
		m_strDllPath = fileDlg.GetPathName();
	}
	UpdateData(FALSE);
}



typedef void(*LPFUN)();
typedef void(*LPFUN2)(DWORD dwPid, char * szDllPath);
HINSTANCE g_hDll;
LPFUN2 g_pfnSetHook = NULL;
LPFUN g_pfnUnHook = NULL;

void HookInject::OnBnClickedInject()
{
	// TODO:  在此添加控件通知处理程序代码
	g_hDll = LoadLibrary("InjectDll.dll");

	if (g_hDll != NULL)
	{
		g_pfnSetHook = (LPFUN2)GetProcAddress(g_hDll, "SetHook");
		g_pfnUnHook = (LPFUN)GetProcAddress(g_hDll, "UnHook");
	}
	else
	{
		MessageBox("加载DLL失败！");
		return;
	}


	//安装钩子函数
	if (g_pfnSetHook != NULL)
	{
		g_pfnSetHook(m_dwPid, m_strDllPath.GetBuffer(0));
	}
	else
	{
		MessageBox("安装钩子失败！");
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
