#pragma once

#include "..\stdafx.h"
// ProcessInject ¶Ô»°¿ò

class ProcessInject 
{
public:
	std::string m_strExePath;
	std::string m_strDllPath;
	void OnBnClickedInject(std::string exePath, std::string dllPath);
	 void OnBnClickedFreemem();

};
