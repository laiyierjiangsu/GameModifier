// ExeToHook.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Framework.h"
#include "WindowDetector.h"


#include "DetectorProtal.h"
DetectorPortal _globalDetector;
int _tmain(int argc, _TCHAR* argv[])
{
	
	_globalDetector.Init();
	static int i = 0;
	while (true)
	{
		Sleep(3000);
		bool bLive = _globalDetector.IsLive(timeGetTime());
		printf("detecor is live £¿£¿ %s\n",bLive ?"true":"false" );
		printf("wait for hacking %d\n", i++);
	}

	return 0;
}

