// Hijack.cpp : 实现文件
//
#include "Hijack.h"


CHAR *g_pszHeader = "////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\r\n"
"// 头文件\r\n"
"#include <windows.h>\r\n"
"////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\r\n\r\n";


CHAR *g_pszHeader2 = "////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\r\n\r\n\r\n\r\n\r\n"
"////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\r\n"
"// 宏定义\r\n"
"#define NAKED __declspec(naked)\r\n"
"#define EXPORT __declspec(dllexport)\r\n"
"#define MYEXPORTFUN EXPORT NAKED\r\n"
"////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\r\n\r\n\r\n\r\n\r\n"
"////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\r\n"
"//全局变量\r\n"
"HMODULE hDll = NULL;\r\n"
"DWORD dwRetaddress[%d];							//存放返回地址\r\n"
"////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\r\n\r\n\r\n\r\n\r\n"
"////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\r\n"
"// 内部函数 获取真实函数地址\r\n"
"FARPROC WINAPI GetAddress(PCSTR pszProcName)\r\n"
"{\r\n"
"\t\tFARPROC fpAddress;\r\n"
"\t\tCHAR szTemp[MAX_PATH] = { 0 };\r\n"
"\t\tfpAddress = GetProcAddress(hDll, pszProcName);\r\n"
"\t\tif (fpAddress == NULL)\r\n"
"\t\t{\r\n"
"\t\t\t\twsprintf(szTemp,\"无法找到函数 :%s 的地址 \", pszProcName);\r\n"
"\t\t\t\tMessageBoxA(NULL, szTemp, \"错误\", MB_OK);\r\n"
"\t\t\t\tExitProcess(-2);\r\n"
"\t\t}\r\n"
"\t\t//返回真实地址\r\n"
"\t\treturn fpAddress;\r\n"
"}\r\n"
"////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\r\n\r\n\r\n"
"////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\r\n"
"// DLL MAIN\r\n"
"BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, PVOID pvReserved)\r\n"
"{\r\n"
"\t\tswitch (fdwReason)\r\n"
"\t\t{\r\n"
"\t\tcase DLL_PROCESS_ATTACH:\r\n"
"\t\t\t\thDll = LoadLibrary(\"修改为要就劫持的DLL\");\r\n"
"\t\t\t\tif (!hDll)\r\n"
"\t\t\t\t{\r\n"
"\t\t\t\t\tMessageBox(\"需要劫持的DLL不存在，注意改名\");\r\n"
"\t\t\t\t\treturn FALSE;\r\n"
"\t\t\t\t}\r\n"
"\t\t\t\tMessageBox(\"附加成功\");\r\n"
"\t\t\tbreak;\r\n"
"\t\tcase DLL_PROCESS_DETACH:\r\n"
"\t\t\t\tif (hDll != NULL)\r\n"
"\t\t\t\t\tFreeLibrary(hDll); \r\n"
"\t\t\tbreak; \r\n"
"\t\t}\r\n"
"\t\treturn TRUE;\r\n"

"}\r\n"
"////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\r\n\r\n\r\n\r\n";


CHAR *g_pszFun = "////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\r\n"
"//导出函数 %d  需要定义的变量请定义为全局，否则会有堆栈平衡问题\r\n"
"MYEXPORTFUN My_%s()\r\n"
"{\r\n"
"\t\tGetAddress(\"%s\");\r\n"
"\t\t//此时栈顶保持的是返回地址\r\n"
"\t\t__asm pop dwRetaddress[%d]				//保存原函数地址\r\n"
"\t\t//调用原函数\r\n"
"\t\t__asm call eax\r\n"
"\t\t__asm jmp dword ptr dwRetaddress[%d]			//跳回原函数\r\n"
"}\r\n"
"////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\r\n\r\n\r\n\r\n\r\n";
// Hijack 对话框





// Hijack 消息处理程序
CHAR * g_pszMyCode = "D:\\MyDll.c";
void Hijack::StartHijack()
{
	// TODO:  在此添加控件通知处理程序代码
	HANDLE hCFile = NULL;
	DWORD dwWritten = 0;
	CHAR szTemp[10000] = { 0 };

	hCFile = CreateFile(g_pszMyCode, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hCFile == INVALID_HANDLE_VALUE)
	{
		TipBox("创建文件失败");
		return;
	}

	//写入头部
	if (!WriteFile(hCFile, g_pszHeader, strlen(g_pszHeader), &dwWritten, NULL))
	{
		TipBox("写入文件失败");
		CloseHandle(hCFile);
		return;
	}

	//写入#pragma comment部分
	// 输出所有元素
	DWORD dwIndex = 1;
	for (POSITION pos = m_FunNameList.GetHeadPosition(); pos != NULL;)
	{
		ZeroMemory(szTemp, 10000);
		CString str = m_FunNameList.GetNext(pos);
		sprintf(szTemp, "#pragma comment(linker, \"/EXPORT:%s=_My_%s,@%d\")\r\n", str.GetBuffer(0), str.GetBuffer(0), dwIndex);
		dwIndex++;

		//写入#pragma comment部分
		if (!WriteFile(hCFile, szTemp, strlen(szTemp), &dwWritten, NULL))
		{
			MessageBox("写入文件失败");
			CloseHandle(hCFile);
			return;
		}
	}


	ZeroMemory(szTemp, 10000);
	sprintf(szTemp, g_pszHeader2, dwIndex, "%s");
	dwWritten = 0;
	//继续写入
	if (!WriteFile(hCFile, szTemp, strlen(szTemp), &dwWritten, NULL))
	{
		TipBox("写入文件失败");
		CloseHandle(hCFile);
		return;
	}

	DWORD dwCount = 1;
	for (POSITION pos = m_FunNameList.GetHeadPosition(); pos != NULL;)
	{
		CString str = m_FunNameList.GetNext(pos);
		ZeroMemory(szTemp, 10000);
		sprintf(szTemp, g_pszFun, dwCount, str.GetBuffer(0), str.GetBuffer(0), dwCount, dwCount);


		//写入#pragma comment部分
		if (!WriteFile(hCFile, szTemp, strlen(g_pszFun), &dwWritten, NULL))
		{
			TipBox("写入文件失败");
			CloseHandle(hCFile);
			return;
		}
		dwCount++;
	}

	CloseHandle(hCFile);
	ZeroMemory(szTemp, 10000);
	sprintf(szTemp, "源码生成成功：所在路径为%s", g_pszMyCode);
	TipBox(szTemp);
}


