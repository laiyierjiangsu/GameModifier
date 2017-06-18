#pragma  once
#include <assert.h>
#define  False_Return(hr) \
if (!(hr))\
{\
	assert(false);\
	return false;\
}