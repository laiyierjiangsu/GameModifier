// ThreadInject.cpp : 实现文件
//

#include "ThreadInject.h"
#include <assert.h>
#include "include/Utils.h"
//777A01D5    68 78563412     push    12345678
//777A01DA    9C              pushfd
//777A01DB    60              pushad

//777A01EB    61              popad
//777A01EC    9D              popfd
//777A01ED    C3              retn
//结构必须字节对齐1
#pragma pack(1)  
typedef struct _INJECT_CODE
{
	BYTE  byPUSH;
	DWORD dwPUSH_VALUE;
	BYTE  byPUSHFD;
	BYTE  byPUSHAD;
	BYTE  byMOV_EAX;          //mov eax, addr szDllpath
	DWORD dwMOV_EAX_VALUE;
	BYTE  byPUSH_EAX;         //push eax
	BYTE  byMOV_ECX;          //mov ecx, LoadLibrary
	DWORD dwMOV_ECX_VALUE;
	WORD  wCALL_ECX;          //call ecx
	BYTE  byPOPAD;
	BYTE  byPOPFD;
	BYTE  byRETN;
	CHAR  szDllPath[MAX_PATH];
}INJECT_CODE, *PINJECT_CODE;
#pragma pack()  


// ThreadInject 对话框




HANDLE g_hProcess = 0;
LPVOID g_lpBuffer = NULL;
void ThreadInject::StartInject(std::wstring stExe, std::wstring strDll)
{

	m_strExePath = stExe;
	TCHAR szPath[MAX_PATH] = { 0 };
	GetCurrentDirectoryW(MAX_PATH, szPath);
	m_strDllPath = szPath + std::wstring(L"\\") + strDll;
	assert(_waccess(m_strDllPath.c_str(), 0) == 0);

	m_dwPid = GetProcessIdByName(WideChar2Utf8((wchar_t*)m_strExePath.c_str()));
	// TODO:  在此添加控件通知处理程序代码
	//打开目标进程
	BOOL   bRet = 0;
	
	INJECT_CODE ic = { 0 };
	THREADENTRY32 te32 = { 0 };
	HANDLE hThreadSnap = NULL;
	DWORD dwThreadId = 0;
	HANDLE hThread = NULL;
	CONTEXT oldContext = { 0 };
	CONTEXT newContext = { 0 };
	DWORD dwOldEip = 0;
	//打开进程
	g_hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_dwPid);
	
	if (!g_hProcess)
	{
		TipBox("OpenProcess 失败");
		return;
	}


	g_lpBuffer = VirtualAllocEx(g_hProcess, NULL, 0x1000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (!g_lpBuffer)
	{
		TipBox("VirtualAllocEx 失败");
		return;
	}
	std::string dllPath = WideChar2Utf8((wchar_t*)m_strDllPath.c_str());
	//给ShellCode结构体赋值
	ic.byPUSH			= 0x68;
	ic.dwPUSH_VALUE		= 0x12345678;
	ic.byPUSHFD			= 0x9C;
	ic.byPUSHAD			= 0x60;
	ic.byMOV_EAX		= 0xB8;
	ic.dwMOV_EAX_VALUE = (DWORD)g_lpBuffer + offsetof(INJECT_CODE, szDllPath);
	ic.byPUSH_EAX		= 0x50;
	ic.byMOV_ECX		= 0xB9;
	ic.dwMOV_ECX_VALUE  = (DWORD)&LoadLibrary;
	ic.wCALL_ECX		= 0xD1FF;
	ic.byPOPAD			= 0x61;
	ic.byPOPFD			= 0x9D;
	ic.byRETN			= 0xC3;
	memcpy(ic.szDllPath, dllPath.c_str(), dllPath.length());

	//写入ShellCode
	bRet = WriteProcessMemory(g_hProcess, g_lpBuffer, &ic, sizeof(ic), NULL);
	if (!bRet)
	{
		TipBox("写入内存失败");
		return;
	}

	//创建线程快照查找目标程序主线程
	te32.dwSize = sizeof(te32);
	hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hThreadSnap == INVALID_HANDLE_VALUE)
	{
		TipBox("CreateToolhelp32Snapshot 失败");
		return;
	}

	//遍历查询目标程序主线程ID
	if (Thread32First(hThreadSnap, &te32))
	{
		do
		{
			if (m_dwPid == te32.th32OwnerProcessID)
			{
				dwThreadId = te32.th32ThreadID;
				break;
			}
		} while (Thread32Next(hThreadSnap, &te32));
	}
	
	//打开目标主线程
	hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, dwThreadId);

	if (!hThread)
	{
		TipBox("OpenThread 失败");
		return;
	}

	//挂起目标主线程
	bRet = SuspendThread(hThread);

	if (bRet == -1)
	{
		TipBox("SuspendThread 失败");
		return;
	}

	oldContext.ContextFlags = CONTEXT_FULL;
	bRet = GetThreadContext(hThread, &oldContext);
	if (!bRet)
	{
		TipBox("GetThreadContext 失败");
		return;
	}
	newContext = oldContext;


#ifdef _WIN64
	newContext.Rip = (DWORD)g_lpBuffer;
	dwOldEip = newContext.Rip;
#else 
	newContext.Eip = (DWORD)g_lpBuffer;
	dwOldEip = newContext.Eip;
#endif

	//;将指针指向ShellCode第一句push 12345678h中的地址,写入返回地址
	bRet = WriteProcessMemory(g_hProcess, ((char*)g_lpBuffer) + 1, &dwOldEip, sizeof(DWORD), NULL);
	if (!bRet)
	{
		TipBox("写入内存失败");
		return;
	}

	bRet = SetThreadContext(hThread, &newContext);

	if (!bRet)
	{
		TipBox("SetThreadContext 失败");
		return;
	}

	//然后把主线程跑起来
	bRet = ResumeThread(hThread);
	
	if (bRet == -1)
	{
		TipBox("ResumeThread 失败");
		return;
	}


}


void ThreadInject::OnReleaseInject()
{
	// TODO:  在此添加控件通知处理程序代码
	if (!VirtualFreeEx(g_hProcess, g_lpBuffer, 0, MEM_RELEASE))
	{
		TipBox("VirtualFreeEx 失败");
		return;
	}

	TipBox("释放对方空间成功");
}
