// ProcessInject.cpp : 实现文件
//


#include "ProcessInject.h"

//ShellCode结构体
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



// ProcessInject 消息处理程序

HANDLE g_hProcess1 = NULL;
LPVOID g_lpBuffer1 = NULL;
void ProcessInject::OnBnClickedInject(std::string exePath,std::string dllPath)
{
	m_strExePath = exePath;
	m_strDllPath = dllPath;
	// TODO:  在此添加控件通知处理程序代码
	// TODO:  在此添加控件通知处理程序代码
	BOOL bRet = FALSE;
	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	CONTEXT oldContext = { 0 };
	CONTEXT newContext = { 0 };
	INJECT_CODE ic = { 0 };
	DWORD dwOldEip = 0;
	si.wShowWindow = SW_SHOWDEFAULT;
	si.cb = sizeof(PROCESS_INFORMATION);
	HANDLE hThread = NULL;
	//以挂起的方式创建进程
	std::wstring path = string2Wstring(m_strExePath);
	bRet = CreateProcess(path.c_str(), NULL, NULL, NULL, FALSE, CREATE_SUSPENDED,
		NULL, NULL, &si, &pi);

	if (!bRet)
	{
		TipBox("CreateProcess 失败");
		return;
	}

	g_hProcess1 = pi.hProcess;
	hThread = pi.hThread;
	//申请内存
	g_lpBuffer1 = VirtualAllocEx(g_hProcess1, NULL, 0x1000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	if (g_lpBuffer1 == NULL)
	{
		TipBox("VirtualAllocEx 失败");
		return;
	}

	//给ShellCode结构体赋值
	ic.byPUSH = 0x68;
	ic.dwPUSH_VALUE = 0x12345678;
	ic.byPUSHFD = 0x9C;
	ic.byPUSHAD = 0x60;
	ic.byMOV_EAX = 0xB8;
	ic.dwMOV_EAX_VALUE = (DWORD)g_lpBuffer1 + offsetof(INJECT_CODE, szDllPath);
	ic.byPUSH_EAX = 0x50;
	ic.byMOV_ECX = 0xB9;
	ic.dwMOV_ECX_VALUE = (DWORD)&LoadLibrary;
	ic.wCALL_ECX = 0xD1FF;
	ic.byPOPAD = 0x61;
	ic.byPOPFD = 0x9D;
	ic.byRETN = 0xC3;
	memcpy(ic.szDllPath, m_strDllPath.c_str(), m_strDllPath.length());

	//写入ShellCode
	bRet = WriteProcessMemory(g_hProcess1, g_lpBuffer1, &ic, sizeof(ic), NULL);
	if (!bRet)
	{
		TipBox("写入内存失败");
		return;
	}

	//获取线程上下文
	oldContext.ContextFlags = CONTEXT_FULL;
	bRet = GetThreadContext(hThread, &oldContext);
	if (!bRet)
	{
		TipBox("GetThreadContext 失败");
		return;
	}
	newContext = oldContext;


#ifdef _WIN64
	newContext.Rip = (DWORD)g_lpBuffer1;
	dwOldEip = newContext.Rip;
#else 
	newContext.Eip = (DWORD)g_lpBuffer1;
	dwOldEip = newContext.Eip;
#endif



	//;将指针指向ShellCode第一句push 12345678h中的地址,写入返回地址
	bRet = WriteProcessMemory(g_hProcess1, ((char*)g_lpBuffer1) + 1, &dwOldEip, sizeof(DWORD), NULL);
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


void ProcessInject::OnBnClickedFreemem()
{
	// TODO:  在此添加控件通知处理程序代码
	if (!VirtualFreeEx(g_hProcess1, g_lpBuffer1, 0, MEM_RELEASE))
	{
		MessageBox("VirtualFreeEx 失败");
		return;
	}

	MessageBox("释放对方空间成功");
}


void ProcessInject::OnBnClickedButton3()
{
	// TODO:  在此添加控件通知处理程序代码
	char szFilter[] = "可执行程序|*.exe";
	CFileDialog fileDlg(TRUE, "exe", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
	UpdateData(TRUE);
	if (fileDlg.DoModal() == IDOK)
	{
		
	}
	UpdateData(FALSE);
}


void ProcessInject::OnBnClickedButton4()
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
