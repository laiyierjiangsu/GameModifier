#pragma once


// APCInject 对话框

class APCInject : public CDialogEx
{
	DECLARE_DYNAMIC(APCInject)

public:
	APCInject(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~APCInject();

// 对话框数据
	enum { IDD = IDD_DIALOG5 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_strExePath;
	CString m_strDllPath;
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedInject();
};
