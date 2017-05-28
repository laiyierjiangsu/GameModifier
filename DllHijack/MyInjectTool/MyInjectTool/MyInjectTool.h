
// MyInjectTool.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once
#include "PEFuncs.h"
#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif
#include <vector>
#include "resource.h"		// 主符号
using namespace std;

// CMyInjectToolApp: 
// 有关此类的实现，请参阅 MyInjectTool.cpp
//

class CMyInjectToolApp : public CWinApp
{
public:
	CMyInjectToolApp();

// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
public:
	vector<PIMAGE_SECTION_HEADER> vct_SectionHeader;//保存节表信息
	CFile* m_pFile;
	MAP_FILE_STRUCT m_stMapFile;
	PIMAGE_DOS_HEADER m_pDosHeader;
	PIMAGE_NT_HEADERS m_pNtHeader;
};

extern CMyInjectToolApp theApp;