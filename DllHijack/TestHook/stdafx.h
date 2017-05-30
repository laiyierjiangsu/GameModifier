// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include <windows.h>
#include <string>
#include <list>
#include <map>
// TODO: reference additional headers your program requires here
void TipBox(std::wstring str);
void TipBox(std::string str);
std::wstring string2Wstring(std::string sToMatch);
//把宽字符串转换成字符串，输出使用
std::string wstring2string(std::wstring sToMatch);