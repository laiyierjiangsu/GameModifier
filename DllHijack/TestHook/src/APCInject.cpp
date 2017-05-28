// APCInject.cpp : ʵ���ļ�
//

#include "APCInject.h"

void APCInject::StartInject(std::wstring stExe, std::wstring strDll)
{
	m_strExePath = stExe;
	m_strDllPath = strDll;

	// TODO:  �ڴ����ӿؼ�֪ͨ�����������
	DWORD dwRet = 0;
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	ZeroMemory(&pi, sizeof(pi));
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(STARTUPINFO);

	//�Թ���ķ�ʽ��������
	dwRet = CreateProcessW(m_strExePath.c_str(),
		NULL,
		NULL,
		NULL,
		FALSE,
		CREATE_SUSPENDED,
		NULL,
		NULL,
		&si,
		&pi);

	if (!dwRet)
	{
		MessageBoxW(NULL,L"CreateProcessʧ�ܣ���",L"Tip",MB_OK);
		return;
	}

	PVOID lpDllName = VirtualAllocEx(pi.hProcess,
		NULL,
		m_strDllPath.size(),
		MEM_COMMIT,
		PAGE_READWRITE);



	if (lpDllName)
	{
		//��DLL·��д��Ŀ����̿ռ�
		if (WriteProcessMemory(pi.hProcess, lpDllName, m_strDllPath.c_str(), m_strDllPath.size(), NULL))
		{
			LPVOID nLoadLibrary = (LPVOID)GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "LoadLibraryA");
			//��Զ��APC���в���LoadLibraryA
			if (!QueueUserAPC((PAPCFUNC)nLoadLibrary, pi.hThread, (DWORD)lpDllName))
			{
				MessageBoxW(NULL, L"QueueUserAPCʧ�ܣ���", L"Tip", MB_OK);
				return;
			}
		}
		else
		{
			MessageBoxW(NULL, L"WriteProcessMemoryʧ�ܣ���", L"Tip", MB_OK);
			return;
		}
	}

	//�ָ����߳�
	ResumeThread(pi.hThread);
	MessageBoxW(NULL, L"APCע��ɹ�����", L"Tip", MB_OK);
}