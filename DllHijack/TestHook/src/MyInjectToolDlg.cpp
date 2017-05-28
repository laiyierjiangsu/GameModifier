
// MyInjectToolDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MyInjectTool.h"
#include "MyInjectToolDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMyInjectToolDlg 对话框



CMyInjectToolDlg::CMyInjectToolDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMyInjectToolDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMyInjectToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_listProcess);
	DDX_Control(pDX, IDC_LIST1, m_listDll);
}

BEGIN_MESSAGE_MAP(CMyInjectToolDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(ID_SHOW, &CMyInjectToolDlg::OnShowProcess)
	ON_WM_CONTEXTMENU()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST2, &CMyInjectToolDlg::OnItemchangedList1)
	ON_COMMAND(ID_IMPORT, &CMyInjectToolDlg::OnImport)
	ON_COMMAND(ID_THREAD, &CMyInjectToolDlg::OnThread)
	ON_COMMAND(ID_PROCESS, &CMyInjectToolDlg::OnProcess)
	ON_COMMAND(ID_DEBUG, &CMyInjectToolDlg::OnDebug)
	ON_COMMAND(ID_APC, &CMyInjectToolDlg::OnApc)
	ON_COMMAND(ID_REG, &CMyInjectToolDlg::OnReg)
	ON_COMMAND(ID_HOOK, &CMyInjectToolDlg::OnHook)
	ON_COMMAND(ID_REMOTE, &CMyInjectToolDlg::OnRemote)
	ON_COMMAND(ID_INPUT, &CMyInjectToolDlg::OnInput)
	ON_COMMAND(ID_HIJACK, &CMyInjectToolDlg::OnHijack)
	ON_COMMAND(ID_ANTIHOOK, &CMyInjectToolDlg::OnAntihook)
END_MESSAGE_MAP()


// CMyInjectToolDlg 消息处理程序

BOOL CMyInjectToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	m_listProcess.InsertColumn(0,"进程名称",LVCFMT_LEFT,150,-1);
    m_listProcess.InsertColumn(1,"进程ID",LVCFMT_LEFT,150,-1);
    m_listProcess.InsertColumn(2,"映像路径",LVCFMT_LEFT,500,-1);
    
    m_listDll.InsertColumn(0,"模块名称",LVCFMT_LEFT,150,-1);
    m_listDll.InsertColumn(1,"模块路径",LVCFMT_LEFT,500,-1);
    ZeroMemory(m_dwID,sizeof(DWORD) * 1000);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

/*******************************************************
*函数功能:提权代码
*函数参数:无
*函数返回:bool
*注意事项:无
*最后修改时间:2017/5/13
*******************************************************/
bool CMyInjectToolDlg::EnableDebugPriv() //提权代码
{
	HANDLE hToken;

	LUID sedebugnameValue;

	TOKEN_PRIVILEGES tkp;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		return false;
	}
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &sedebugnameValue))
	{
		CloseHandle(hToken);

		return false;
	}

	tkp.PrivilegeCount = 1;

	tkp.Privileges[0].Luid = sedebugnameValue;

	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL))
	{
		CloseHandle(hToken);

		return false;
	}
	return true;
}

/*******************************************************
*函数功能:遍历所有的进程并且显示
*函数参数:无
*函数返回:无
*注意事项:无
*最后修改时间:2017/5/13
*******************************************************/
void CMyInjectToolDlg::OnShowprocess()
{
	ZeroMemory(m_dwID, sizeof(DWORD)* 1000);
	m_listProcess.DeleteAllItems();
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);

	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		MessageBox("获取系统进程快照失败！！！");
		return;
	}

	BOOL bResult = Process32First(hProcessSnap, &pe32);
	int i = 0;
	while (bResult)
	{

		CString myStr = pe32.szExeFile;
		CString myStrId;

		int nId = pe32.th32ProcessID;
		myStrId.Format("%d", nId);
		m_listProcess.InsertItem(i, myStr);
		m_listProcess.SetItemText(i, 1, myStrId);
		m_listProcess.SetItemText(i, 2, pe32.szExeFile);
		m_dwID[i] = pe32.th32ProcessID;
		i++;
		bResult = Process32Next(hProcessSnap, &pe32);
	}

	CloseHandle(hProcessSnap);
}

/*******************************************************
*函数功能:按照进程ID显示所有DLL模块
*函数参数:参数1：当前列表行数
*函数返回:无
*注意事项:无
*最后修改时间:2017/5/13
*******************************************************/
void CMyInjectToolDlg::OnShowmoudle(int nIndex)
{
	DWORD dwId = 0;

	EnableDebugPriv();
	dwId = m_dwID[nIndex];

	HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
	m_listDll.DeleteAllItems();

	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwId);
	int n = GetLastError();
	if (hModuleSnap == INVALID_HANDLE_VALUE){
		printf("CreateToolhelp32SnapshotError! \n");
		return;
	}

	MODULEENTRY32 module32;
	module32.dwSize = sizeof(module32);
	BOOL bResult = Module32First(hModuleSnap, &module32);

	int i = 0;
	while (bResult){
		CString strDllName = module32.szModule;
		CString strDllPath = module32.szExePath;

		m_listDll.InsertItem(i, strDllName);
		m_listDll.SetItemText(i, 1, strDllPath);
		bResult = Module32Next(hModuleSnap, &module32);
		i++;
	}

	CloseHandle(hModuleSnap);
}


void CMyInjectToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMyInjectToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMyInjectToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMyInjectToolDlg::OnShowProcess()
{
	// TODO:  在此添加命令处理程序代码
	m_listProcess.DeleteAllItems();
	OnShowprocess();
}


void CMyInjectToolDlg::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	// TODO:  在此处添加消息处理程序代码
	CMenu popMenu;

	popMenu.LoadMenu(IDR_MENU1);         //载入菜单

	CMenu *pPopup;

	pPopup = popMenu.GetSubMenu(0);     //获得子菜单指针



	pPopup->EnableMenuItem(ID_SHOW, MF_BYCOMMAND | MF_ENABLED);  //允许菜单项使用





	//ClientToScreen(&point);            //将客户区坐标转换成屏幕坐标

	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);                //显示弹出菜单，参数依次为(鼠标在菜单左边|跟踪右键，x，y，this)

	pPopup->Detach();

	popMenu.DestroyMenu();
}


void CMyInjectToolDlg::OnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if (pNMLV->uNewState)
	{
		OnShowmoudle(pNMLV->iItem);
	}

	*pResult = 0;
}


void CMyInjectToolDlg::OnImport()
{
	// TODO:  在此添加命令处理程序代码
	m_pImportDlg = new ImportInject;
	m_pImportDlg->Create(IDD_DIALOG1);
	m_pImportDlg->ShowWindow(SW_SHOW);
}


void CMyInjectToolDlg::OnThread()
{
	// TODO:  在此添加命令处理程序代码
	m_pThreadDlg = new ThreadInject;
	m_pThreadDlg->Create(IDD_DIALOG2);
	m_pThreadDlg->ShowWindow(SW_SHOW);
}


void CMyInjectToolDlg::OnProcess()
{
	// TODO:  在此添加命令处理程序代码
	m_pProcessDlg = new ProcessInject;
	m_pProcessDlg->Create(IDD_DIALOG3);
	m_pProcessDlg->ShowWindow(SW_SHOW);
}


void CMyInjectToolDlg::OnDebug()
{
	// TODO:  在此添加命令处理程序代码
	m_pDebugDlg = new DebugInject;
	m_pDebugDlg->Create(IDD_DIALOG4);
	m_pDebugDlg->ShowWindow(SW_SHOW);
}


void CMyInjectToolDlg::OnApc()
{
	// TODO:  在此添加命令处理程序代码
	m_pApcDlg = new APCInject;
	m_pApcDlg->Create(IDD_DIALOG5);
	m_pApcDlg->ShowWindow(SW_SHOW);
}


void CMyInjectToolDlg::OnReg()
{
	// TODO:  在此添加命令处理程序代码
	m_RegDlg = new RegInject;
	m_RegDlg->Create(IDD_DIALOG6);
	m_RegDlg->ShowWindow(SW_SHOW);
}


void CMyInjectToolDlg::OnHook()
{
	// TODO:  在此添加命令处理程序代码
	m_HookDlg = new HookInject;
	m_HookDlg->Create(IDD_DIALOG7);
	m_HookDlg->ShowWindow(SW_SHOW);
}


void CMyInjectToolDlg::OnRemote()
{
	// TODO:  在此添加命令处理程序代码
	m_RemoteInjectDlg = new RemoteInject;
	m_RemoteInjectDlg->Create(IDD_DIALOG10);
	m_RemoteInjectDlg->ShowWindow(SW_SHOW);
}


void CMyInjectToolDlg::OnInput()
{
	// TODO:  在此添加命令处理程序代码
	m_InputInjectDlg = new InputInject;
	m_InputInjectDlg->Create(IDD_DIALOG9);
	m_InputInjectDlg->ShowWindow(SW_SHOW);
}


void CMyInjectToolDlg::OnHijack()
{
	// TODO:  在此添加命令处理程序代码
	m_HijackDlg = new Hijack;
	m_HijackDlg->Create(IDD_DIALOG8);
	m_HijackDlg->ShowWindow(SW_SHOW);
}


void CMyInjectToolDlg::OnAntihook()
{
	// TODO:  在此添加命令处理程序代码
	m_BlookHookDlg = new BlookHook;
	m_BlookHookDlg->Create(IDD_DIALOG11);
	m_BlookHookDlg->ShowWindow(SW_SHOW);
}
