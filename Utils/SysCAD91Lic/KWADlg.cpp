//============== Copyright Kenwalt (Pty) Ltd (1997, ...) ====================
// $Log:  $
// 
// $Nokeywords: $
//===========================================================================
#include "stdafx.h"
#include "syscad91lic.h"
#include "syscadlicver.h"
#include "license.h"
#include "kwaDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------

#define WMU_INITLICENSE       (WM_USER + 500)

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
  SetDlgItemText(IDC_TXT_COPYRIGHT, KWA_VERINFO_CopyrightFull);
  SetDlgItemText(IDC_TXT_COMPANY, KWA_VERINFO_CompanyFull);
  char Buff[128];
  sprintf(Buff, "SysCAD 9.1 Licensing Utility (Version %d.%d)", KWA_VERINFO_V0, KWA_VERINFO_V1);
  #ifdef _DEBUG
  strcat(Buff, " (Debug)");
  #endif
  SetDlgItemText(IDC_TXT_FULLVERSION, Buff);
  return TRUE;
  }

//---------------------------------------------------------------------------
//===========================================================================
//===========================================================================

void SetVisibleWindowPos(CWnd* pWnd, int xPos, int yPos)
  {//ensure window is visible, not off edge of screen because of change in resolution...
  const int MaxX = GetSystemMetrics(SM_CXSCREEN) - 1;
  const int MaxY = GetSystemMetrics(SM_CYSCREEN) - 1;
  if (xPos>=MaxX)
    xPos = MaxX - GetSystemMetrics(SM_CXSIZE) - 180;
  if (yPos>=MaxY)
    yPos = MaxY - GetSystemMetrics(SM_CYSIZE) - 110;
  pWnd->SetWindowPos(NULL, xPos, yPos, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
  }

//===========================================================================

CKwaDlg::CKwaDlg(CWnd* pParent /*=NULL*/)
  : CDialog(CKwaDlg::IDD, pParent)
  {
  //{{AFX_DATA_INIT(CKwaDlg)
  //}}AFX_DATA_INIT
  // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
  m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
  }

//---------------------------------------------------------------------------

void CKwaDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CKwaDlg)
  //}}AFX_DATA_MAP
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CKwaDlg, CDialog)
  //{{AFX_MSG_MAP(CKwaDlg)
  ON_WM_SYSCOMMAND()
  ON_WM_PAINT()
  ON_WM_QUERYDRAGICON()
  ON_BN_CLICKED(IDC_ABOUT, OnAbout)
  ON_BN_CLICKED(IDC_AUTHORIZE, OnAuthorize)
  ON_BN_CLICKED(IDC_INFORMATION, OnInformation)
  ON_WM_RBUTTONUP()
  ON_BN_CLICKED(IDC_MORE, OnMore)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
  ON_COMMAND_RANGE(ID_CK_LIC_TRIAL, ID_CK_LIC_INFO, OnLicense)
  ON_UPDATE_COMMAND_UI_RANGE(ID_CK_LIC_TRIAL, ID_CK_LIC_INFO, OnUpdateLicense)
  ON_MESSAGE(WMU_INITLICENSE, OnInitLicenseMsg)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CKwaDlg::OnInitDialog()
  {
  CWaitCursor Wait;
  CDialog::OnInitDialog();
  
  // Add "About..." menu item to system menu.

  // IDM_ABOUTBOX must be in the system command range.
  ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
  ASSERT(IDM_ABOUTBOX < 0xF000);

  CMenu* pSysMenu = GetSystemMenu(FALSE);
  CString strAboutMenu;
  strAboutMenu.LoadString(IDS_ABOUTBOX);
  if (!strAboutMenu.IsEmpty())
    {
    CMenu M;
    if (M.LoadMenu(IDR_LICMENU))
      {
      pSysMenu->AppendMenu(MF_SEPARATOR);
      pSysMenu->AppendMenu(MF_POPUP, (UINT)M.m_hMenu, "License");
      }
    pSysMenu->AppendMenu(MF_SEPARATOR);
    pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
    }

  // Set the icon for this dialog.  The framework does this automatically
  //  when the application's main window is not a dialog
  SetIcon(m_hIcon, TRUE);   // Set big icon
  SetIcon(m_hIcon, FALSE);  // Set small icon

  CRect R;
  GetClientRect(&R);
  int x = AfxGetApp()->GetProfileInt("License", "XPos", ((GetSystemMetrics(SM_CXSCREEN)-R.Width())-1)/2);
  int y = AfxGetApp()->GetProfileInt("License", "YPos", ((GetSystemMetrics(SM_CYSCREEN)-R.Height())-1)/2);
  SetVisibleWindowPos(this, x, y);

  char Buff[256];
  sprintf(Buff, "Version %d.%d", KWA_VERINFO_V0, KWA_VERINFO_V1);
  #ifdef _DEBUG
  strcat(Buff, " (Debug)");
  #endif
  SetDlgItemText(IDC_TXT_VERSION, Buff);
  SetDlgItemText(IDC_TXT_LICENSE, "Checking license...");
  SetDlgItemText(IDC_TXT_USERS, "");

  PostMessage(WMU_INITLICENSE, 0, 0);
  
  return TRUE;  // return TRUE  unless you set the focus to a control
  }

