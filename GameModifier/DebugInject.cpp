// DebugInject.cpp : 实现文件
//

#include "stdafx.h"
#include "DebugInject.h"

//结构必须字节对齐!  
#pragma pack(1)  
typedef struct _INJECT_CODE
{
	BYTE  byMOV_EAX;          //mov eax, addr szDllpath
	DWORD dwMOV_EAX_VALUE;
	BYTE  byPUSH_EAX;         //push eax
	BYTE  byMOV_ECX;          //mov ecx, LoadLibrary
	DWORD dwMOV_ECX_VALUE;
	WORD  wCALL_ECX;          //call ecx
	BYTE  byINT3;             //int 3
	CHAR  szDllPath[MAX_PATH];
}INJECT_CODE, *PINJECT_CODE;
#pragma pack()
// DebugInject 对话框


void DebugInject::StartInject(std::wstring stExe, std::wstring strDll)
{
	 m_strExePath = stExe;
	m_strDllPath = strDll;

	// TODO:  在此添加控件通知处理程序代码
	BOOL bRet;
	DWORD dwProcessId = 0;
	LPVOID lpBaseAddress = NULL;
	HANDLE hThread = NULL;
	HANDLE hProcess = NULL;
	DEBUG_EVENT dbgEvent = { 0 };
	CONTEXT ctxOld = { CONTEXT_FULL };
	CONTEXT ctxNew = { CONTEXT_FULL };
	INJECT_CODE ic = { 0 };
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi = { 0 };
	HMODULE hDll = NULL;
	BOOL bIsSystemBp = TRUE;
	DWORD dwOldEip = 0;
	bRet = CreateProcess(NULL,
		(LPWSTR)m_strExePath.c_str(),
		NULL,
		NULL,
		FALSE,
		DEBUG_ONLY_THIS_PROCESS,
		NULL,
		NULL,
		&si,
		&pi);
	if (!bRet)
	{
		TipBox(L"CreateProcess 失败");
		return;
	}

	//防止被调试进程和调试器一起关闭
	bRet = DebugSetProcessKillOnExit(FALSE);

	while (WaitForDebugEvent(&dbgEvent, INFINITE))
	{
		switch (dbgEvent.dwDebugEventCode)
		{
		case CREATE_PROCESS_DEBUG_EVENT:
			hProcess = dbgEvent.u.CreateProcessInfo.hProcess;
			hThread = dbgEvent.u.CreateProcessInfo.hThread;

			//分配内存,填充注入指令  
			lpBaseAddress = VirtualAllocEx(hProcess,
				NULL,
				sizeof(INJECT_CODE),
				MEM_COMMIT | MEM_RESERVE,
				PAGE_EXECUTE_READWRITE);
			if (NULL == lpBaseAddress)
			{
				MessageBox(NULL,L"VirtualAllocEx 失败",L"Tip",MB_OK);
				return;
			}

			//给ShellCode结构体赋值
			ic.byMOV_EAX = 0xB8;
			ic.dwMOV_EAX_VALUE = (DWORD)lpBaseAddress + offsetof(INJECT_CODE, szDllPath);
			ic.byPUSH_EAX = 0x50;
			ic.byMOV_ECX = 0xB9;
			ic.dwMOV_ECX_VALUE = (DWORD)&LoadLibrary;
			ic.wCALL_ECX = 0xD1FF;
			ic.byINT3 = 0xCC;
			memcpy(ic.szDllPath, m_strDllPath.c_str(), m_strDllPath.size()*sizeof(TCHAR));

			//写入ShellCode
			bRet = WriteProcessMemory(hProcess, lpBaseAddress, &ic, sizeof(ic), NULL);
			if (!bRet)
			{
				MessageBox(NULL, L"WriteProcessMemory 失败", L"Tip", MB_OK);
				return;
			}


			//获取当前线程上下文
			bRet = GetThreadContext(hThread, &ctxOld);
			if (!bRet)
			{
				MessageBox(NULL, L"GetThreadContext 失败", L"Tip", MB_OK);
				return;
			}

			ctxNew = ctxOld;


#ifdef _WIN64
			ctxNew.Rip = (DWORD)lpBaseAddress;
			dwOldEip = ctxNew.Rip;
#else 
			ctxNew.Eip = (DWORD)lpBaseAddress;
			dwOldEip = ctxNew.Eip;
#endif

			bRet = SetThreadContext(hThread, &ctxNew);
			if (!bRet)
			{
				MessageBox(NULL, L"SetThreadContext 失败", L"Tip", MB_OK);
				return;
			}

			break;
		case EXCEPTION_DEBUG_EVENT:
			if (dbgEvent.u.Exception.ExceptionRecord.ExceptionCode == EXCEPTION_BREAKPOINT)
			{
				//屏蔽掉系统断点
				if (bIsSystemBp)
				{
					bIsSystemBp = FALSE;
					break;
				}

				//释放内存
				bRet = VirtualFreeEx(hProcess,
					lpBaseAddress,
					0,
					MEM_RELEASE
					);

				if (!bRet)
				{
					MessageBox(NULL, L"VirtualFreeEx 失败", L"Tip", MB_OK);
					return;
				}

				//恢复到程序创建时的EIP
				bRet = SetThreadContext(hThread, &ctxOld);
				if (!bRet)
				{
					TipBox(L"SetThreadContext 失败");
					return;
				}

				bRet = ContinueDebugEvent(dbgEvent.dwProcessId, dbgEvent.dwThreadId, DBG_CONTINUE);
				if (!bRet)
				{
					TipBox(L"ContinueDebugEvent 失败!!");
					return;
				}
				//退出本进程，让被调试程序跑起来
				//ExitProcess(0);
				return;

			}
			break;
		}

		bRet = ContinueDebugEvent(dbgEvent.dwProcessId, dbgEvent.dwThreadId, DBG_EXCEPTION_NOT_HANDLED);
		if (!bRet)
		{
			TipBox(L"ContinueDebugEvent 失败!!");
			return;
		}
	}
}
