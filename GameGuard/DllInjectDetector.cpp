#include "DllInjectDetector.h"
#include "MiniHook/include/MinHook.h"
#include "Framework.h"
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


DllInjectDetector::DllInjectDetector()
{
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
	//Hook Ntdll!LdrLoadDll£¿£¿£¿ hook createremotethread ? ? Hook writeprocessmemory ?
	hr = MH_EnableHook(MH_ALL_HOOKS);
	False_Return(hr == MH_OK);
	return true;
}

bool DllInjectDetector::DeInit()
{
	MH_STATUS hr = MH_Uninitialize();
	False_Return(hr == MH_OK);
	return true;
}
