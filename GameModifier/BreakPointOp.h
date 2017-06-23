#pragma  once
#include <windows.h>
class BreakPointOp
{
public:
	BreakPointOp();
	BOOL RemoveBreakPoint(HANDLE hProcess, UINT nNum);
	BOOL SetBreakPoint(HANDLE hProcess, LPVOID lpAddr, UINT nNum);
protected:
private:
	char * g_bpData;
};
