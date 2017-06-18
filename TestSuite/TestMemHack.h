#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <TlHelp32.h>
#include <sys\timeb.h>
#include "Utils.h"
#define KILO 1024
#define PAGE 4*KILO //4k
#define MEGE KILO*KILO*KILO
#define KFILELEN 60

class TestMemhack
{
public:
	TestMemhack();
	~TestMemhack();

	void Test();
private:
	BOOL CompareAPage(DWORD dwBaseAddr, DWORD dwValue);
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
	DWORD GetProcessIDByName(wchar_t * pName);
	bool GetOSVersionString(VS_FIXEDFILEINFO *vInfo);
private:
	DWORD* g_dwAddList;
	DWORD g_dwCount ;
	HANDLE g_hProcess;;
	DWORD g_dwId ;
};