void Hijack::OnDropFiles(HDROP hDropInfo)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	m_mylist.DeleteAllItems();
	if (m_pstMapFile != NULL)
	{
		if (m_bIsFirst == FALSE)
		{
			UnLoadFile(m_pstMapFile);
		}
		else
		{
			m_bIsFirst = FALSE;
		}

	}
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	int DropCount = DragQueryFile(hDropInfo, -1, NULL, 0);//取得被拖动文件的数目  
	for (int i = 0; i< DropCount; i++)
	{
		CHAR szStr[MAX_PATH];
		DragQueryFile(hDropInfo, i, szStr, MAX_PATH);//获得拖曳的第i个文件的文件名 

		LoadFileR(szStr, m_pstMapFile);
		if (IsPEFile(m_pstMapFile->ImageBase) == FALSE)
		{
			UnLoadFile(m_pstMapFile);
			MessageBox("不是有效的PE文件");
			return;
		}
	}
	DragFinish(hDropInfo);  //拖放结束后,释放内存  

	GetAndShowExports();
	CDialogEx::OnDropFiles(hDropInfo);
}

/*******************************************************
*函数功能:从DLL中获取并且拿到导入表
*函数参数:无
*函数返回:无
*注意事项:无
*最后修改时间:2017/5/16
*******************************************************/
BOOL Hijack::GetAndShowExports()
{
	CHAR         cBuff[10];
	CHAR		 *szFuncName = NULL;
	UINT                    iNumOfName = 0;
	PDWORD                  pdwRvas, pdwNames;
	PWORD                   pwOrds;
	UINT                    i = 0, j = 0, k = 0;
	BOOL                    bIsByName = FALSE;;


	PIMAGE_NT_HEADERS       pNtH = NULL;
	PIMAGE_EXPORT_DIRECTORY pExportDir = NULL;

	pNtH = GetNtHeaders(m_pstMapFile->ImageBase);
	if (!pNtH)
		return FALSE;
	pExportDir = (PIMAGE_EXPORT_DIRECTORY)GetExportDirectory(m_pstMapFile->ImageBase);
	if (!pExportDir)
		return FALSE;


	pwOrds = (PWORD)MyRvaToPtr(pNtH, m_pstMapFile->ImageBase, pExportDir->AddressOfNameOrdinals);
	pdwRvas = (PDWORD)MyRvaToPtr(pNtH, m_pstMapFile->ImageBase, pExportDir->AddressOfFunctions);
	pdwNames = (PDWORD)MyRvaToPtr(pNtH, m_pstMapFile->ImageBase, pExportDir->AddressOfNames);

	if (!pdwRvas)
		return FALSE;


	iNumOfName = pExportDir->NumberOfNames;

	for (i = 0; i<pExportDir->NumberOfFunctions; i++)
	{
		if (*pdwRvas)
		{
			for (j = 0; j<iNumOfName; j++)
			{
				if (i == pwOrds[j])
				{
					bIsByName = TRUE;
					szFuncName = (char*)MyRvaToPtr(pNtH, m_pstMapFile->ImageBase, pdwNames[j]);
					break;
				}

				bIsByName = FALSE;
			}

			//show funcs to listctrl


			wsprintf(cBuff, "%04lX", (UINT)(pExportDir->Base + i));

			m_mylist.InsertItem(k, cBuff);

			wsprintf(cBuff, "%08lX", (*pdwRvas));
			m_mylist.SetItemText(k, 1, cBuff);

			if (bIsByName)
			{
				m_FunNameList.AddTail(szFuncName);
				m_mylist.SetItemText(k, 2, szFuncName);
				szFuncName == NULL;
			}
			else
			{
				m_mylist.SetItemText(k, 2, "-");
			}

			//
			++k;

		}

		++pdwRvas;

	}
	return TRUE;
}


BOOL Hijack::OnInitDialog()
{


	// TODO:  在此添加额外的初始化
	m_mylist.InsertColumn(0, "Ordinal", LVCFMT_LEFT, 200);
	m_mylist.InsertColumn(1, "Rva", LVCFMT_LEFT, 200);
	m_mylist.InsertColumn(2, "Function Name", LVCFMT_LEFT, 200);
	m_pstMapFile = new MAP_FILE_STRUCT;
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}
