// InputInject.cpp : 实现文件
//

#include "InputInject.h"
#include <Imm.h>
#pragma comment(lib,"IMM32.lib")

// InputInject 对话框





// InputInject 消息处理程序

typedef struct
{
	HWND hWnd;
	DWORD dwPid;
}WNDINFO;

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
	WNDINFO* pInfo = (WNDINFO*)lParam;
	DWORD dwProcessId = 0;
	GetWindowThreadProcessId(hWnd, &dwProcessId);

	if (dwProcessId == pInfo->dwPid)
	{
		pInfo->hWnd = hWnd;
		return FALSE;
	}
	return TRUE;
}

/*******************************************************
*函数功能:按照进程ID获取主窗口句柄
*函数参数:参数1：进程ID
*函数返回:HWND
*注意事项:无
*最后修改时间:2017/5/13
*******************************************************/
HWND GetHwndByProcessId(DWORD dwProcessId)
{
	WNDINFO info = { 0 };
	info.hWnd = NULL;
	info.dwPid = dwProcessId;
	EnumWindows(EnumWindowsProc, (LPARAM)&info);
	return info.hWnd;
}


void InputInject::OnBnClickedInject(std::string dllPath)
{
	m_strDllPath = dllPath;
	// TODO:  在此添加控件通知处理程序代码
	char ImeDllPath[MAX_PATH] = "E:\\InjectDll\\MyImeDll.dll";
	char InjectDllPath[MAX_PATH] = "E:\\InjectDll\\MyDll.dll";

	//原始输入法
	HKL HklOldInput = NULL;
	//新的输入法
	HKL HklNewInput = NULL;
	//输入法布局名称
	TCHAR ImeSymbol[MAX_PATH];
	//被注入进程的窗口句柄
	HWND hWnd = NULL;
	HWND hTopWnd = NULL;

	HKEY phkResult;
	int i = 0;
	TCHAR ValueName[MAX_PATH];  //存储得到的键值名
	TCHAR lpData[MAX_PATH];     //存储得到的键值数据
	DWORD lenValue = MAX_PATH;  //存储键值的数据长度
	DWORD lenData = MAX_PATH;   //存储键值的数据长度

	//1.保存原始的键盘布局，方便后面还原
	SystemParametersInfo(SPI_GETDEFAULTINPUTLANG, 0, &HklOldInput, 0);
	//2.将输入法DLL和待注入Dll一起放到C:\\WINDOWS\\SYSTEM32目录下
	CopyFileA(ImeDllPath, "C:\\WINDOWS\\SYSTEM32\\MyImeDll.ime", FALSE);
	CopyFileA(m_strDllPath.c_str(), "C:\\WINDOWS\\SYSTEM32\\InjectDll.dll", FALSE);

	//3.加载输入法
	HklNewInput = ImmInstallIMEA("C:\\WINDOWS\\SYSTEM32\\MyImeDll.ime", "我的输入法");

	//4.判断输入法句柄是否有效
	if (!ImmIsIME(HklNewInput))
	{
		TipBox("输入法加载失败!!!");
		return;
	}

	//5.激活新输入法的键盘布局
	ActivateKeyboardLayout(HklNewInput, 0);
	//6.获取被激活的输入法注册表项，方便后面删除
	GetKeyboardLayoutName(ImeSymbol);

	//7.获取串口句柄
	hWnd = GetHwndByProcessId(m_dwPID);

	//8.激活指定窗口的输入法
	if (HklNewInput != NULL)
	{
		::PostMessage(hWnd, WM_INPUTLANGCHANGEREQUEST, 0x1, (LPARAM)HklNewInput);
	}

	Sleep(1000);

	//9.还原键盘布局
	SystemParametersInfo(SPI_SETDEFAULTINPUTLANG, 0, &HklOldInput, SPIF_SENDWININICHANGE);

	//10.顶层窗口换回去，不换回去后面卸载不到，因为ime文件占用
	do
	{
		hTopWnd = ::FindWindowEx(NULL, hTopWnd, NULL, NULL);
		if (hTopWnd != NULL)
		{
			::PostMessage(hTopWnd, WM_INPUTLANGCHANGEREQUEST, 0x1, (LPARAM)&HklOldInput);
		}
	} while (hTopWnd == NULL);

	//11.卸载输入法
	if (HklNewInput != NULL)
	{
		UnloadKeyboardLayout(HklNewInput);
	}

	//12.删除注册表项，清理痕迹
	//打开注册表项目，获取句柄
	RegOpenKeyA(HKEY_CURRENT_USER, "Keyboard Layout\\Preload", &phkResult);
	//枚举注册表项目
	while (RegEnumValue(phkResult, i, ValueName, &lenValue, NULL, NULL, (LPBYTE)lpData, &lenData) != ERROR_NO_MORE_ITEMS)
	{
		if (lenData != 0)
		{
			if (wcscmp(ImeSymbol, lpData) == 0)
			{
				//删除项目数值
				RegDeleteValue(phkResult, ValueName);
				break;
			}
		}

		memset(lpData, 0, MAX_PATH);
		memset(ValueName, 0, MAX_PATH);
		lenValue = MAX_PATH;
		lenData = MAX_PATH;
		i++;
	}

	// 删除输入法文件
	DeleteFileA("C:\\WINDOWS\\SYSTEM32\\MyImeDll.ime");
}
