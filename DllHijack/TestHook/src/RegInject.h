#pragma once


// RegInject 对话框

class RegInject : public CDialogEx
{
	DECLARE_DYNAMIC(RegInject)

public:
	RegInject(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~RegInject();

// 对话框数据
	enum { IDD = IDD_DIALOG6 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_strDllPath;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
};
