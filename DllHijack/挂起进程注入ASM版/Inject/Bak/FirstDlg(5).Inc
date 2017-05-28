
include windows.inc
include kernel32.inc
include user32.inc
include Comctl32.inc
include shell32.inc
include ntdll.inc


includelib kernel32.lib
includelib user32.lib
includelib Comctl32.lib
includelib shell32.lib
includelib ntdll.lib
DlgProc			PROTO	:HWND,:UINT,:WPARAM,:LPARAM
InjectCode proto
MyFreeMem proto
.const

IDD_INJECT			equ 101
BTN_INJECT		    equ 1001
IDC_BTN1                    equ 1002
;#########################################################################

.data
	g_szCalc db 'MFCApplication2', 0
	g_szUser32 db 'user32.dll', 0
	g_szMsgBox db 'MessageBoxA', 0
	g_szLoadLibrary db 'LoadLibrary err',0
	g_szGetProcAddress db 'GetProcAddress err',0
	g_szVirtualProtect db 'VirtualProtect err',0
	g_szFreeLibrary db 'FreeLibrary err',0
	g_szFindWindow db 'FindWindow err',0
	g_szGetWindowThreadProcessId db 'GetWindowThreadProcessId err',0
	g_szOpenProcess db 'OpenProcess err',0
	g_szVirtualAllocEx db 'VirtualAllocEx err',0
	g_szWriteProcessMemory db 'WriteProcessMemory err',0
	g_szCreateRemoteThread db 'CreateRemoteThread err',0
	g_szVirtualFreeEx db 'VirtualFreeEx err',0
	g_szSuspendThread db 'SuspendThread err',0
	g_szCreateToolhelp32Snapshot db 'CreateToolhelp32Snapshot err',0
	g_szGetThreadContext db 'GetThreadContext err',0
	g_szSetThreadContext db 'SetThreadContext err',0
	g_szResumeThread db 'SResumeThread err',0
	g_err db 'ERROR',0
	g_strExePath db 'Server.exe',0
	
	
.data?
    g_szAddr   dd ?
    g_pOldEip  DWORD ?
    g_hProcess DWORD ?
    g_lpBuff   DWORD ?
    
hInstance			dd ?

;#########################################################################
