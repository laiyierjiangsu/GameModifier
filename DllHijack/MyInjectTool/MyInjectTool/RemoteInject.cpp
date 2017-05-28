// RemoteInject.cpp : 实现文件
//

#include "stdafx.h"
#include "MyInjectTool.h"
#include "RemoteInject.h"
#include "afxdialogex.h"


// RemoteInject 对话框

IMPLEMENT_DYNAMIC(RemoteInject, CDialogEx)

RemoteInject::RemoteInject(CWnd* pParent /*=NULL*/)
	: CDialogEx(RemoteInject::IDD, pParent)
	, m_dwPID(0)
	, m_strDllPath(_T(""))
{

}

RemoteInject::~RemoteInject()
{
}

void RemoteInject::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_dwPID);
	DDX_Text(pDX, IDC_EDIT4, m_strDllPath);
}


BEGIN_MESSAGE_MAP(RemoteInject, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON2, &RemoteInject::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_INJECT, &RemoteInject::OnBnClickedInject)
END_MESSAGE_MAP()


// RemoteInject 消息处理程序


void RemoteInject::OnBnClickedButton2()
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


void RemoteInject::OnBnClickedInject()
{
	// TODO:  在此添加控件通知处理程序代码
	HANDLE hProcess = NULL;
	HANDLE hThread = NULL;
	HANDLE hThread2 = NULL;
	char* pszRemoteBuffer = NULL;
	DWORD * pDwTidRemote = NULL;



	//UpdateData(TRUE);


	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_dwPID);

	if (hProcess == NULL)
	{
		MessageBox("打开进程失败!!!!");
		return;
	}


	//1.在远程进程中分配内存
	pszRemoteBuffer = (char *)VirtualAllocEx(hProcess, NULL, m_strDllPath.GetLength(), MEM_COMMIT, PAGE_READWRITE);

	if (pszRemoteBuffer == NULL)
	{
		MessageBox("申请远程空间失败");
		return;
	}
	//2.在远程申请的地址当中写入DLL的路径
	SIZE_T dwWriten;
	if (!WriteProcessMemory(hProcess, pszRemoteBuffer, (LPVOID)m_strDllPath.GetBuffer(0), m_strDllPath.GetLength(), &dwWriten))
	{
		MessageBox("写入内存失败");
	}

	//3.获取远程进程中LaodLibry的地址,这里你用的巧合是每个程序中的kernel32的地址的都一样，远程中也一样在
	HMODULE hMouDle = GetModuleHandle("Kernel32");
	PTHREAD_START_ROUTINE pfnLoadLibrary = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle("Kernel32"), "LoadLibraryA");

	if (pfnLoadLibrary == NULL)
	{
		MessageBox("获取LoadLibrary地址失败！！！");
		return;
	}

	//4.创建远程线程
	hThread = CreateRemoteThread(hProcess, NULL, 0, pfnLoadLibrary, pszRemoteBuffer, 0, NULL);

	DWORD dwErrCode = GetLastError();
	if (hThread == NULL)
	{
		MessageBox("创建远程线程失败");
		return;
	}

	WaitForSingleObject(hThread, 2000);
}
