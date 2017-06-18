#pragma once
#include <string>
#include <list>
using WndList = std::list<std::wstring>;
using WndListIter = WndList::iterator;
class WindowDetector
{
public:
	WindowDetector();
	~WindowDetector();
	void Detect();
private:
	void RegisterWndName();
	void CheckWnd();
	WndList _wndList;
};

