#pragma once

#include <windows.h>
#include <string>

// DebugInject ¶Ô»°¿ò

class DebugInject 
{
public:
	void StartInject(std::wstring stExe, std::wstring strDll);
private:
	std::wstring m_strExePath;
	std::wstring m_strDllPath;


};
