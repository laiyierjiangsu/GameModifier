#pragma once
#include <string>
#include <list>
typedef std::list<std::wstring> WndList ;
typedef WndList::iterator WndListIter;
class WindowDetector
{
public:
	WindowDetector();
	~WindowDetector();
	void Detect();
private:
	void RegisterWndName();
	void CheckWnd();
	void EnumAllWnd();
private:
	WndList _wndList;
};

