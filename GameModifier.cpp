#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <TlHelp32.h>
#include <sys\timeb.h>

#define KONEK 1024
#define KPAGE 4*KONEK
#define KONEG KONEK*KONEK*KONEK
#define KFILELEN 60

BOOL CompareAPage(DWORD dwBaseAddr,DWORD dwValue);
BOOL FindFirst(DWORD dwValue);
BOOL FindNext(DWORD dwValue);
BOOL WriteMemory(DWORD dwValue);
void ShowAddList(void);
void editValue(DWORD dwId);
void showAllProcess();
BOOL closeProcess(DWORD dwId);
void showMenu();
DWORD getProcessId();
DWORD GetBaseAddress(DWORD dwPID);

DWORD g_dwAddList[KPAGE] = {0};
DWORD g_dwCount = 0;
HANDLE g_hProcess = NULL;
DWORD g_dwId = 0;

int main(int argc,char *argv[])
{
	UINT uIndex = 0;
	DWORD dwId;
	while(1)
	{
		showMenu();
		scanf("%d",&uIndex);
		switch (uIndex)
		{
		case 1:
			showAllProcess();
			break;
		case 2:
			editValue(getProcessId());
			break;
		case 3:
			closeProcess(getProcessId());
			break;
		case 4:
			system("pause");
			return 0;
			break;
		case 5:
			DWORD dwValue = GetBaseAddress(getProcessId());
			printf("基址：%#08x\n",dwValue);
			break;
		}
		system("pause");
	}
	
	system("pause");
	return 0;
}

void showMenu()
{
	system("cls");
	printf("1.进程列表\n");
	printf("2.修改内存\n");
	printf("3.结束进程\n");
	printf("4.退出系统\n");
	printf("5.得到基址\n");
	printf("请输入选择：");
}

DWORD getProcessId()
{
	DWORD dwId;
	printf("请输入进程ID:");
	scanf("%d",&dwId);
	g_dwId = dwId;
	return dwId;
}

#include <windows.h>
#include <stdint.h>
#include <memory>

#pragma comment(lib, "version.lib" )

bool GetOSVersionString(WCHAR* version, size_t maxlen)
{
	WCHAR path[_MAX_PATH];
	if (!GetSystemDirectoryW(path, _MAX_PATH))
		return false;

	wcscat_s(path, L"\\kernel32.dll");

	//
	// Based on example code from this article
	// http://support.microsoft.com/kb/167597
	//

	DWORD handle;
#if (_WIN32_WINNT >= _WIN32_WINNT_VISTA)
	DWORD len = GetFileVersionInfoSizeExW(FILE_VER_GET_NEUTRAL, path, &handle);
#else
	DWORD len = GetFileVersionInfoSizeW(path, &handle);
#endif
	if (!len)
		return false;

	std::unique_ptr<uint8_t> buff(new (std::nothrow) uint8_t[len]);
	if (!buff)
		return false;

#if (_WIN32_WINNT >= _WIN32_WINNT_VISTA)
	if (!GetFileVersionInfoExW(FILE_VER_GET_NEUTRAL, path, 0, len, buff.get()))
#else
	if (!GetFileVersionInfoW(path, 0, len, buff.get()))
#endif
		return false;

	VS_FIXEDFILEINFO *vInfo = nullptr;
	UINT infoSize;

	if (!VerQueryValueW(buff.get(), L"\\", reinterpret_cast<LPVOID*>(&vInfo), &infoSize))
		return false;

	if (!infoSize)
		return false;

	swprintf_s(version, maxlen, L"%u.%u.%u.%u",
		HIWORD(vInfo->dwFileVersionMS),
		LOWORD(vInfo->dwFileVersionMS),
		HIWORD(vInfo->dwFileVersionLS),
		LOWORD(vInfo->dwFileVersionLS));

	return true;
}
void editValue(DWORD dwId)
{
	g_hProcess = OpenProcess(PROCESS_ALL_ACCESS,FALSE,dwId);
	if (!g_hProcess)
	{
		printf("打开进程%s失败\n",dwId);
		return;
	}

	DWORD dwValue;
	printf("请输入第一次的值:");
	scanf("%d",&dwValue);

	FindFirst(dwValue);

	ShowAddList();

	printf("请输入第二次的值：");
	scanf("%d",&dwValue);

	FindNext(dwValue);

	ShowAddList();

	printf("请输入要新值:");
	scanf("%d",&dwValue);
	WriteMemory(dwValue);
}

