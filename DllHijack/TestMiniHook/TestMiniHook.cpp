// TestMiniHook.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MiniHook/include/MinHook.h"
#include <assert.h>
int _tmain(int argc, _TCHAR* argv[])
{

	MH_STATUS hr = MH_Initialize();
	assert(hr == MH_OK);

	hr = MH_Uninitialize();
	assert(hr == MH_OK);
	return 0;
}

