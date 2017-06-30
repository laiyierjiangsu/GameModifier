#include "FileIntegrityDetection.h"
#include "ThirdParty/Decoder/crc32.h"
#include "Framework.h"
FileIntegrityDetector::FileIntegrityDetector()
{
}

FileIntegrityDetector::~FileIntegrityDetector()
{
}
void FileIntegrityDetector::CheckFileModified()
{
	AddCrc322File();
	assert(IsFileModified());
}

bool FileIntegrityDetector::CheckFileSum()
{
	FILE* pFile = fopen("InjectDll.dll", "r");
	False_Return(pFile != nullptr);
	unsigned long crc32;
	False_Return(Crc32_ComputeFile(pFile, &crc32) == 0);
	fclose(pFile);

	pFile = fopen("InjectDll2.dll", "r");
	False_Return(pFile != nullptr);
	unsigned long crc322;
	False_Return(Crc32_ComputeFile(pFile, &crc322) == 0);
	fclose(pFile);

	assert(crc322 == crc32);
	return true;

}

void FileIntegrityDetector::Detect()
{
//	CheckFileSum();
	CheckFileModified();
}
bool FileIntegrityDetector::AddCrc322File()
{
	PIMAGE_DOS_HEADER	    pDosHeader=NULL;
	PIMAGE_NT_HEADERS       pNtHeader=NULL;
	PIMAGE_SECTION_HEADER   pSecHeader=NULL;


	TCHAR szFileName[MAX_PATH ] ;
	static OPENFILENAME ofn ;
	HANDLE hFile;	
	DWORD szTemp,fileSize,szCRC32;
	TCHAR  *pBuffer ;

	static TCHAR szFilter[] =TEXT ("*.dll");

	szFileName[0] = '\0';                         
	ZeroMemory(&ofn, sizeof(ofn));                             // 初始化OPENFILENAME结构
	ofn.lStructSize       = sizeof (OPENFILENAME) ;
	ofn.lpstrFilter       = szFilter ;
	ofn.lpstrFile         = szFileName ;         
	ofn.nMaxFile          = MAX_PATH ;   
	ofn.nMaxFileTitle     = MAX_PATH ;
	ofn.Flags             = OFN_HIDEREADONLY | OFN_CREATEPROMPT ;           

	if(GetOpenFileName (&ofn))
	{

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
			fileSize=GetFileSize(hFile,&szTemp);  // 获取文件大小
			if (fileSize == 0xFFFFFFFF) return FALSE;
			pBuffer = new TCHAR [fileSize];     // 申请内存
			if(ReadFile(hFile, pBuffer, fileSize, &szTemp, NULL)==NULL) return FALSE;// 读取数据
		}
		else
		{
			printf("I can't access file!");
			return 0;
		}

	}
	else
		return 0;


	pDosHeader=(PIMAGE_DOS_HEADER)pBuffer;	 	 
	pNtHeader=(PIMAGE_NT_HEADERS32)((DWORD)pDosHeader+pDosHeader->e_lfanew);

	fileSize=fileSize-DWORD(pDosHeader->e_lfanew);//将PE文件头前那部分数据去除

	szCRC32=Crc32_ComputeBuf((BYTE*)(pBuffer+pDosHeader->e_lfanew),fileSize);

	//读3ch处的PE文件头指针：
	DWORD Writeadd=DWORD(pDosHeader->e_lfanew-4);//定位到PE文件头（即字串“PE\0\0”处）前4个字节处，准备在这写CRC-32值：

	SetFilePointer(hFile,Writeadd,NULL,FILE_BEGIN);//设置文件指针

	if(!WriteFile(hFile,&szCRC32,4,&szTemp,NULL))
	{
		printf("Error while patching !");
		CloseHandle(hFile);
		return 0;
	}


	CloseHandle(hFile);
	printf("Patch successfull !");
}

bool FileIntegrityDetector::IsFileModified()
{

	PIMAGE_DOS_HEADER	    pDosHeader=NULL;
	PIMAGE_NT_HEADERS       pNtHeader=NULL;
	PIMAGE_SECTION_HEADER   pSecHeader=NULL;

	DWORD fileSize,OriginalCRC32,NumberOfBytesRW;
	//TCHAR  *pBuffer ,szFileName[MAX_PATH]; 
	TCHAR* szFileName = L"InjectedDll.dll";

	//获得文件名
	//	GetModuleFileName(NULL,szFileName,MAX_PATH);
	//打开文件
	HANDLE hFile = CreateFile(
		szFileName,
		GENERIC_READ,
		FILE_SHARE_READ, 
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if ( hFile == INVALID_HANDLE_VALUE ) return FALSE;


	//获得文件长度 :
	fileSize = GetFileSize(hFile,NULL);
	if (fileSize == 0xFFFFFFFF) return FALSE;

	TCHAR* pBuffer = new TCHAR [fileSize];     // 申请内存,也可用VirtualAlloc等函数申请内存
	ReadFile(hFile,pBuffer, fileSize, &NumberOfBytesRW, NULL);//读取文件内容
	CloseHandle(hFile);  //关闭文件


	pDosHeader=(PIMAGE_DOS_HEADER)pBuffer;
	pNtHeader=(PIMAGE_NT_HEADERS32)((DWORD)pDosHeader+pDosHeader->e_lfanew);
	//定位到PE文件头（即字串“PE\0\0”处）前4个字节处，并读出储存在这里的CRC-32值：
	OriginalCRC32 =*((DWORD *)((DWORD)pNtHeader-4));	 	 

	fileSize=fileSize-DWORD(pDosHeader->e_lfanew);//将PE文件头前那部分数据去除
	unsigned long iCurCrc = Crc32_ComputeBuf((BYTE*)(pBuffer+pDosHeader->e_lfanew),fileSize);
	if (iCurCrc == OriginalCRC32 )
		return TRUE;
	else
		return FALSE;
}