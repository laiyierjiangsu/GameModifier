#include "DllInjectDetector.h"
#include "ThirdParty/MiniHook/include/MinHook.h"
#include "Framework.h"
#include <psapi.h>
typedef struct _UNICODE_STRING { // UNICODE_STRING structure  
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
} UNICODE_STRING;
typedef UNICODE_STRING *PUNICODE_STRING;


typedef NTSTATUS(WINAPI *fLdrLoadDll) //LdrLoadDll function prototype  
(
IN PWCHAR PathToFile OPTIONAL,
IN ULONG Flags OPTIONAL,
IN PUNICODE_STRING ModuleFileName,
OUT PHANDLE ModuleHandle
);


typedef VOID(WINAPI *fRtlInitUnicodeString) //RtlInitUnicodeString function prototype  
(
PUNICODE_STRING DestinationString,
PCWSTR SourceString
);
fLdrLoadDll pOrigin = nullptr;
NTSTATUS WINAPI MyLoadDll(
	IN PWCHAR PathToFile OPTIONAL,
	IN ULONG Flags OPTIONAL,
	IN PUNICODE_STRING ModuleFileName,
	OUT PHANDLE ModuleHandle
	)
{
	return (pOrigin)(PathToFile, Flags, ModuleFileName, ModuleHandle);
};


DWORD m_dwUser32Start;
DWORD m_dwUser32End;
typedef HMODULE(WINAPI*fLoadLibraryW)(LPCWSTR  lpLibFileName);
fLoadLibraryW rawLoadLibraryW;
HMODULE WINAPI NLoadLibraryW(LPCWSTR  lpLibFileName)
{
	//get the return address
	DWORD dwCaller;
	//ebp+4返回上层调用者的地址
	__asm push dword ptr[ebp + 4]
		__asm pop  dword ptr[dwCaller]
		if (dwCaller > m_dwUser32Start && dwCaller < m_dwUser32End)
		{
			return FALSE;
		}

		return rawLoadLibraryW(lpLibFileName);
}


typedef HMODULE(WINAPI*fLoadLibrary)(LPCTSTR   lpLibFileName);
fLoadLibrary rawLoadLibrary;
HMODULE WINAPI NLoadLibrary(LPCTSTR   lpLibFileName)
{
	//get the return address
	DWORD dwCaller;
	//ebp+4返回上层调用者的地址
	__asm push dword ptr[ebp + 4]
		__asm pop  dword ptr[dwCaller]
		if (dwCaller > m_dwUser32Start && dwCaller < m_dwUser32End)
		{
			return FALSE;
		}

		return rawLoadLibrary(lpLibFileName);
}



DllInjectDetector::DllInjectDetector()
{
	bInitialized = false;
}

DllInjectDetector::~DllInjectDetector()
{
}

bool DllInjectDetector::Init()
{
	
	MH_STATUS hr = MH_Initialize();
	False_Return(hr == MH_OK);
	hr = MH_CreateHookApi(L"Ntdll.dll", "LdrLoadDll", (LPVOID)&MyLoadDll, (LPVOID*)&pOrigin);
	False_Return(hr == MH_OK);
	//hr = MH_CreateHookApi(L"Kernel32.dll", "LoadLibraryW", (LPVOID)&NLoadLibraryW, (LPVOID*)&rawLoadLibraryW);
	//False_Return(hr == MH_OK);
	//hr = MH_CreateHookApi(L"Kernel32.dll", "LoadLibraryA", (LPVOID)&NLoadLibrary, (LPVOID*)&rawLoadLibrary);
	//False_Return(hr == MH_OK);

	//Hook Ntdll!LdrLoadDll？？？ hook createremotethread ? ? Hook writeprocessmemory ?
	hr = MH_EnableHook(MH_ALL_HOOKS);
	False_Return(hr == MH_OK);
	bInitialized = true;
	return true;
}

bool DllInjectDetector::DeInit()
{
	MH_STATUS hr = MH_Uninitialize();
	False_Return(hr == MH_OK);
	bInitialized = false;
	return true;
}

bool DllInjectDetector::EnumAllModulesOfProcess()
{
	static int iTick = 0;
	HMODULE hMods[1024];
	HANDLE hProcess = GetCurrentProcess();
	DWORD cbNeeded;
	unsigned int i;
	// Get a list of all the modules in this process.
	printf("Enum all moudules in process -------------------------begin!\n");
	if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
	{
		int iCount = cbNeeded / sizeof(HMODULE);
		for (i = 0; i < iCount; i++)
		{
			TCHAR szModName[MAX_PATH];
			// Get the full path to the module's file.
			if (GetModuleFileNameEx(hProcess, hMods[i], szModName,
				sizeof(szModName) / sizeof(TCHAR)))
			{
				wprintf(L"%s \n", szModName);
			}
		}
		printf("Enum all moudules %d------Count:%d-----------End!\n",iTick++,iCount);
	}
	else
	{
		printf("OpenProcessToken() failed with code: %d \n", GetLastError());
	}

	// Release the handle to the process.
	CloseHandle(hProcess);
	return true;
}

void DllInjectDetector::Detect()
{
	if (!bInitialized)
	{
		Init();
	}
	LoadLibrary(L"InjectedDll.dll");
	EnumAllModulesOfProcess();
}
