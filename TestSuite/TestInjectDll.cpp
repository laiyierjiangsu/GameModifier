#include "TestInjectDll.h"
#include "GameModifier/APCInject.h"
#include "GameModifier/DebugInject.h"
#include "GameModifier/HookInject.h"
#include "GameModifier/RemoteInject.h"
#include "GameModifier/ThreadInject.h"
#include "GameModifier/ProcessInject.h"
#include "include/Framework.h"
APCInject stAPCInject;
DebugInject stDebugInject;
HookInject stHookInject;
RemoteInject stRemoteInject;
ThreadInject stThreadInject;
ProcessInject stProcessInject;
TestInjectDll::TestInjectDll()
{
}

TestInjectDll::~TestInjectDll()
{
}

void TestInjectDll::Test()
{
	//stAPCInject.StartInject(L"GameGuard.exe", L"InjectedDll.dll");
	//stDebugInject.StartInject(L"GameGuard.exe", L"InjectedDll.dll");
	//stBlockHook.OnBlockHook();
	//stHookInject.StartInject(L"GameGuard.exe", L"InjectedDll.dll");
	 stRemoteInject.StartInject("GameGuard.exe", "InjectedDll.dll");
	//stThreadInject.StartInject(L"GameGuard.exe", L"InjectedDll.dll");
	//stProcessInject.OnStartInject("GameGuard.exe", "InjectedDll.dll");
	//TestLoadInjectedDll();
}
typedef  void (* ExportFun)();
void TestInjectDll::TestLoadInjectedDll()
{
	HMODULE hModule =  LoadLibrary(L"InjectedDll.dll");
	assert(hModule != nullptr);
	ExportFun proc = (ExportFun)GetProcAddress(hModule, "TestMessageBox");     
	assert(NULL != proc);
	(*proc)();
}
