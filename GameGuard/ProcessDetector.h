#pragma once
#include <string>
#include <list>
using EXEList = std::list<std::wstring>;
using EXElIstIter = EXEList::iterator;
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

