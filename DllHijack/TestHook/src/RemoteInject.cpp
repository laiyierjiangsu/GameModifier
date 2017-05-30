// RemoteInject.cpp : 实现文件
//

#include "RemoteInject.h"



void RemoteInject::OnBnClickedInject(std::string path)
{
	m_strDllPath = path;
	m_dwPID = GetCurrentProcessId();
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
		TipBox("打开进程失败!!!!");
		return;
	}


	//1.在远程进程中分配内存
	pszRemoteBuffer = (char *)VirtualAllocEx(hProcess, NULL, m_strDllPath.size(), MEM_COMMIT, PAGE_READWRITE);

	if (pszRemoteBuffer == NULL)
	{
		TipBox("申请远程空间失败");
		return;
	}
	//2.在远程申请的地址当中写入DLL的路径
	SIZE_T dwWriten;
	if (!WriteProcessMemory(hProcess, pszRemoteBuffer, (LPVOID)m_strDllPath.c_str(), m_strDllPath.length(), &dwWriten))
	{
		TipBox("写入内存失败");
	}

	//3.获取远程进程中LaodLibry的地址,这里你用的巧合是每个程序中的kernel32的地址的都一样，远程中也一样在
	HMODULE hMouDle = GetModuleHandleA("Kernel32");
	PTHREAD_START_ROUTINE pfnLoadLibrary = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("Kernel32"), "LoadLibraryA");

	if (pfnLoadLibrary == NULL)
	{
		TipBox("获取LoadLibrary地址失败！！！");
		return;
	}

	//4.创建远程线程
	hThread = CreateRemoteThread(hProcess, NULL, 0, pfnLoadLibrary, pszRemoteBuffer, 0, NULL);

	DWORD dwErrCode = GetLastError();
	if (hThread == NULL)
	{
		TipBox("创建远程线程失败");
		return;
	}

	WaitForSingleObject(hThread, 2000);
}
