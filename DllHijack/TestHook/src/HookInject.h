#pragma once


// HookInject 对话框

class HookInject : public CDialogEx
{
	DECLARE_DYNAMIC(HookInject)

public:
	HookInject(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~HookInject();

// 对话框数据
	enum { IDD = IDD_DIALOG7 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	// 进程ID
	DWORD m_dwPid;
	CString m_strDllPath;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedInject();
	afx_msg void OnBnClickedUnhook();
};