//---------------------------------------------------------------------------

LRESULT CKwaDlg::OnInitLicenseMsg(WPARAM wParam, LPARAM lParam)
  {
  CWaitCursor Wait;
  BOOL LicenseFailed = !Lic.Init();
  if (!LicenseFailed)
    {
    if (!Lic.Check())
      {
      #if CK_ALLOWDEMOMODE
      Lic.SetDemoMode();
      #else
      LicenseFailed = !Lic.StartDialog();
      if (!LicenseFailed)
        LicenseFailed = !Lic.Check(); //check again!
      #endif
      }
    }
  CWaitCursor Wait1; //old hour glass may be gone
  if (LicenseFailed)
    {
    AfxMessageBox("License Failed");
    PostMessage(WM_COMMAND, IDCANCEL);
    return TRUE;
    }

  UpdateInfo();

  /*if (Lic.DemoMode())
    //LogNote("License", MB_OK|MB_ICONINFORMATION, "Using SysCAD in Demo Mode");
    LogWarning("License", 0, "Using SysCAD in Demo Mode");*/

  return TRUE;
  }

//---------------------------------------------------------------------------

void CKwaDlg::UpdateInfo()
  {
  if (Lic.Licensed())
    {
    char Buff[256];
    if (Lic.DaysLeft()==CK_InfiniteDays)
      sprintf(Buff, "Software is licensed");
    else
      sprintf(Buff, "License expires in %d days", Lic.DaysLeft());
    SetDlgItemText(IDC_TXT_LICENSE, Buff);
    sprintf(Buff, "Copies allowed:%d     Multiple users allowed:%d", Lic.Copies(), Lic.MultiUsers());
    SetDlgItemText(IDC_TXT_USERS, Buff);
    /*if (Lic.DaysLeft()<=CK_WarnDays)
      LogWarning("License", Lic.DaysLeft()<=CK_UrgentWarnDays ? MB_OK|MB_ICONINFORMATION : 0, "License expires in %d days", Lic.DaysLeft());
    else if (Lic.DaysLeft()!=CK_InfiniteDays)
      LogNote("License", 0, "License expires in %d days", Lic.DaysLeft());*/
    }
  else
    {
    SetDlgItemText(IDC_TXT_LICENSE, "Software is NOT licensed");
    SetDlgItemText(IDC_TXT_USERS, "Users:0");
    }
  }

//---------------------------------------------------------------------------

