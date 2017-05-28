// APCInject.cpp : 实现文件
//

#include "stdafx.h"
#include "MyInjectTool.h"
#include "APCInject.h"
#include "afxdialogex.h"


// APCInject 对话框

IMPLEMENT_DYNAMIC(APCInject, CDialogEx)

APCInject::APCInject(CWnd* pParent /*=NULL*/)
	: CDialogEx(APCInject::IDD, pParent)
	, m_strExePath(_T(""))
	, m_strDllPath(_T(""))
{

}

APCInject::~APCInject()
{
}

void APCInject::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strExePath);
	DDX_Text(pDX, IDC_EDIT2, m_strDllPath);
}


BEGIN_MESSAGE_MAP(APCInject, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON3, &APCInject::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &APCInject::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_INJECT, &APCInject::OnBnClickedInject)
END_MESSAGE_MAP()


// APCInject 消息处理程序


void APCInject::OnBnClickedButton3()
{
	// TODO:  在此添加控件通知处理程序代码
	char szFilter[] = "可执行程序|*.exe";
	CFileDialog fileDlg(TRUE, "exe", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
	UpdateData(TRUE);
	if (fileDlg.DoModal() == IDOK)
	{
		m_strExePath = fileDlg.GetPathName();
	}
	UpdateData(FALSE);
}


void APCInject::OnBnClickedButton4()
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


void APCInject::OnBnClickedInject()
{
	// TODO:  在此添加控件通知处理程序代码
	DWORD dwRet = 0;
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	ZeroMemory(&pi, sizeof(pi));
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(STARTUPINFO);

	//以挂起的方式创建进程
	dwRet = CreateProcess(m_strExePath.GetBuffer(0),
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
		MessageBox("CreateProcess失败！！");
		return;
	}

	PVOID lpDllName = VirtualAllocEx(pi.hProcess,
		NULL,
		m_strDllPath.GetLength(),
		MEM_COMMIT,
		PAGE_READWRITE);



	if (lpDllName)
	{
		//将DLL路径写入目标进程空间
		if (WriteProcessMemory(pi.hProcess, lpDllName, m_strDllPath.GetBuffer(0), m_strDllPath.GetLength(), NULL))
		{
			LPVOID nLoadLibrary = (LPVOID)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");
			//向远程APC队列插入LoadLibraryA
			if (!QueueUserAPC((PAPCFUNC)nLoadLibrary, pi.hThread, (DWORD)lpDllName))
			{
				MessageBox("QueueUserAPC失败！！");
				return;
			}
		}
		else
		{
			MessageBox("WriteProcessMemory失败！！");
			return;
		}
	}

	//恢复主线程
	ResumeThread(pi.hThread);
	MessageBox("APC注入成功");

}
