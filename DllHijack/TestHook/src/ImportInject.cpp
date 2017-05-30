// ImportInject.cpp : 实现文件
//

#include "ImportInject.h"
#include "PEFuncs.h"
#include <IMAGEHLP.H>
// ImportInject 对话框



// ImportInject 消息处理程序

/*******************************************************
*函数功能:计算内存对齐或者文件对齐后的大小
*函数参数:参数1：实际大小，参数2：对齐值
*函数返回:DWORD
*注意事项:无
*最后修改时间:2017/5/13
*******************************************************/
DWORD ClacAlignment(DWORD dwSize, DWORD dwAlign)
{
	if (dwSize % dwAlign != 0)
	{
		return (dwSize / dwAlign + 1)*dwAlign;
	}
	else
	{
		return dwSize;
	}
}

void ImportInject::OnBnClickedButton1(std::wstring peFile)
{
	m_strFile = peFile;
	// TODO:  在此添加控件通知处理程序代码
	BOOL bRet = FALSE;
	char szExePath[MAX_PATH] = { 0 };
	char *szExe = "Temp.exe";
	GetModuleFileNameA(NULL, szExePath, MAX_PATH);
	(strrchr(szExePath, '\\'))[1] = 0;
	strcat(szExePath, szExe);

	m_strTempPath = string2Wstring(szExePath);
	

	//复制一份文件用于修改，源文件保留。
	bRet = ::CopyFile(m_strFile.c_str(), m_strTempPath.c_str(), FALSE);

	if (bRet == 0)
	{
		TipBox("复制文件失败");
	}

	std::string filePath = wstring2string(m_strFile);
	LoadFileR((LPTSTR)filePath.c_str(), &m_stMapFile);



	if (!IsPEFile(m_stMapFile.ImageBase))
	{
		::MessageBoxA(nullptr, "不是有效的PE文件", "不是有效的PE文件", MB_OK);
		UnLoadFile(&m_stMapFile);
		//EnableEditCtrl(hWnd, FALSE);
		return;
	}



}


void ImportInject::OnBnClickedButton2()
{

	if (m_strFun.empty())
	{
		TipBox("请输入DLL函数名");
		return;
	}
	static int nIndex = 0;
	//m_strFunList.InsertItem(nIndex, m_strFun);
	//m_strFun.Empty();
	nIndex++;
}


