#include "DetectorProtal.h"
#include "Framework.h"
#include <thread>
#pragma comment(lib, "Winmm.lib")
#include "FileIntegrityDetection.h"
#include "ImageIntegrityDetector.h"
#include "WindowDetector.h"
#include "ProcessDetector.h"
FileIntegrityDetector _gFileIntegrityDetector;
ImageIntegrityDetector _gImageIntegrityDetector;
WindowDetector		_gWndDetector;
ProcessDetector		_gProcessDetector;
DetectorPortal::DetectorPortal()
{
	iCurrTime = 0;
}

DetectorPortal::~DetectorPortal()
{

}

void DetectorPortal ::DetectProcess()
{
	static int i = 0;
	_detectorThreadId = GetCurrentThreadId();
	_detectorThreadHandle = GetCurrentThread();
	while (true)
	{
		_gFileIntegrityDetector.Detect();
		//_gImageIntegrityDetector.Detect();
		_gWndDetector.Detect();
		_gProcessDetector.Detect();
		//printf("Thread fun excute %d\n", i++);
		Sleep(5000);
		//break;
	}
	printf("Detector thread has been exit!");
	iCurrTime = timeGetTime();
	//CloseHandle(_detectorThreadId);
}
bool DetectorPortal::Init()
{
	thread = std::thread(&DetectorPortal::DetectProcess, this);
	thread.detach();
	return true;
}

bool DetectorPortal::DeInit()
{
	return true;
}

bool DetectorPortal::IsLive(DWORD iTime)
{
	if (iCurrTime == 0)
	{
		return true;
	}
	DWORD iGap = iTime - iCurrTime;
	return std::abs(long(iGap)) < 20000;
	//CONTEXT ThreadCon;
	//return GetThreadContext(_detectorThreadHandle, &ThreadCon);
	//return thread.joinable();
	//DWORD exitflag = 0;
	//if (GetExitCodeThread(_detectorThreadHandle, &exitflag))
	//{
	//	if (exitflag == STILL_ACTIVE)
	//	{
	//		return true;
	//	}
	//	else
	//	{
	//		return false;
	///	}
		//DWORD dw = WaitForSingleObject(_detectorThreadId, 0);
		//return (dw == WAIT_TIMEOUT);   //线程正在运行  
	//}
	//else
	//{
	//	return false;
	//}

}
