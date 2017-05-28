// BlookHook.cpp : 实现文件
//

#include "stdafx.h"
#include "MyInjectTool.h"
#include "BlookHook.h"
#include "afxdialogex.h"


// BlookHook 对话框

IMPLEMENT_DYNAMIC(BlookHook, CDialogEx)

BlookHook::BlookHook(CWnd* pParent /*=NULL*/)
	: CDialogEx(BlookHook::IDD, pParent)
	, m_dwPID(0)
{

}

BlookHook::~BlookHook()
{
}

void BlookHook::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_dwPID);
}


BEGIN_MESSAGE_MAP(BlookHook, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &BlookHook::OnBnClickedButton1)
END_MESSAGE_MAP()


// BlookHook 消息处理程序
typedef void(*LPFUN)();
typedef void(*LPFUN2)(DWORD dwPid);
HINSTANCE g_hBlookDll;
LPFUN2 g_pfnBlookSetHook = NULL;
LPFUN g_pfnBlookUnHook = NULL;

void BlookHook::OnBnClickedButton1()
{
	// TODO:  在此添加控件通知处理程序代码
	g_hBlookDll = LoadLibrary("BlockHook.dll");

	if (g_hBlookDll != NULL)
	{
		g_pfnBlookSetHook = (LPFUN2)GetProcAddress(g_hBlookDll, "SetHook");
		g_pfnBlookUnHook = (LPFUN)GetProcAddress(g_hBlookDll, "UnHook");
	}
	else
	{
		MessageBox("加载DLL失败！");
		return;
	}


	//安装钩子函数
	if (g_pfnBlookSetHook != NULL)
	{
		g_pfnBlookSetHook(m_dwPID);
	}
	else
	{
		MessageBox("安装钩子失败！");
		return;
	}

}