void CKwaDlg::OnSysCommand(UINT nID, LPARAM lParam)
  {
  UINT id = (nID & 0xFFF0);
  if (id == IDM_ABOUTBOX)
    {
    CAboutDlg dlgAbout;
    dlgAbout.DoModal();
    }
  else if (id>=ID_CK_LIC_TRIAL && id<=ID_CK_LIC_INFO)
    {
    OnLicense(id);
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

void CKwaDlg::OnPaint() 
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
HCURSOR CKwaDlg::OnQueryDragIcon()
  {
  return (HCURSOR) m_hIcon;
  }

//---------------------------------------------------------------------------

void CKwaDlg::OnAbout() 
  {
  CAboutDlg dlgAbout;
  dlgAbout.DoModal();
  }

//---------------------------------------------------------------------------

void CKwaDlg::OnLicense(UINT nID)
  {
  char Buff[256];
  switch (nID)
    {
    case ID_CK_LIC_TRIAL:
      //if (Lic.IssueTrial(CK_NoOfTrialDays, TRUE))
      //  AfxMessageBox("Trial License has been issued.");
      break;
    case ID_CK_LIC_AUTH: 
      if (Lic.IssueLicense())
        AfxMessageBox("License has been issued.");
      break;
    case ID_CK_LIC_KILL:
      {
      CString ConfirmCode;
      if (Lic.Kill(ConfirmCode))
        {
        sprintf(Buff, "License has been removed.\nConfirmation code '%s'", (const char*)ConfirmCode);
        AfxMessageBox(Buff);
        }
      break;
      }
    case ID_CK_LIC_REGTRANSFER:
      if (Lic.DoRegisterTransfer())
        AfxMessageBox("License transfer has been registered.");
      break;
    case ID_CK_LIC_TRANSOUT:
      if (Lic.DoTransferOut())
        AfxMessageBox("License has been transfered out.");
      break;
    case ID_CK_LIC_TRANSIN:
      if (Lic.DoTransferIn())
        AfxMessageBox("License has been transfered in.");
      break;
    case ID_CK_LIC_DIRECTTRANS:
      if (Lic.DoDirectTransfer())
        AfxMessageBox("License has been transfered.");
      break;
    case ID_CK_LIC_INFO:
      Lic.Info();
      break;
    }
  Lic.QuickCheck(0);
  UpdateInfo();
  }

//---------------------------------------------------------------------------

void CKwaDlg::OnUpdateLicense(CCmdUI* pCmdUI)
  {
  BOOL NotActive = (!Lic.Blocked());
  switch (pCmdUI->m_nID)
    {
    case ID_CK_LIC_TRIAL:
      pCmdUI->Enable(FALSE);
      //pCmdUI->Enable(!Lic.Licensed() && !Lic.TrialFailed() && NotActive);
      break;
    case ID_CK_LIC_AUTH:
      pCmdUI->Enable(NotActive);
      break;
    case ID_CK_LIC_INFO:
      pCmdUI->Enable(TRUE);
      break;
    case ID_CK_LIC_KILL:
    case ID_CK_LIC_TRANSOUT:
    case ID_CK_LIC_DIRECTTRANS:
      pCmdUI->Enable(Lic.Licensed() && NotActive);
      break;
    case ID_CK_LIC_REGTRANSFER:
    case ID_CK_LIC_TRANSIN:
      pCmdUI->Enable(!Lic.Licensed() && NotActive);
      break;
    }
  }

//---------------------------------------------------------------------------

void CKwaDlg::OnAuthorize() 
  {
  OnLicense(ID_CK_LIC_AUTH);
  }

//---------------------------------------------------------------------------

void CKwaDlg::OnInformation() 
  {
  OnLicense(ID_CK_LIC_INFO);
  }

//---------------------------------------------------------------------------

void CKwaDlg::PopupMenu(CPoint point)
  {
  CMenu M;
  if (M.LoadMenu(IDR_LICMENU))
    {
    CMenu Menu;
    Menu.CreatePopupMenu();
    Menu.AppendMenu(MF_POPUP, (UINT)M.m_hMenu, "License");
    POINT ScreenPoint = point;
    // Convert the mouse point to screen coordinates since that is what TrackPopupMenu() expects.
    ClientToScreen(&ScreenPoint);
    CMenu* pMenuTrackPopup = Menu.GetSubMenu(0);
    pMenuTrackPopup->TrackPopupMenu(TPM_RIGHTBUTTON, ScreenPoint.x, ScreenPoint.y, this, NULL);
    Menu.DestroyMenu();
    }
  }

//---------------------------------------------------------------------------

void CKwaDlg::OnRButtonUp(UINT nFlags, CPoint point) 
  {
  PopupMenu(point);
  CDialog::OnRButtonUp(nFlags, point);
  }

//---------------------------------------------------------------------------

void CKwaDlg::OnMore() 
  {
  CPoint point(10,10);
  PopupMenu(point);
  }

//---------------------------------------------------------------------------

void CKwaDlg::SaveWindowPos()
  {
  CRect R;
  GetWindowRect(&R);
  AfxGetApp()->WriteProfileInt("License", "XPos", R.left);
  AfxGetApp()->WriteProfileInt("License", "YPos", R.top);
  }

//---------------------------------------------------------------------------

void CKwaDlg::OnClose() 
  {
  SaveWindowPos();
  CDialog::OnClose();
  }

//---------------------------------------------------------------------------

void CKwaDlg::OnOK() 
  {
  SaveWindowPos();
  CDialog::OnOK();
  }

//---------------------------------------------------------------------------

void CKwaDlg::OnCancel() 
  {
  SaveWindowPos();
  CDialog::OnCancel();
  }

//---------------------------------------------------------------------------
