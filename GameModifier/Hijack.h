#pragma once


#include "stdafx.h"
// Hijack �Ի���
#include "PEFuncs.h"
class Hijack
{
public:
	//CListCtrl m_mylist;
	PMAP_FILE_STRUCT m_pstMapFile;
	BOOL m_bIsFirst = TRUE;
	std::list<std::string> m_FunNameList;

public:
	BOOL GetAndShowExports();
	void StartHijack();
	void OnDropFiles(HDROP hDropInfo);
	std::string m_strInjectFile;
	
};