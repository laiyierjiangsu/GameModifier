// ExeToHook.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <thread>
void ThreadFun()
{
	static int i = 0;
	while (true)
	{
		printf("Thread fun excute %d\n",i++);
		Sleep(1000);
	}

}
int _tmain(int argc, _TCHAR* argv[])
{
	std::thread thread = std::thread(&ThreadFun);
	static int i = 0;
	while (true)
	{
		Sleep(3000);
		printf("wait for hacking %d\n", i++);
	}
	return 0;
}

