#pragma once
#include <string>
#include <list>
typedef  std::list<std::wstring> EXEList;
typedef  EXEList::iterator EXElIstIter;
class ProcessDetector
{
public:
	ProcessDetector();
	~ProcessDetector();
	void Detect();
private:
	bool FindProcess(std::wstring name);
	bool EnumProcessInfo();
	void InitExeList();
private:
	EXEList  _exeList;
};

