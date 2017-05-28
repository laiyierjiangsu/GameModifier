#pragma once
#include "afxwin.h"


// DebugInject 对话框

class DebugInject : public CDialogEx
{
	DECLARE_DYNAMIC(DebugInject)

public:
	DebugInject(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~DebugInject();

// 对话框数据
	enum { IDD = IDD_DIALOG4 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_strExePath;
	CString m_strDllPath;
	afx_msg void OnBnClickedInject();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton3();

};
