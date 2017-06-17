#include "TMemBase.h"
#include "Framework.h"
void TMemBase::Test()
{
	printf("Test mem base: %p\n", &pBuff);
	DWORD a = (DWORD)&pBuff;
	DWORD b = (DWORD)this;
	//assert(a == b);
	printf("The offset of private member:%d\n", (DWORD)&pBuff - (DWORD)this);
}

