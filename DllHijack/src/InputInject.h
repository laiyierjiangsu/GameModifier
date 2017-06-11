#pragma once
#include "..\stdafx.h"

// InputInject ¶Ô»°¿ò

class InputInject 
{
public:
	DWORD m_dwPID;
	std::string m_strDllPath;
	 void OnBnClickedInject(std::string dllPath);
};
