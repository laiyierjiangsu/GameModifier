// ExeToHook.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <thread>
#include "MiniHook/include/MinHook.h"
#include <assert.h>
void ThreadFun()
{
	static int i = 0;
	while (true)
	{
		printf("Thread fun excute %d\n", i++);
		Sleep(1000);
	}

}
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


int _tmain(int argc, _TCHAR* argv[])
{
	MH_STATUS hr = MH_Initialize();
	assert(hr == MH_OK);
	hr = MH_CreateHookApi(L"Ntdll.dll", "LdrLoadDll", (LPVOID)&MyLoadDll, (LPVOID*)&pOrigin);
	assert(hr == MH_OK);
	//Hook Ntdll!LdrLoadDll£¿£¿£¿ hook createremotethread ? ? Hook writeprocessmemory ?
	hr = MH_EnableHook(MH_ALL_HOOKS);
	assert(hr == MH_OK);
	std::thread thread = std::thread(&ThreadFun);
	static int i = 0;
	while (true)
	{
		Sleep(3000);
		printf("wait for hacking %d\n", i++);
	}
	hr = MH_Uninitialize();
	assert(hr == MH_OK);
	return 0;
}

