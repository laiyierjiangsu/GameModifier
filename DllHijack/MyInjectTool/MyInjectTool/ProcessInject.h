#pragma once


// ProcessInject 对话框

class ProcessInject : public CDialogEx
{
	DECLARE_DYNAMIC(ProcessInject)

public:
	ProcessInject(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~ProcessInject();

// 对话框数据
	enum { IDD = IDD_DIALOG3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_strExePath;
	CString m_strDllPath;
	afx_msg void OnBnClickedInject();
	afx_msg void OnBnClickedFreemem();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
};
