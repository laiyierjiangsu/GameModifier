#pragma  once
#include <windows.h>
#include <TlHelp32.h>
#include <string>
#include <stdio.h>
inline std::string Utils_WideChar_To_Utf8(wchar_t* wc)
{
	int len = WideCharToMultiByte(CP_ACP, 0, wc, wcslen(wc), NULL, 0, NULL, NULL);
	char* m_char = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, wc, wcslen(wc), m_char, len, NULL, NULL);
	m_char[len] = '\0';
	std::string rRet = std::string(m_char);
	delete[] m_char;
	return rRet;
}
inline std::wstring CharToWchar(char* c)
{

	int len = MultiByteToWideChar(CP_ACP, 0, c, strlen(c), NULL, 0);
	wchar_t* m_wchar = new wchar_t[len + 1];
	MultiByteToWideChar(CP_ACP, 0, c, strlen(c), m_wchar, len);
	m_wchar[len] = '\0';
	std::wstring sRet = std::wstring(m_wchar);
	return sRet;
}

inline DWORD GetProcessIdByName(std::string name)
{
	// 定义进程信息结构  
	PROCESSENTRY32 pe32 = { sizeof(pe32) };

	// 创建系统当前进程快照  
	HANDLE hProcessShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessShot == INVALID_HANDLE_VALUE)
		return FALSE;

	std::wstring strWname = CharToWchar((char*)name.c_str());
	if (Process32First(hProcessShot, &pe32))
	{
		do
		{
			if (wcscmp(pe32.szExeFile, strWname.c_str()) == 0)
			{
				return pe32.th32ProcessID;
			}

		} while (Process32Next(hProcessShot, &pe32));
	}
	return 0;
}
