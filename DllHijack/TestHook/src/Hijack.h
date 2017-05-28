#pragma once
#include "afxcmn.h"


// Hijack 对话框

class Hijack : public CDialogEx
{
	DECLARE_DYNAMIC(Hijack)

public:
	Hijack(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~Hijack();

// 对话框数据
	enum { IDD = IDD_DIALOG8 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_mylist;
	PMAP_FILE_STRUCT m_pstMapFile;
	BOOL m_bIsFirst = TRUE;
	CStringList m_FunNameList;

public:
	BOOL GetAndShowExports();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	virtual BOOL OnInitDialog();
};
