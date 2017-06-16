//检测线程的入口，所有逻辑都在这个里面做
#pragma  once
#include <thread>
class DetectorPortal
{
public:
	DetectorPortal();
	~DetectorPortal();
	bool Init();
	bool DeInit();
	//检测线程是否还在
	bool IsLive(DWORD iTime);
private:
	void DetectProcess();

	DWORD  _detectorThreadId;
	HANDLE  _detectorThreadHandle;
	DWORD   iCurrTime;
	std::thread thread;
};