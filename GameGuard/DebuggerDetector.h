#pragma  once
class DebuggerDetector
{
public:
	DebuggerDetector();
	~DebuggerDetector();

	bool Detect();
private:
	//Check_debug
	bool NGuard_IsDebuggerPresent();
	bool NGuard_CheckByOutputDebugString();
	bool NGuard_NtQueryInfoProc_DbgPort();
	bool NGuard_NtQueryInfoProc_SYSTEM_INFORMATION();
	bool NGuard_NtQueryInfoProc_ZW_QUERY_INFORMATION_PROCESS();

	//Ban debugger
	bool NGuard_NtQueryInfoProc_SET_INFORMATION_THREAD();
	


};

