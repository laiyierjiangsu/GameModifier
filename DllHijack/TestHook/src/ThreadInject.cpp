// ThreadInject.cpp : 实现文件
//

#include "stdafx.h"
#include "MyInjectTool.h"
#include "ThreadInject.h"
#include "afxdialogex.h"

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

IMPLEMENT_DYNAMIC(ThreadInject, CDialogEx)

ThreadInject::ThreadInject(CWnd* pParent /*=NULL*/)
	: CDialogEx(ThreadInject::IDD, pParent)
	, m_dwPid(0)
	, m_strDllPath(_T(""))
{

}

ThreadInject::~ThreadInject()
{
}

void ThreadInject::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_dwPid);
	DDX_Text(pDX, IDC_EDIT4, m_strDllPath);
}


BEGIN_MESSAGE_MAP(ThreadInject, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON2, &ThreadInject::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &ThreadInject::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &ThreadInject::OnBnClickedButton4)
END_MESSAGE_MAP()


// ThreadInject 消息处理程序


void ThreadInject::OnBnClickedButton2()
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

HANDLE g_hProcess = 0;
LPVOID g_lpBuffer = NULL;
void ThreadInject::OnBnClickedButton3()
{
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
		MessageBox("OpenProcess 失败");
		return;
	}


	g_lpBuffer = VirtualAllocEx(g_hProcess, NULL, 0x1000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (!g_lpBuffer)
	{
		MessageBox("VirtualAllocEx 失败");
		return;
	}

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
	memcpy(ic.szDllPath, m_strDllPath.GetBuffer(0), m_strDllPath.GetLength());

	//写入ShellCode
	bRet = WriteProcessMemory(g_hProcess, g_lpBuffer, &ic, sizeof(ic), NULL);
	if (!bRet)
	{
		MessageBox("写入内存失败");
		return;
	}

	//创建线程快照查找目标程序主线程
	te32.dwSize = sizeof(te32);
	hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hThreadSnap == INVALID_HANDLE_VALUE)
	{
		MessageBox("CreateToolhelp32Snapshot 失败");
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

	if (!OpenThread)
	{
		MessageBox("OpenThread 失败");
		return;
	}

	//挂起目标主线程
	bRet = SuspendThread(hThread);

	if (bRet == -1)
	{
		MessageBox("SuspendThread 失败");
		return;
	}

	oldContext.ContextFlags = CONTEXT_FULL;
	bRet = GetThreadContext(hThread, &oldContext);
	if (!bRet)
	{
		MessageBox("GetThreadContext 失败");
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
		MessageBox("写入内存失败");
		return;
	}

	bRet = SetThreadContext(hThread, &newContext);

	if (!bRet)
	{
		MessageBox("SetThreadContext 失败");
		return;
	}

	//然后把主线程跑起来
	bRet = ResumeThread(hThread);
	
	if (bRet == -1)
	{
		MessageBox("ResumeThread 失败");
		return;
	}


}


void ThreadInject::OnBnClickedButton4()
{
	// TODO:  在此添加控件通知处理程序代码
	if (!VirtualFreeEx(g_hProcess, g_lpBuffer, 0, MEM_RELEASE))
	{
		MessageBox("VirtualFreeEx 失败");
		return;
	}

	MessageBox("释放对方空间成功");
}
