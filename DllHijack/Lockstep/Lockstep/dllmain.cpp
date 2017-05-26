// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

#ifndef __ILOCKSTEP_H__
#define __ILOCKSTEP_H__

#ifdef LOCKSTEP_EXPORTS
#define LOCKSTEP_API __declspec(dllexport)
#else
#define LOCKSTEP_API __declspec(dllimport)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
					  DWORD  ul_reason_for_call,
					  LPVOID lpReserved
					  )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			MessageBoxA(nullptr,"DLL_PROCESS_ATTACH","DLL_PROCESS_ATTACH",MB_OK);
			break;
		}
	case DLL_THREAD_ATTACH:
		{
			MessageBoxA(nullptr,"DLL_PROCESS_ATTACH","DLL_PROCESS_ATTACH",MB_OK);
			break;
		}
	case DLL_THREAD_DETACH:
		{
			MessageBoxA(nullptr,"DLL_PROCESS_ATTACH","DLL_PROCESS_ATTACH",MB_OK);
			break;
		}
	case DLL_PROCESS_DETACH:
		{
			MessageBoxA(nullptr,"DLL_PROCESS_ATTACH","DLL_PROCESS_ATTACH",MB_OK);
			break;
		}

	}
	return TRUE;
}


#endif

