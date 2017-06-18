#include "TMemBase.h"
#include "Framework.h"
static int iValueToBeModified = 1023;
void TMemBase::Test()
{
	//search a process's memory for a specified value.
	//Memory Scanner
	printf("Value to be modified value:%d , addr: %p\n", iValueToBeModified++, &iValueToBeModified);
#if 0
	printf("Test mem base: %p\n", &pBuff);
	DWORD a = (DWORD)&pBuff;
	DWORD b = (DWORD)this;
	//assert(a == b);
	printf("The offset of private member:%d\n", (DWORD)&pBuff - (DWORD)this);
#endif
}

