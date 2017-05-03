// ConsoleCrakeMe.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
char* test = "NBA2kOnline.exe";
int main()
{
	GetCurrentThread();
	printf("Currentthreadid: %d\n", GetCurrentThreadId());
	printf(test);
	getchar();
    return 0;
}