BOOL CompareAPage(DWORD dwBaseAddr,DWORD dwValue)
{
	BYTE bytes[KPAGE];
	if (!ReadProcessMemory(g_hProcess,(LPCVOID)dwBaseAddr,bytes,KPAGE,NULL))
	{
		//printf("读取内存失败\n");
		return FALSE;
	}

	DWORD *pdw = (DWORD*)bytes;

	for (int i=0;i<KONEK;i++)
	{
		if (pdw[i] == dwValue)
		{
			g_dwAddList[g_dwCount++] = dwBaseAddr + i*sizeof(DWORD);
		}		
	}
	return TRUE;
}

BOOL FindFirst(DWORD dwValue)
{
	OSVERSIONINFO vi = {sizeof(vi)};
	//GetVersionEx(&vi);
	DWORD dwBase;

	if (vi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
	{
		printf("Windows 98\n");
		dwBase = 4 * KONEK * KONEK;
	}
	else if (vi.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		printf("Windows NT\n");
		dwBase = 64 * KONEK;
	}
	g_dwCount = 0;

	DWORD dwOld = 0;
	DWORD dwNew = 0;

	struct timeb start;
	struct timeb end;

	printf("搜索中...\n%%%02d",0.0);

	ftime(&start);

	//dwBase = GetBaseAddress(g_dwId);
	for (;dwBase < 2 * KONEG;dwBase+=KPAGE)
	{
		dwNew = dwBase/(KONEG/50);
		if (dwNew != dwOld)
		{
			printf("\b\b%02d",dwNew);
			dwOld = dwNew;
		}
		
		CompareAPage(dwBase,dwValue);
	}
	ftime(&end);
	printf("\b\b100\n搜索完成\n");
	
	printf("用时%d毫秒\n",(end.time-start.time)*1000+ end.millitm-start.millitm);

	return TRUE;
}

BOOL FindNext(DWORD dwValue)
{
	DWORD dwCount = 0;
	DWORD dwValue1 = 0;

	for (int i=0;i<g_dwCount;i++)
	{
		if (!ReadProcessMemory(g_hProcess,(LPCVOID)g_dwAddList[i],&dwValue1,sizeof(DWORD),NULL))
		{
			//printf("读取内存失败\n");
			return FALSE;
		}
		if (dwValue1 == dwValue)
		{
			g_dwAddList[dwCount++] = g_dwAddList[i];
		}
	}
	g_dwCount = dwCount;

	return TRUE;
}

BOOL WriteMemory(DWORD dwValue)
{
	for (int i=0;i<g_dwCount;i++)
	{
		if (!WriteProcessMemory(g_hProcess,(LPVOID)g_dwAddList[i],(LPCVOID)&dwValue,sizeof(DWORD),NULL))
		{
			return FALSE;
		}
		
	}	
	return TRUE;
}


void ShowAddList(void)
{
	printf("地址列表...\n");
	for (int i=0;i<g_dwCount;i++)
	{
		printf("%#010x\n",g_dwAddList[i]);
	}
}

BOOL closeProcess(DWORD dwId)
{
	BOOL bRet = FALSE;
	HANDLE hHandle = OpenProcess(PROCESS_ALL_ACCESS,FALSE,dwId);

	if (hHandle != NULL)
	{
		bRet = TerminateProcess(hHandle,0);
	}
	CloseHandle(hHandle);
	return bRet;
}

void showAllProcess()
{
	PROCESSENTRY32 pc;
	pc.dwSize = sizeof(pc);

	HANDLE dProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);

	if (INVALID_HANDLE_VALUE == dProcessSnap)
	{
		printf("获得进程失败");
		system("pause");
		return ;
	}

	BOOL bMore = Process32First(dProcessSnap,&pc);

	while (bMore)
	{
		printf("进程ID:%4d | 进程名称：%s\n",pc.th32ProcessID,pc.szExeFile);
		bMore = Process32Next(dProcessSnap,&pc);
	}

	CloseHandle(dProcessSnap);

}

//////////////////////////////////////////////////////////////////////////
//   函数功能： 获取exe模块的加载地址
//   参   数： dwPID：进程的pid； 
//   返 回 值： 返回exe模块基址；
//////////////////////////////////////////////////////////////////////////
DWORD GetBaseAddress(DWORD dwPID)
{
	HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
	MODULEENTRY32 me32;
	// Take a snapshot of all modules in the specified process.
	hModuleSnap = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, dwPID );
	if( hModuleSnap == INVALID_HANDLE_VALUE )
	{
		printf("失败!");
		return 0;
	}

	me32.dwSize = sizeof( MODULEENTRY32 );
	
	if( !Module32First( hModuleSnap, &me32 ) )
	{
		CloseHandle( hModuleSnap );           // clean the snapshot object
		return 0;
	}
	DWORD Value = (DWORD)me32.modBaseAddr;
	CloseHandle( hModuleSnap );
	return Value;
}