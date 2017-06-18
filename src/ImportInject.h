#pragma once
#include "..\stdafx.h"
#include <vector>
// ImportInject 对话框
#include "PEFuncs.h"
class ImportInject 
{
public:
	std::wstring m_strFile;
	std::string m_strDll;
	std::wstring m_strFun;
	std::wstring m_strTempPath;

	std::vector<PIMAGE_SECTION_HEADER> vct_SectionHeader;//保存节表信息
	MAP_FILE_STRUCT m_stMapFile;
	PIMAGE_DOS_HEADER m_pDosHeader;
	PIMAGE_NT_HEADERS m_pNtHeader;


	void OnBnClickedButton1(std::wstring peFile);
	void OnBnClickedButton2();
	std::vector<std::string> m_strFunList;
	void OnBnClickedButton3();
};
