
// MyInjectToolDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "ImportInject.h"
#include "ThreadInject.h"
#include "ProcessInject.h"
#include "DebugInject.h"
#include "APCInject.h"
#include "RegInject.h"
#include "HookInject.h"
#include "Hijack.h"
#include "InputInject.h"
#include "RemoteInject.h"
#include "BlookHook.h"
// CMyInjectToolDlg 对话框
class CMyInjectToolDlg : public CDialogEx
{
// 构造
public:
	CMyInjectToolDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MYINJECTTOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_listProcess;
	CListCtrl m_listDll;
	//存放每一行对应的进程ID
	DWORD m_dwID[1000];
	afx_msg void OnShowProcess();
	//遍历显示进程
	void CMyInjectToolDlg::OnShowprocess();
	//显示所有的DLL
	void OnShowmoudle(int nIndex);
	//提升权限
	bool EnableDebugPriv();
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnImport();
	afx_msg void OnThread();
	afx_msg void OnProcess();
	afx_msg void OnDebug();
	afx_msg void OnApc();
	afx_msg void OnReg();
	afx_msg void OnHook();
	afx_msg void OnRemote();
	afx_msg void OnInput();
	afx_msg void OnHijack();

public:
	ImportInject*  m_pImportDlg;
	ThreadInject*  m_pThreadDlg;
	ProcessInject* m_pProcessDlg;
	DebugInject*   m_pDebugDlg;
	APCInject*     m_pApcDlg;
	RegInject*     m_RegDlg;
	HookInject*    m_HookDlg;
	Hijack*        m_HijackDlg;
	InputInject*   m_InputInjectDlg;
	RemoteInject*  m_RemoteInjectDlg;
	BlookHook*     m_BlookHookDlg;

	afx_msg void OnAntihook();
};
