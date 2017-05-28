#pragma once


// RemoteInject 对话框

class RemoteInject : public CDialogEx
{
	DECLARE_DYNAMIC(RemoteInject)

public:
	RemoteInject(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~RemoteInject();

// 对话框数据
	enum { IDD = IDD_DIALOG10 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	DWORD m_dwPID;
	CString m_strDllPath;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedInject();
};
