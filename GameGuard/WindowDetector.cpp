#include "WindowDetector.h"
#include "Framework.h"
WindowDetector::WindowDetector()
{
	RegisterWndName();
}

WindowDetector::~WindowDetector()
{
}



void WindowDetector::CheckWnd()
{
	for (auto item :_wndList)
	{
		HWND hwnd = FindWindowW(item.c_str(), NULL);
		if (hwnd)
		{
			wprintf(L"Find window :%s \n", item.c_str());
		}
	}
}

void WindowDetector::RegisterWndName()
{
	//枚举所有的窗口
	// ollyice
	_wndList.push_back(L"1212121");
	_wndList.push_back(L"icu_dbg");
	_wndList.push_back(L"pe--diy");
	_wndList.push_back(L"ollydbg");
	_wndList.push_back(L"odbydyk");
	_wndList.push_back(L"WinDbgFrameClass");
	_wndList.push_back(L"TDeDeMainForm");
	_wndList.push_back(L"TIdaWindow");
	_wndList.push_back(L"TIdaWindow");
	_wndList.push_back(L"TESTDBG");
	_wndList.push_back(L"kk1");
	_wndList.push_back(L"Eew75");
	_wndList.push_back(L"Shadow");
	_wndList.push_back(L"PEiD v0.94");
	_wndList.push_back(L"Registry Monitor - Sysinternals: www.sysinternals.com");
	_wndList.push_back(L"File Monitor - Sysinternals: www.sysinternals.com");
	_wndList.push_back(L"Import REConstructor v1.6 FINAL (C) 2001-2003 MackT/uCF");
	_wndList.push_back(L"PEiD v0.94");
}

void WindowDetector::Detect()
{
	CheckWnd();
	EnumAllWnd();
}
BOOL CALLBACK printVisibleWindows(HWND wnd, LPARAM lParam)
{
	if (!IsWindowVisible(wnd))return 1;
	int length = GetWindowTextLength(wnd);
	if (!length)return 1;
	length++;
	TCHAR *buffer = new TCHAR[length];
	GetWindowText(wnd, buffer, length);
	wprintf(L"%d %s\n", wnd, buffer);
	delete[] buffer;
	return 1;
}
void WindowDetector::EnumAllWnd()
{
	DWORD dwThreadId = GetCurrentThreadId();
	HDESK hDesktop = GetThreadDesktop(dwThreadId);
	printf("------begin-----------\n");
	EnumDesktopWindows(hDesktop, printVisibleWindows, 0);
	printf("--------end--------------\n");
	return ;	
}
