#pragma once


#include "stdafx.h"
// Hijack ¶Ô»°¿ò
#include "PEFuncs.h"
class Hijack
{
public:
	Hijack();
	//CListCtrl m_mylist;
	PMAP_FILE_STRUCT m_pstMapFile;
	BOOL m_bIsFirst;
	std::list<std::string> m_FunNameList;

public:
	BOOL GetAndShowExports();
	void StartHijack();
	void OnDropFiles(HDROP hDropInfo);
	std::string m_strInjectFile;
	
};