void ImportInject::OnBnClickedButton3()
{
	// TODO:  在此添加控件通知处理程序代码
	// TODO: Add your control notification handler code here
	FILE* fp;
	//最后一个节
	PIMAGE_SECTION_HEADER lpImgLastSection;
	//要添加的区块
	IMAGE_SECTION_HEADER ImgNewSection;
	//第一个节头
	PIMAGE_SECTION_HEADER lpFirstSectionHeader;
	//打开源文件修改。
	PIMAGE_NT_HEADERS lpNtHeader = new IMAGE_NT_HEADERS;
	PIMAGE_NT_HEADERS lpNewNtHeader = new IMAGE_NT_HEADERS;
	//节的数目
	int nSectionNum = 0;
	//新节的RVA
	DWORD dwNewSectionRVA, dwNewImportRva;
	//新节的文件偏移
	DWORD dwNewFA = 0;
	//节对齐
	int nSectionAlignment = 0;
	//文件对齐
	int nFileAlignment = 0;
	//DLL名称的长度
	int nDllLen = 0;
	//需要写入的函数数目
	int nFunNum = m_strFunList.size();
	//相对于新节的文件偏移
	DWORD dwNewOffset = 0;
	//要添加的节表头
	//IMAGE_SECTION_HEADER ImgNewSection;
	PIMAGE_IMPORT_DESCRIPTOR lpImport, lpNewImport;
	//原来导入表的大小，和新导入表的大小
	DWORD dwImportSize, dwNewImportSize;
	//计算新节头的文件偏移
	DWORD dwNewSectionOffset;

	std::string filePath = wstring2string(m_strTempPath);
	fp = ::fopen(filePath.c_str(), "rb+");
	if (fp == NULL)
	{
		::DeleteFileA(filePath.c_str());
		TipBox("打开临时文件失败！！");
		return;
	}

	lpFirstSectionHeader = GetFirstSectionHeader(m_stMapFile.ImageBase);
	lpNtHeader = GetNtHeaders(m_stMapFile.ImageBase);
	nSectionNum = lpNtHeader->FileHeader.NumberOfSections;
	nSectionAlignment = lpNtHeader->OptionalHeader.SectionAlignment;
	nFileAlignment = lpNtHeader->OptionalHeader.FileAlignment;

	//获取导入表的指针
	lpImport = (PIMAGE_IMPORT_DESCRIPTOR)ImageDirectoryEntryToData(m_stMapFile.ImageBase, FALSE, IMAGE_DIRECTORY_ENTRY_IMPORT, &dwImportSize);
	//计算新的导入表的大小:旧的导入表大小 + 新的导入表大小
	dwNewImportSize = dwImportSize + sizeof(IMAGE_IMPORT_DESCRIPTOR);
	//获取最后一个节头
	lpImgLastSection = lpFirstSectionHeader + (nSectionNum - 1);
	//获取新节的RVA
	dwNewSectionRVA = lpImgLastSection->VirtualAddress
		+ ClacAlignment(lpImgLastSection->Misc.VirtualSize, nSectionAlignment);
	//计算新的文件偏移
	dwNewFA = lpImgLastSection->PointerToRawData
		+ ClacAlignment(lpImgLastSection->SizeOfRawData, nFileAlignment);

	//1.在复制的文件中写入DLL名
	fseek(fp, dwNewFA, SEEK_SET);
	dwNewOffset = m_strDll.length() + 1;
	fwrite(m_strDll.c_str(), dwNewOffset, 1, fp);

	DWORD *arrINTRva = new DWORD[nFunNum + 1];
	memset(arrINTRva, 0, sizeof(DWORD)*(nFunNum + 1));

	//2.写入所有的的IMAGE_IMPORT_BY_NAME结构,也就是写入所有函数名
	for (int i = 0; i < nFunNum; i++)
	{
		DWORD dwTempRva = 0;

		static int nFunLen = 0;
		PIMAGE_IMPORT_BY_NAME pImportFun = new IMAGE_IMPORT_BY_NAME;
		pImportFun->Hint = i;
		std::string strFunName = m_strFunList[i];
		fseek(fp, dwNewFA + dwNewOffset, SEEK_SET);
		//计算IMAGE_IMPORT_BY_NAME的RVA存入数组
		dwTempRva = dwNewSectionRVA + dwNewOffset;
		arrINTRva[i] = dwTempRva;
		dwNewOffset = dwNewOffset + strFunName.length() + 1 + sizeof(WORD);
		memcpy(pImportFun->Name, strFunName.c_str(), strFunName.length() + 1);
		fwrite(pImportFun, strFunName.length() + 1 + sizeof(WORD), 1, fp);


	}
	DWORD dwINTRVA = dwNewSectionRVA + dwNewOffset;

	//3.写入所有的的INT结构
	for (int i = 0; i < nFunNum + 1; i++)
	{

		fseek(fp, dwNewFA + dwNewOffset, SEEK_SET);
		dwNewOffset += sizeof(DWORD);
		//末尾填充0结构体
		fwrite(&arrINTRva[i], sizeof(DWORD), 1, fp);
	}

	//4.申请新空间存放旧的的IID和新的IID
	lpNewImport = (PIMAGE_IMPORT_DESCRIPTOR)malloc(dwNewImportSize);
	memset(lpNewImport, 0, dwNewImportSize);
	memcpy(lpNewImport, lpImport, dwImportSize);

	int i = 0;
	while (1)
	{
		if (lpNewImport[i].OriginalFirstThunk == 0 && lpNewImport[i].TimeDateStamp == 0 &&
			lpNewImport[i].ForwarderChain == 0 && lpNewImport[i].Name == 0 && lpNewImport[i].FirstThunk == 0)
		{
			lpNewImport[i].Name = dwNewSectionRVA;
			lpNewImport[i].TimeDateStamp = 0;
			lpNewImport[i].ForwarderChain = 0;
			lpNewImport[i].FirstThunk = dwINTRVA;
			lpNewImport[i].OriginalFirstThunk = dwINTRVA;
			break;
		}
		else i++;
	}
	//计算新的导入表RVA
	dwNewImportRva = dwNewSectionRVA + dwNewOffset;
	//写入所有的导入表项
	fseek(fp, dwNewFA + dwNewOffset, SEEK_SET);
	fwrite(lpNewImport, dwNewImportSize, 1, fp);
	dwNewOffset += dwNewImportSize;

	//计算文件对齐需要补零的值
	DWORD dwFileAlign = ClacAlignment(dwNewOffset, nFileAlignment) - dwNewOffset;

	for (size_t i = 0; i < dwFileAlign; i++)
	{
		fputc('\0', fp);
	}


	//5.添加一个新节表头项
	memset(&ImgNewSection, 0, sizeof(IMAGE_SECTION_HEADER));
	//添加名为.newsec的新节
	strcpy((char*)ImgNewSection.Name, ".newsec");
	ImgNewSection.VirtualAddress = dwNewSectionRVA;
	ImgNewSection.PointerToRawData = dwNewFA;
	ImgNewSection.Misc.VirtualSize = ClacAlignment(dwNewOffset, nSectionAlignment);
	ImgNewSection.SizeOfRawData = ClacAlignment(dwNewOffset, nFileAlignment);
	ImgNewSection.Characteristics = 0xC0000040;

	//计算新节头的文件偏移
	dwNewSectionOffset = (DWORD)lpFirstSectionHeader -
		(DWORD)m_stMapFile.ImageBase + sizeof(IMAGE_SECTION_HEADER)*nSectionNum;
	fseek(fp, dwNewSectionOffset, 0);

	//写入节表头
	fwrite(&ImgNewSection, sizeof(IMAGE_SECTION_HEADER), 1, fp);
	memcpy(&ImgNewSection, lpFirstSectionHeader, sizeof(IMAGE_SECTION_HEADER));
	fseek(fp, (DWORD)lpFirstSectionHeader - (DWORD)m_stMapFile.ImageBase, SEEK_SET);
	fwrite(&ImgNewSection, sizeof(IMAGE_SECTION_HEADER), 1, fp);

	//6.更新NT头数据
	memcpy(lpNewNtHeader, lpNtHeader, sizeof(IMAGE_NT_HEADERS));
	int nNewImageSize = lpNtHeader->OptionalHeader.SizeOfImage + ClacAlignment(dwNewOffset, nSectionAlignment);
	lpNewNtHeader->OptionalHeader.SizeOfImage = nNewImageSize;
	lpNewNtHeader->OptionalHeader.DataDirectory[11].Size = 0;
	lpNewNtHeader->OptionalHeader.DataDirectory[11].VirtualAddress = 0;
	lpNewNtHeader->OptionalHeader.DataDirectory[12].Size = 0;
	lpNewNtHeader->OptionalHeader.DataDirectory[12].VirtualAddress = 0;
	lpNewNtHeader->FileHeader.NumberOfSections = nSectionNum + 1;
	lpNewNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress = dwNewImportRva;
	lpNewNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size = dwNewImportSize;

	//写入新的NT头
	fseek(fp, (DWORD)(lpNtHeader)-(DWORD)m_stMapFile.ImageBase, SEEK_SET);
	fwrite(lpNewNtHeader, sizeof(IMAGE_NT_HEADERS), 1, fp);

	if (fp != NULL)
	{
		fclose(fp);
	}
	UnLoadFile(&m_stMapFile);

	//释放扫尾工作

	if (arrINTRva != NULL)
	{
		delete[] arrINTRva;
		arrINTRva = NULL;
	}
}
