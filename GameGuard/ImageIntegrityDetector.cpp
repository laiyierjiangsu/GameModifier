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
	typedef  void (* ExportFun)();
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
	printf("Original Crc read from file: %08x\n",OriginalCRC32);
	//得到第一个区块的起始地址
	pSectionHeader = IMAGE_FIRST_SECTION(pNTHeader);
    IMAGE_FILE_HEADER* pFileHeader =	&pNTHeader->FileHeader;
	for (int i = 0; i < pFileHeader->NumberOfSections;i++)
	{
		if((strcmp((char*)pSectionHeader->Name,".text") == 0))
		{
			break;
		}
		pSectionHeader++;
	}

	printf("ImageCodeSectionCrc32 first section name:%s\n",pSectionHeader->Name);
	BYTE* pBuffStart = (BYTE*)(ImageBase + pSectionHeader->VirtualAddress); 
	crc32 = Crc32_ComputeBuf(pBuffStart,pSectionHeader->Misc.VirtualSize);

	ExportFun proc = (ExportFun)GetProcAddress((HMODULE)ImageBase, "TestMessageBox");     
	assert(NULL != proc);
	(*proc)();

	assert(OriginalCRC32 == crc32);
	printf("Memory code crc32 value: %08x\n",crc32);


	return true;
}

void ImageIntegrityDetector::Detect()
{

	//CheckCodeSnipeCrc32();






	CheckImageIsModified();

}

void ImageIntegrityDetector::CheckImageIsModified()
{
	printf("Start checking image integrity!\n");
	DWORD PECrc32 = 0, ImageCrc32 = 0;
	assert(AddCodeSectionCrc2File(L"InjectedDll.dll"));
	HMODULE hModule = LoadLibraryA("InjectedDll.dll");
	assert(hModule != nullptr);
	assert(ImageCodeSectionCrc32("InjectedDll.dll", ImageCrc32));
	
	//assert(PEFileCodeSectionCrc32_2("InjectedDll.dll", PECrc32));
	//assert(PEFileCodeSectionCrc32("InjectedDll.dll", PECrc32));
	CalcImageSize("InjectedDll.dll");
	CalcImageSize("kernel32.dll");
	CalcImageSize("lpk.dll");
	
	printf("End checking image integrity!\n");
}

void ImageIntegrityDetector::CheckSelfIsModified()
{

}

void ImageIntegrityDetector::CalcImageSize(char* pDllname)
{
	PIMAGE_DOS_HEADER pDosHeader = nullptr;
	PIMAGE_NT_HEADERS pNTHeader = nullptr;
	PIMAGE_SECTION_HEADER pSectionHeader = nullptr;
	IMAGE_FILE_HEADER *pFileHeader;
	DWORD ImageBase;
	ImageBase = (DWORD)GetModuleHandleA(pDllname);
	pDosHeader=(PIMAGE_DOS_HEADER)ImageBase;	 	 
	pNTHeader=(PIMAGE_NT_HEADERS32)((DWORD)pDosHeader+pDosHeader->e_lfanew);
	pFileHeader = &pNTHeader->FileHeader;
	IMAGE_OPTIONAL_HEADER32* pOptionalFile = &pNTHeader->OptionalHeader;
	printf("begin size: %x ,  imageSize: %x,end size: %x,\n",
		ImageBase,pOptionalFile->SizeOfImage,ImageBase+pOptionalFile->SizeOfImage);

}

bool ImageIntegrityDetector::AddCodeSectionCrc2File(TCHAR*szFileName)
{
		PIMAGE_DOS_HEADER	    pDosHeader=NULL;
		PIMAGE_NT_HEADERS       pNtHeader=NULL;
		PIMAGE_SECTION_HEADER   pSecHeader=NULL;

		//TCHAR szFileName[MAX_PATH ] ;
		DWORD szTemp;
		static OPENFILENAME ofn ;
		HANDLE hFile;	
		long FileSize;
		DWORD szCRC32;
		TCHAR  *pBuffer ;

		ZeroMemory(&ofn, sizeof(ofn));                             // 初始化OPENFILENAME结构

		//static TCHAR szFilter[] =TEXT ("EXE Files (*.exe)\0*.exe\0") \
			//TEXT ("All Files (*.*)\0*.*\0\0") ;
		//TCHAR* szFileName =L"InjectedDll.dll";
		//szFileName[0] = '\0';                         
		ZeroMemory(&ofn, sizeof(ofn));                             // 初始化OPENFILENAME结构
		ofn.lStructSize       = sizeof (OPENFILENAME) ;
		ofn.lpstrFilter       = nullptr;
		ofn.lpstrFile         = szFileName ;         
		ofn.nMaxFile          = MAX_PATH ;   
		ofn.nMaxFileTitle     = MAX_PATH ;
		ofn.Flags             = OFN_HIDEREADONLY | OFN_CREATEPROMPT ;           

		//if(GetOpenFileName (&ofn))
		//{

			hFile = CreateFile(
				szFileName,
				GENERIC_READ | GENERIC_WRITE, 
				FILE_SHARE_READ, 
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL,
				NULL);


			if( hFile != INVALID_HANDLE_VALUE )
			{
				FileSize=GetFileSize(hFile,&szTemp);  // 获取文件大小
				if (FileSize == 0xFFFFFFFF) return FALSE;
				pBuffer = new TCHAR [FileSize];     // 申请内存
				if(ReadFile(hFile, pBuffer, FileSize, &szTemp, NULL)==NULL) return FALSE;// 读取数据

			}
			else
			{
				printf("I can't access file!");
				return false;
			}


		pDosHeader=(PIMAGE_DOS_HEADER)pBuffer;	 	 
		pNtHeader=(PIMAGE_NT_HEADERS32)((DWORD)pDosHeader+pDosHeader->e_lfanew);
		IMAGE_FILE_HEADER *pFileHeader = &pNtHeader->FileHeader;
		pSecHeader=IMAGE_FIRST_SECTION(pNtHeader);   //得到第一个区块的起始地址
		for(int i = 0 ; i < pFileHeader->NumberOfSections;i++)
		{
			if((strcmp((char*)pSecHeader->Name,".text") == 0))
			{
				break;
			}
			pSecHeader++;
		}
		BYTE* pBuffStart = (BYTE*)(pBuffer+pSecHeader->PointerToRawData);
		szCRC32=Crc32_ComputeBuf(pBuffStart,pSecHeader->Misc.VirtualSize);

		DWORD Writeadd=DWORD(pDosHeader->e_lfanew-4);//定位到PE文件头（即字串“PE\0\0”处）前4个字节处，准备在这写CRC-32值：
		SetFilePointer(hFile,Writeadd,NULL,FILE_BEGIN);//设置文件指针
		printf("Original Crc: %08x\n",szCRC32);

		if(!WriteFile(hFile,&szCRC32,4,&szTemp,NULL))
		{
			printf("Error while patching !");
			CloseHandle(hFile);
			return 0;
		}


		delete pBuffer;  // 释放内存
		CloseHandle(hFile);	
		printf("Patch successfull !\n");
		return 1;
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
			printf("Start detect code snip!");
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
		
		printf("End detect code snip!");
}

