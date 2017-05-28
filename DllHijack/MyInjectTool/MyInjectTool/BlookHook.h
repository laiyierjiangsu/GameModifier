#pragma once


// BlookHook 对话框

class BlookHook : public CDialogEx
{
	DECLARE_DYNAMIC(BlookHook)

public:
	BlookHook(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~BlookHook();

// 对话框数据
	enum { IDD = IDD_DIALOG11 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	DWORD m_dwPID;
	afx_msg void OnBnClickedButton1();
};
