#include "ImageIntegrityDetector.h"
#include "ThirdParty/Decoder/crc32.h"
#include "Framework.h"
#include "PEFuncs.h"
#include "Utils.h"
ImageIntegrityDetector::ImageIntegrityDetector()
{
}

ImageIntegrityDetector::~ImageIntegrityDetector()
{
}

bool ImageIntegrityDetector::ImageCodeSectionCrc32(char* pModuleName, DWORD &crc32)
{
	PIMAGE_DOS_HEADER pDosHeader = nullptr;
	PIMAGE_NT_HEADERS pNTHeader = nullptr;
	PIMAGE_SECTION_HEADER pSectionHeader = nullptr;
	DWORD ImageBase, OriginalCRC32;
	ImageBase = (DWORD)GetModuleHandleA(pModuleName);
	pDosHeader = (PIMAGE_DOS_HEADER)ImageBase;
	pNTHeader = (PIMAGE_NT_HEADERS32)((DWORD)pDosHeader + pDosHeader->e_lfanew);
	//定位到PE文件头（及字符串“PE\0\0”处）前四个字节处，并读出存储在这里的CRC-32值
	OriginalCRC32 = *((DWORD*)((DWORD)pNTHeader - 4));
	//得到第一个区块的起始地址
	pSectionHeader = IMAGE_FIRST_SECTION(pNTHeader);
	crc32 = Crc32_ComputeBuf((BYTE*)(ImageBase + pSectionHeader->VirtualAddress),
					 pSectionHeader->Misc.VirtualSize);

	BYTE* pStart = (BYTE*)(ImageBase + pSectionHeader->VirtualAddress);
	DWORD Test = Crc32_ComputeBuf((BYTE*)(ImageBase + pSectionHeader->VirtualAddress),
		pSectionHeader->Misc.VirtualSize);
	assert(crc32 == Test);

	return true;
}

void ImageIntegrityDetector::Detect()
{
	CheckCodeSnipeCrc32();
	DWORD PECrc32 = 0, ImageCrc32 = 0;
	HMODULE hModule = LoadLibraryA("InjectedDll.dll");
	assert(hModule != nullptr);
	assert(ImageCodeSectionCrc32("InjectedDll.dll", ImageCrc32));
	assert(PEFileCodeSectionCrc32_2("InjectedDll.dll", PECrc32));
	assert(PEFileCodeSectionCrc32("InjectedDll.dll", PECrc32));
	assert(ImageCrc32 == PECrc32);
}

bool ImageIntegrityDetector::PEFileCodeSectionCrc32(char* pFile, DWORD& crc32)
{
	HANDLE hFile = CreateFileA(pFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	False_Return(INVALID_HANDLE_VALUE != hFile);
	//读取DOS文件头
	_IMAGE_DOS_HEADER dosHeader = { 0 };
	//首先移动到文件头
	DWORD num;
	int size = GetFileSize(hFile, NULL);
	BYTE* pBuff = new BYTE[size];
	::SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	False_Return(ReadFile(hFile, pBuff, size, &num, NULL));
	False_Return(num == size);


	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pBuff;
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS32)((DWORD)pDosHeader + pDosHeader->e_lfanew);
	DWORD ImageBase = (DWORD)pBuff;
	False_Return(pNTHeader->Signature == 0x00004550);

	PIMAGE_SECTION_HEADER pSectionHeader = IMAGE_FIRST_SECTION(pNTHeader);
	crc32 = Crc32_ComputeBuf((BYTE*)(ImageBase + pSectionHeader->VirtualAddress),
		pSectionHeader->Misc.VirtualSize);
	BYTE* pStart = (BYTE*)(ImageBase + pSectionHeader->VirtualAddress);
	::CloseHandle(hFile);
	delete[]pBuff;
	return true;
}

bool ImageIntegrityDetector::PEFileCodeSectionCrc32_2(char* pFile , DWORD& crc32)
{
	MAP_FILE_STRUCT stFileMap;

	BOOL bRet = LoadFileR((LPTSTR)Utf82WideChar(pFile).c_str(),&stFileMap);
	assert(bRet ==TRUE);
	PIMAGE_SECTION_HEADER pSectionHeader = GetFirstSectionHeader(stFileMap.ImageBase);
	crc32 = Crc32_ComputeBuf((BYTE*)((BYTE*)stFileMap.ImageBase + pSectionHeader->VirtualAddress),
		pSectionHeader->Misc.VirtualSize);
	//BYTE* pStart = (BYTE*)(ImageBase + pSectionHeader->VirtualAddress);

	UnLoadFile(&stFileMap);
	return bRet;

}

bool ImageIntegrityDetector::CheckCodeSnipeCrc32()
{
	DWORD addr1, addr2, size;
	_asm mov addr1, offset codeBegin;
	_asm mov addr2, offset codeEnd;
	codeBegin:
		//OutputDebugString(L"test");
		//OutputDebugString(L"test0");
		//OutputDebugString(L"test1");
		int a = 0;
		a = a + 1;
		a = a - 1;
	codeEnd:
		size = addr2 - addr1;
		DWORD curcrc32 = Crc32_ComputeBuf((void*)addr1, size);
		DWORD oldCrc32 = 0xbcf07446;
		assert(oldCrc32 == curcrc32);
}

