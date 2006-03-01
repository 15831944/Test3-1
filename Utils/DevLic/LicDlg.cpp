// ScdLicDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DevLic.h"
#include "LicDlg.h"
#include "DLLLicenseDialog.h"
#include ".\licdlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
protected:
	//{{AFX_MSG(CAboutDlg)
    virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
  };

//---------------------------------------------------------------------------

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
  {
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
  }

//---------------------------------------------------------------------------

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
  {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CAboutDlg::OnInitDialog() 
  {
  CDialog::OnInitDialog();
  CString Title, RevStr;
  GetWindowText(Title);
  Title += "  Rev ";
  CScribble::GetScribbleVer(RevStr);
  Title += RevStr;
  SetWindowText((const char*)Title);
  return TRUE;
  }

//---------------------------------------------------------------------------
//===========================================================================
// CDevLicDlg dialog

CDevLicDlg::CDevLicDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDevLicDlg::IDD, pParent)
  {
	//{{AFX_DATA_INIT(CDevLicDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
  bForcedLoc = false;
  }

//---------------------------------------------------------------------------

CDevLicDlg::~CDevLicDlg()
  {
//  Lic.Exit();
  }

//---------------------------------------------------------------------------

void CDevLicDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CDevLicDlg)
  //}}AFX_DATA_MAP
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CDevLicDlg, CDialog)
	//{{AFX_MSG_MAP(CDevLicDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_INIT, OnInit)
	ON_BN_CLICKED(IDC_LOCATION, OnLocation)
	ON_UPDATE_COMMAND_UI(IDC_LOCATION, OnUpdateLocation)
	ON_BN_CLICKED(IDC_BUTTON_DLL, OnButtonDll)
	ON_BN_CLICKED(IDC_LAUNCH_SCDLic, OnLaunchLicense)
	ON_BN_CLICKED(IDC_CHECKLICENSE, OnCheckLicense)
	//}}AFX_MSG_MAP
  ON_BN_CLICKED(IDC_CLIPBOARDCOPY, OnBnClickedClipboardcopy)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CDevLicDlg::OnInitDialog()
  {
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
    {
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
      {
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
      }
    }

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
extern CDevLicApp theApp;

  CString s;
  char Buff[1024];
//  if (bForcedLoc)
//    s = sForcedLoc;
//  else
  s = AppPF.RdStr("Developers License 2.0", "SMDK License Location" , "");

  sprintf(Buff, "Location : %s", s.GetLength()>0 ? (const char*)s : "Not Set");
  SetDlgItemText(IDC_LOCATION_TXT, Buff);
  sprintf(Buff, "Commandline : %s", theApp.sCmdLine.GetLength()>0 ? (const char*)theApp.sCmdLine : "none");
  SetDlgItemText(IDC_CMDLINE_TXT, Buff);

  CString Title, RevStr;
  GetWindowText(Title);
  Title += "  Rev ";
  CScribble::GetScribbleVer(RevStr);
  Title += RevStr;
  SetWindowText((const char*)Title);
  UpdateDialogControls(this, false);
  return TRUE;  // return TRUE  unless you set the focus to a control
  }

//---------------------------------------------------------------------------

void CDevLicDlg::OnSysCommand(UINT nID, LPARAM lParam)
  {
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
    {
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
    }
	else
    {
		CDialog::OnSysCommand(nID, lParam);
    }
  }

//---------------------------------------------------------------------------
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDevLicDlg::OnPaint() 
  {
	if (IsIconic())
    {
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
    }
	else
    {
		CDialog::OnPaint();
    }
  }

//---------------------------------------------------------------------------
// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDevLicDlg::OnQueryDragIcon()
  {
  return (HCURSOR)m_hIcon;
  }

//---------------------------------------------------------------------------

void CDevLicDlg::OnInit() 
  {
  SetDlgItemText(IDC_BUSY_TXT, "Busy initialising...");
  char Buff[1024];
  sprintf(Buff, "Initialise : OK!");
  CWaitCursor Wait;

  CString s;

  s = AppPF.RdStr("Developers License 2.0", "SMDK License Location" , "");

//  sprintf(Buff, "Location : %s", (Lic.GetAppPath() ? Lic.GetAppPath() : "current"));
//  SetDlgItemText(IDC_LOCATION_TXT, Buff);

  UpdateDialogControls(this, false);
  }

//---------------------------------------------------------------------------

