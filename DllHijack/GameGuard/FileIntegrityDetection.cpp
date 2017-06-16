#include "FileIntegrityDetection.h"
#include "Decoder/crc32.h"
#include "Framework.h"
FileIntegrityDetector::FileIntegrityDetector()
{
}

FileIntegrityDetector::~FileIntegrityDetector()
{
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
	CheckFileSum();
}
