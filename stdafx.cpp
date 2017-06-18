// stdafx.cpp : source file that includes just the standard includes
// TestHook.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file
void TipBox(std::wstring str)
{
	MessageBox(NULL, str.c_str(), L"Tip", MB_OK);
}

void TipBox(std::string str)
{
	MessageBoxA(NULL, str.c_str(), "Tip", MB_OK);
}

//把字符串转换成宽字符串
std::wstring string2Wstring(std::string sToMatch)
{
	int iWLen = MultiByteToWideChar(CP_ACP, 0, sToMatch.c_str(), sToMatch.size(), 0, 0); // 计算转换后宽字符串的长度。（不包含字符串结束符）
	wchar_t *lpwsz = new wchar_t[iWLen + 1];
	MultiByteToWideChar(CP_ACP, 0, sToMatch.c_str(), sToMatch.size(), lpwsz, iWLen); // 正式转换。
	lpwsz[iWLen] = L'/0';
	std::wstring wsToMatch(lpwsz);
	delete[]lpwsz;
	return wsToMatch;
}
//把宽字符串转换成字符串，输出使用
std::string wstring2string(std::wstring sToMatch)
{
	std::string sResult;
	int iLen = WideCharToMultiByte(CP_ACP, NULL, sToMatch.c_str(), -1, NULL, 0, NULL, FALSE); // 计算转换后字符串的长度。（包含字符串结束符）
	char *lpsz = new char[iLen];
	WideCharToMultiByte(CP_OEMCP, NULL, sToMatch.c_str(), -1, lpsz, iLen, NULL, FALSE); // 正式转换。
	sResult.assign(lpsz, iLen - 1); // 对string对象进行赋值。
	delete[]lpsz;
	return sResult;
}