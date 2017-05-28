#pragma once


// ThreadInject 对话框

class ThreadInject : public CDialogEx
{
	DECLARE_DYNAMIC(ThreadInject)

public:
	ThreadInject(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~ThreadInject();

// 对话框数据
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	// 进程ID
	DWORD m_dwPid;
	CString m_strDllPath;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
};
