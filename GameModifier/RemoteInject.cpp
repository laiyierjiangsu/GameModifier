// RemoteInject.cpp : 实现文件
//

#include "RemoteInject.h"
#include <tlhelp32.h>  
#include <assert.h>
#include <stdio.h>
#include <io.h>
#include "include/Utils.h"
void RemoteInject::StartInject(std::string stExe, std::string strDll)
{
	char szPath[MAX_PATH] = { 0 };
	GetCurrentDirectoryA(MAX_PATH, szPath);
	m_strDllPath = szPath + std::string("\\") + strDll;
	assert(_access(m_strDllPath.c_str(), 0) == 0);

	m_dwPID = GetProcessIdByName(stExe);
	// TODO:  在此添加控件通知处理程序代码
	HANDLE hProcess = NULL;
	HANDLE hThread = NULL;
	HANDLE hThread2 = NULL;
	DWORD * pDwTidRemote = NULL;



	//UpdateData(TRUE);


	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_dwPID);

	if (hProcess == NULL)
	{
		TipBox("打开进程失败!!!!");
		return;
	}


	//1.在远程进程中分配内存
	BYTE* 	pszRemoteBuffer = NULL;
	int  bufLen = m_strDllPath.size();
	pszRemoteBuffer = (BYTE *)VirtualAllocEx(hProcess, NULL, bufLen, MEM_COMMIT, PAGE_READWRITE);

	if (pszRemoteBuffer == NULL)
	{
		TipBox("申请远程空间失败");
		return;
	}
	//2.在远程申请的地址当中写入DLL的路径
	SIZE_T dwWriten;
	if (!WriteProcessMemory(hProcess, pszRemoteBuffer, (LPVOID)m_strDllPath.c_str(), bufLen, &dwWriten))
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

DWORD RemoteInject::GetProcessIdByName(std::string name)
{
	// 定义进程信息结构  
	PROCESSENTRY32 pe32 = { sizeof(pe32) };

	// 创建系统当前进程快照  
	HANDLE hProcessShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessShot == INVALID_HANDLE_VALUE)
		return FALSE;

	std::wstring strWname = CharToWchar((char*)name.c_str());
	if (Process32First(hProcessShot, &pe32))
	{
		do
		{
			if (wcscmp(pe32.szExeFile, strWname.c_str()) == 0)
			{
				return pe32.th32ProcessID;
			}

		} while (Process32Next(hProcessShot, &pe32));
	}
	return 0;
}