void CDevLicDlg::OnLocation() 
{
  CLicenseLocationDlg Dlg;

  // Retrieve the License Path from the ini file
  if ( Dlg.m_AppPath == "" )
    Dlg.m_AppPath = AppPF.RdStr("Developers License 2.0", "SMDK License Location" , "");

  if (Dlg.DoModal()==IDOK)
  {
    if (Dlg.m_AppPath.GetLength()>0 && Dlg.m_AppPath[Dlg.m_AppPath.GetLength()-1]!='\\')
      Dlg.m_AppPath += '\\';
  }

  // Store the License Path in the ini file    
  AppPF.WrStr("Developers License 2.0", "SMDK License Location", Dlg.m_AppPath);

  char Buff[1024];
  sprintf(Buff, "Location : %s", Dlg.m_AppPath.GetLength()>0 ? (const char*)Dlg.m_AppPath : "Not Set");

  SetDlgItemText(IDC_LOCATION_TXT, Buff);

  UpdateData(false);
}

//---------------------------------------------------------------------------

void CDevLicDlg::OnUpdateLocation(CCmdUI* pCmdUI)
  {

  }


//---------------------------------------------------------------------------

void CDevLicDlg::OnButtonDll() 
{
extern CDevLicApp theApp;
  // Get the latest License Location
  theApp.sLicenseLoc = AppPF.RdStr("Developers License 2.0", "SMDK License Location" , "");

  // Check for valid license
  if ( theApp.CheckLicense() )
  {
    // If so we can launch the license dll dialog

    CDLLLicenseDialog Dlg;

    // Retrieve the DLLName from the ini file
    if ( Dlg.m_DllName == "" )
      Dlg.m_DllName = AppPF.RdStr("Developers License 2.0", "SMDK DLL Name" , "");

    UpdateData(false);

    if (Dlg.DoModal()==IDOK)
    {

    }
	
    // Store the DLLName in the ini file
    if ( Dlg.m_DllName != "" )
      AppPF.WrStr("Developers License 2.0", "SMDK DLL Name", Dlg.m_DllName);

    UpdateData(false);
  }
  else
  {
    AfxMessageBox("Developer License NOT found!\nCheck your license location");
  }
}

//---------------------------------------------------------------------------

void CDevLicDlg::OnLaunchLicense() 
{
extern CDevLicApp theApp;
  // Get the latest License Location
  theApp.sLicenseLoc = AppPF.RdStr("Developers License 2.0", "SMDK License Location" , "");
  CString KWA = theApp.sLicenseLoc;
  KWA += "kwa.exe";
  CFileStatus Status;
  if (CFile::GetStatus((LPCTSTR)KWA, Status))
    {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);
    si.wShowWindow = SW_SHOWDEFAULT;
    BOOL b = CreateProcess(NULL, (char*)(const char*)KWA, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
    if (!b)
      AfxMessageBox("Unable to start kwa.exe");
    else
      {
      int Cnt = 0;
      DWORD ExitCode;
      GetExitCodeProcess(pi.hProcess, &ExitCode);
      while (ExitCode==STILL_ACTIVE && Cnt<500000)
        {
        Cnt++;
        Sleep(200);
        GetExitCodeProcess(pi.hProcess, &ExitCode);
        }
      if (Cnt>=500000)
        AfxMessageBox("Error: not waiting for kwa.exe to exit!");
      if (ExitCode)
        {
        Sleep(200);
        //AfxMessageBox("Error: BAD ExitCode!");
        }
      //Lic.Init();
      //Lic.Check(CK_SMDK_Mask);
      //SetDlgItemText(IDC_TXT_LICENSEOK, Lic.OK() ? "Licensed" : "NOT licensed");
      }
    }
  else
    {
    char Buff[512];
    sprintf(Buff, "Unable to find 'kwa.exe' at '%s'\nEnter correct location", (const char*)theApp.sLicenseLoc);
    AfxMessageBox(Buff);
    }
	
}

void CDevLicDlg::OnCheckLicense() 
{
extern CDevLicApp theApp;
  // Get the latest License Location
  theApp.sLicenseLoc = AppPF.RdStr("Developers License 2.0", "SMDK License Location" , "");

  // Check for valid license
  if ( theApp.CheckLicense() )
  {
    AfxMessageBox("Developer License OK", MB_OK|MB_ICONINFORMATION);
  }
  else
  {
    AfxMessageBox("Developer License NOT found!\nCheck your license location", MB_OK|MB_ICONEXCLAMATION);
  }
	
}

void CDevLicDlg::OnBnClickedClipboardcopy()
  {
  const char* buff = "\"..\\..\\devlic.exe\" /q /f:\"$(TargetPath)\"";
  if (OpenClipboard())
    {
    if (EmptyClipboard())
      {
      const int len = strlen(buff);
      HGLOBAL hData = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, len+1);
      char *p = (char *)GlobalLock(hData);
      strcpy(p, buff);
      GlobalUnlock(hData);
      if (SetClipboardData(CF_TEXT, hData)==NULL)
        GlobalFree(hData);
      }
    CloseClipboard();
    }
  }
