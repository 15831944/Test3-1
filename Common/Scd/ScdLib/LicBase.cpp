//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================


#include "stdafx.h"
#define __LICBASE_CPP
#include "licbase.h"
#include "scdver.h"
#include "scdverlic.h"

//#if CK_LICENSINGON
#include "sc_defs.h"
#include "tagobj.h"
#include "scd_wm.h"
#include "scdctrls.h"
//#include "msgwnd.h"
#include "resource.h"


#if CK_LICENSINGON

#define CK_USE6134        01             

#import "CrypKeyCOM.dll" no_namespace named_guids
static ICrypKeySDKPtr s_ptr;


#if CK_USE6134 
#include "crypkey.6134.h"
#else
#include "crypkey.57.h"
#endif

#endif

#include "winsvc.h"

#include "optoff.h"

//===========================================================================
//=== Generic Code...

#if CK_USE6134 
const char* LicINISection = "License61";
#else
const char* LicINISection = "License";
#endif

char* CK_KeyFile = "syscad.exe";
char* CK_AppName = "SysCAD";
/*//for example.exe...
#define CK_USER_KEY   "D050 815C D1A2 A79D B1"
#define CK_MASTER_KEY "2A5D 57C4 1B4C 135B F09E 17F7 600B 2D70 79E8 F275 C36A"

#define CK_COMPANYNUM 79560      //these are the correct numbers for example.exe
#define CK_PASSNUM    984534120  //you can use them in your tests if you need to
*/

#if !BYPASSLICENSING
//for syscad.exe                
#if CK_USE6134 

#define CK_USER_KEY   "DE5E 81ED E83A 12EB 81D2 C0FA 1B"
#define CK_MASTER_KEY "7d1c2e0f6da99db43c7c95c71ce87456daabb10b8766eb79b8e9cea\
b6d4dc8fc76f19b2cb190b66ca53f0decb4f9c26246841cc969ae2eb4c35f91ad1bed30d2e4\
cb851bb9a6dbb7270d4385df5499b0f45a65f273f72542cf2d388fd1fae7d9283dadd0aa21e\
b3656b892b302de4d71963c9c26497cec9275d03c2c5757cb9c"

#else

#define CK_USER_KEY   "DE5E 81ED E83A 12EB 81D2 C0FA 1B"
#define CK_MASTER_KEY "8EF9 57C4 A348 EBF4 A0F2 1089 4A9C D26F 7792 DF72 440D"

#endif

#define CK_COMPANYNUM 7956342
#define CK_PASSNUM    482693111

//NBNB Ensure correct version of CRP32D60.lib is used!
#if CK_USE6134 
#pragma comment(lib, "CRP32D60.6134.lib")            
#else
#pragma comment(lib, "CRP32D60.57.lib")
#endif

#endif

//const int CK_NetworkChecktime = 600;
const int CK_NetworkChecktime = 900;
const int CK_TrialVerNo = 1;          //Trial license version number
const int CK_NumDefinedLevels = 2; //number of bits to be used for level
const int CK_NumDefinedOpts = 30;
              
//===========================================================================
//=== SysCAD Specific Code...

const int CK_NoOfOptions = 24;
struct CK_OptionDetails
  {
  unsigned char iBitNumber;                          
  char* pName;
  bool fNormallyShow;
  };

CK_OptionDetails CK_OptionNames[CK_NoOfOptions] =
{
  { 25, "ProBal Mode", true },
  { 24, "Dynamic Mode (Simple)", true },
  { 23, "Dynamic Mode (Full)", true },
  { 22, "Electrical Mode", false },
  {  7, "MineServe Mode", false },
  { 20, "Full License (No=run-time only)", true },
  { 16, "OPC Server", true },
  { 17, "Drivers / Marshal", true },
  { 19, "Application COM Interface", true },
  { 18, "COM Specie Properties", true },
  {  7, "Design / Analyse options", false },
  { 12, "Extra models (Heat Exchange)", true },
  { 13, "Extra models (Heat Exchange Extra)", true },
  { 10, "Extra models (Size distribution)", true },
  { 11, "Extra models (Alumina)", true },
  { 14, "Extra models (Black Box)", true },
  { 15, "Extra models (Electrical)", false },
  {  0, "Extra models (User)", false },
  {  1, "Extra models (QAL)", false },
  {  2, "Extra models (QAL Extra)", false },
  {  5, "Extra models (RioTinto)", false },
  {  6, "Extra models (Alcan)", false },
  { 28, "Version 9.0", true },
  { 29, "Version 8.2", false }
};

char* CK_LevelNames[4] =
  { "Trial",
    "Normal",
    "Unused",
    "Unused"
    };

//===========================================================================
//
//
//
//===========================================================================

#if BYPASSLICENSING
int GetNumCopies()          { return 99; };
int GetNumMultiUsers()      { return 99; };
ULONG GetLevel(int NoLevels)  
  {
  return 1;  
  };
int Get1RestInfo(int X)
  { 
  return 0;  
  };
int GetRestrictionInfo(int * authopt, ULONG * start_date, int * num_allowed, int * num_used)
  {
  *authopt=0;
  *start_date=0;
  *num_allowed=1000;
  *num_used=0;
  return 0;
  };
int GetOption(int No, int Index)                  { return 1; };
int GetAuthorization(dword * dwOpLevel, int dec)  
  { 
  CK_SysCADSecurity Opt;

  Opt.m_OpLevel = 0; //clears all bits

  Opt.m_Opts.Level            = 1;
  Opt.m_Opts.Ver82            = 0;
  Opt.m_Opts.Ver90            = 1;
  Opt.m_Opts.Spare_Ver91      = 0;
  Opt.m_Opts.Spare_Ver92      = 0;
  Opt.m_Opts.Mode_ProBal      = 1;
  Opt.m_Opts.Mode_DynamicFlow = 1;
  Opt.m_Opts.Mode_DynamicFull = 1;
  Opt.m_Opts.Mode_Electrical  = 1;
  //Opt.m_Opts.Only_SteadyState = 0;
  Opt.m_Opts.Func_FullEdit    = 1;
  Opt.m_Opts.Func_COM         = 1;
  Opt.m_Opts.Func_COMProp     = 1;
  Opt.m_Opts.Func_Drivers     = 1;
  Opt.m_Opts.Func_OPCServer   = 1;
  Opt.m_Opts.Mdls_Electrical  = 1;
  Opt.m_Opts.Mdls_BlackBox    = 1;
  Opt.m_Opts.Mdls_HeatExtra   = 1;
  Opt.m_Opts.Mdls_HeatBal     = 1;
  Opt.m_Opts.Mdls_Alumina     = 1;
  Opt.m_Opts.Mdls_SizeDist    = 1;
  Opt.m_Opts.Old_Dynamic      = 1;
  Opt.m_Opts.Old_Probal       = 1;
  Opt.m_Opts.Client_MineServe = 0;
  Opt.m_Opts.Client_Alcan     = 1;
  Opt.m_Opts.Client_RTTS      = 1;
  Opt.m_Opts.Old_Dynamic82    = 1;
  Opt.m_Opts.Old_Probal82     = 1;
  Opt.m_Opts.Client_QALExtra  = 1;
  Opt.m_Opts.Client_QAL       = 1;
  Opt.m_Opts.Client_Other     = 1;


  *dwOpLevel=Opt.m_OpLevel; 

  return 0; 
  };
int CrypkeyVersion()                              { return 11; };
LPTSTR MyExplainErr(int Func, int err)              { return "Explanation"; };
#else
LPTSTR MyExplainErr(int Func, int err)              
  { 
  if (gs_License.UseCOM())
    return s_ptr->ExplainErr(CKExplainEnum(Func), err);
  return ExplainErr(Func, err);
//return "Explanation";
  };

#endif

dword CSysCADLicense::FixOptions(dword dwOpLevel) 
  { 
  CK_SysCADSecurity Opt;
  Opt.m_OpLevel = dwOpLevel; 

  if (bCOMMineServeOn || FORCEMINESERVE)
    {
    //Opt.m_OpLevel = 0;
    Opt.m_Opts.Client_MineServe = 1;

    }

  if (Opt.m_Opts.Client_MineServe)
    {
    if (bTrialMode)
      Opt.m_OpLevel |= GetTrialOptions();
    if (bDemoMode)
      Opt.m_OpLevel |= GetDemoOptions();

    bLicensed  = 1;
    bTrialMode = 0;
    bDemoMode  = 0;

    Opt.m_Opts.Level            = 1;
    Opt.m_Opts.Ver90            |= 1;
    Opt.m_Opts.Mode_DynamicFlow |= 1;
    Opt.m_Opts.Mode_Electrical  |= 1;
    //Opt.m_Opts.Only_SteadyState |= 1;
    Opt.m_Opts.Func_COM         |= 1;
    Opt.m_Opts.Mdls_Electrical  |= 1;
    Opt.m_Opts.Mdls_HeatBal     |= 1;
    }

  return Opt.m_OpLevel; 

  };

//===========================================================================
//=== Generic Code...

class CStartLicenseDlg : public CDialog
  {
  public:
    CStartLicenseDlg(CWnd* pParent = NULL);   // standard constructor
    //{{AFX_DATA(CStartLicenseDlg)
    enum { IDD = IDD_CK_STARTDLG };
    //}}AFX_DATA
    //{{AFX_VIRTUAL(CStartLicenseDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  protected:
    //{{AFX_MSG(CStartLicenseDlg)
    afx_msg void OnIssue();
    afx_msg void OnTrial();
    virtual BOOL OnInitDialog();
    afx_msg void OnRegtrans();
    afx_msg void OnTransin();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  };

//---------------------------------------------------------------------------

CStartLicenseDlg::CStartLicenseDlg(CWnd* pParent /*=NULL*/)
  : CDialog(CStartLicenseDlg::IDD, pParent)
  {
  //{{AFX_DATA_INIT(CStartLicenseDlg)
  //}}AFX_DATA_INIT
  }

//---------------------------------------------------------------------------

void CStartLicenseDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CStartLicenseDlg)
  //}}AFX_DATA_MAP
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CStartLicenseDlg, CDialog)
  //{{AFX_MSG_MAP(CStartLicenseDlg)
  ON_BN_CLICKED(IDC_CK_ISSUE, OnIssue)
  ON_BN_CLICKED(IDC_CK_TRIAL, OnTrial)
  ON_BN_CLICKED(IDC_CK_REGTRANS, OnRegtrans)
  ON_BN_CLICKED(IDC_CK_TRANSIN, OnTransin)
  //}}AFX_MSG_MAP
  END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CStartLicenseDlg::OnInitDialog() 
  {
  CDialog::OnInitDialog();
  char Buff[256];
  SetDlgItemText(IDC_CK_APP_VERSION, gs_License.GetAppVersion());
  sprintf(Buff, "Issue &Trial License (%d days)", CK_NoOfTrialDays);
  SetDlgItemText(IDC_CK_TRIAL, Buff);
  return TRUE;
  }

//---------------------------------------------------------------------------

void CStartLicenseDlg::OnIssue() 
  {
  if (gs_License.IssueLicense())
    OnOK();
  }

//---------------------------------------------------------------------------

void CStartLicenseDlg::OnTrial() 
  {
  if (gs_License.IssueTrial(CK_NoOfTrialDays, TRUE))
    OnOK();
  }

//---------------------------------------------------------------------------

void CStartLicenseDlg::OnRegtrans() 
  {
  gs_License.DoRegisterTransfer();
  }

//---------------------------------------------------------------------------

void CStartLicenseDlg::OnTransin() 
  {
  if (gs_License.DoTransferIn())
    OnOK();
  }

//===========================================================================

class CLicenseLocationDlg : public CDialog
  {
  public:
    CLicenseLocationDlg(CWnd* pParent = NULL);   // standard constructor
    //{{AFX_DATA(CLicenseLocationDlg)
    enum { IDD = IDD_CK_LOCATIONDLG };
    CString m_AppPath;
    //}}AFX_DATA
    //{{AFX_VIRTUAL(CLicenseLocationDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  protected:
    //{{AFX_MSG(CLicenseLocationDlg)
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    afx_msg void OnBrowse();
    afx_msg void OnLocationOption();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  };

//---------------------------------------------------------------------------

CLicenseLocationDlg::CLicenseLocationDlg(CWnd* pParent /*=NULL*/)
  : CDialog(CLicenseLocationDlg::IDD, pParent)
  {
  //{{AFX_DATA_INIT(CLicenseLocationDlg)
  m_AppPath = _T("");
	//}}AFX_DATA_INIT
  }

//---------------------------------------------------------------------------

void CLicenseLocationDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CLicenseLocationDlg)
  DDX_Text(pDX, IDC_CK_APPPATH, m_AppPath);
	//}}AFX_DATA_MAP
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CLicenseLocationDlg, CDialog)
  //{{AFX_MSG_MAP(CLicenseLocationDlg)
  ON_BN_CLICKED(IDC_CK_BROWSE, OnBrowse)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CLicenseLocationDlg::OnInitDialog() 
  {
  CDialog::OnInitDialog();
  UpdateDialogControls(this, FALSE);
  return TRUE;
  }

//---------------------------------------------------------------------------

void CLicenseLocationDlg::OnBrowse() 
  {
  UpdateData(TRUE);
  char Buff[128];
  sprintf(Buff, "%s (%s)|%s||", CK_AppName, CK_KeyFile, CK_KeyFile);

  CSCDFileDialog Dlg(TRUE, NULL, CK_KeyFile, OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, Buff);
  Dlg.m_ofn.lpstrInitialDir = (const char*)m_AppPath;
  sprintf(Buff, "Find %s with license", CK_AppName);
  Dlg.m_ofn.lpstrTitle = Buff;
  if (Dlg.DoModal()==IDOK)
    {
    CString NewPath = Dlg.GetPathName();
    char Drv[_MAX_DRIVE];
    char Dir[_MAX_DIR];
    _splitpath((const char*)NewPath, Drv, Dir, NULL, NULL);
    m_AppPath = Drv;
    m_AppPath += Dir;
    UpdateData(FALSE);
    }

  }

//---------------------------------------------------------------------------

void CLicenseLocationDlg::OnOK() 
  {
  UpdateData(TRUE);

  CDialog::OnOK();
  }

//---------------------------------------------------------------------------
//===========================================================================

class CAuthLicenseDlg : public CDialog
  {
  public:
    BOOL bValid;

    CAuthLicenseDlg(CWnd* pParent = NULL);   // standard constructor
    //{{AFX_DATA(CAuthLicenseDlg)
    enum { IDD = IDD_CK_AUTHDLG };
    CString m_SiteCode;
    CString m_SiteKey;
    //}}AFX_DATA
    //{{AFX_VIRTUAL(CAuthLicenseDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  protected:
    //{{AFX_MSG(CAuthLicenseDlg)
    virtual void OnOK();
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  };

//---------------------------------------------------------------------------

CAuthLicenseDlg::CAuthLicenseDlg(CWnd* pParent /*=NULL*/)
  : CDialog(CAuthLicenseDlg::IDD, pParent)
  {
  //{{AFX_DATA_INIT(CAuthLicenseDlg)
  m_SiteCode = _T("");
  m_SiteKey = _T("");
  //}}AFX_DATA_INIT
  bValid = 0;
  }

//---------------------------------------------------------------------------

void CAuthLicenseDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CAuthLicenseDlg)
  DDX_Text(pDX, IDC_CK_SITECODE, m_SiteCode);
  DDX_Text(pDX, IDC_CK_SITEKEY, m_SiteKey);
  //}}AFX_DATA_MAP
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CAuthLicenseDlg, CDialog)
  //{{AFX_MSG_MAP(CAuthLicenseDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CAuthLicenseDlg::OnInitDialog() 
  {
  CDialog::OnInitDialog();
  char Buff[1024];
  sprintf(Buff, "Location: %s", gs_License.GetAppPath());
  SetDlgItemText(IDC_CK_TXT_LOCATION, Buff);
  return TRUE;
  }

//---------------------------------------------------------------------------

void CAuthLicenseDlg::OnOK() 
  {
  UpdateData(TRUE);
  bValid = (gs_License.DoIssue((char*)(const char*)m_SiteKey));
  if (bValid)
    CDialog::OnOK();
  }

//===========================================================================

class CTransferDlg : public CDialog
  {
  public:
    CString sTitle;
    CString sTransOption;
    CString sLicCount;
    CString sLicLocation;
    BOOL bUseBrowse;
    CTransferDlg(char* Title, char* TransOption, char* LicCount, BOOL UseBrowse = FALSE, CWnd* pParent = NULL);
    //{{AFX_DATA(CTransferDlg)
    enum { IDD = IDD_CK_TRANSFERDLG };
    CString m_sPath;
    //}}AFX_DATA
    //{{AFX_VIRTUAL(CTransferDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  protected:
    //{{AFX_MSG(CTransferDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnBrowse();
    virtual void OnOK();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  };

//---------------------------------------------------------------------------

CTransferDlg::CTransferDlg(char* Title, char* TransOption, char* LicCount, BOOL UseBrowse /*=FALSE*/, CWnd* pParent /*=NULL*/)
  : CDialog(CTransferDlg::IDD, pParent)
  {
  sTitle = Title;
  sTransOption = TransOption;
  bUseBrowse = UseBrowse;
  sLicCount = LicCount;
  sLicLocation = "Location: ";
  sLicLocation += gs_License.GetAppPath();
  //{{AFX_DATA_INIT(CTransferDlg)
  m_sPath = _T("");
  //}}AFX_DATA_INIT
  }

//---------------------------------------------------------------------------

void CTransferDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CTransferDlg)
  DDX_Text(pDX, IDC_CK_PATH, m_sPath);
  //}}AFX_DATA_MAP
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CTransferDlg, CDialog)
  //{{AFX_MSG_MAP(CTransferDlg)
  ON_BN_CLICKED(IDC_CK_BROWSE, OnBrowse)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CTransferDlg::OnInitDialog() 
  {
  CDialog::OnInitDialog();
  SetDlgItemText(IDC_CK_TXTLOCATION, (const char*)sLicLocation);
  SetDlgItemText(IDC_CK_TXTTRANSFEROPTION, (const char*)sTransOption);
  SetDlgItemText(IDC_CK_TXTCOUNT, (const char*)sLicCount);
  SetWindowText((const char*)sTitle);
  if (!bUseBrowse)
    {
    GetDlgItem(IDC_CK_BROWSE)->EnableWindow(FALSE);
    GetDlgItem(IDC_CK_BROWSE)->ShowWindow(SW_HIDE);
    }
  return TRUE;
  }

//---------------------------------------------------------------------------

void CTransferDlg::OnBrowse() 
  {
  UpdateData(TRUE);
  char Buff[128];
  sprintf(Buff, "%s (%s)|%s||", CK_AppName, CK_KeyFile, CK_KeyFile);

  CSCDFileDialog Dlg(TRUE, NULL, CK_KeyFile, OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, Buff);
  Dlg.m_ofn.lpstrInitialDir = (const char*)m_sPath;
  sprintf(Buff, "Find New %s folder", CK_AppName);
  Dlg.m_ofn.lpstrTitle = Buff;
  if (Dlg.DoModal()==IDOK)
    {
    CString NewPath = Dlg.GetPathName();
    char Drv[_MAX_DRIVE];
    char Dir[_MAX_DIR];
    _splitpath((const char*)NewPath, Drv, Dir, NULL, NULL);
    m_sPath = Drv;
    m_sPath += Dir;
    UpdateData(FALSE);
    }
  }

//---------------------------------------------------------------------------

void CTransferDlg::OnOK() 
  {
  if (bUseBrowse)
    {
    UpdateData(TRUE);
    if (m_sPath.GetLength()>0 && m_sPath[m_sPath.GetLength()]!='\\')
      m_sPath += '\\';
    }
  CDialog::OnOK();
  }

//===========================================================================
//
//
//
//===========================================================================
                     
CLicense::CLicense()
  {
  bDidInitCrypkey = 0;
  bMultiUserFailure = 0;
  bLicensed = 0;
  bDemoMode = 0;
  bTrialMode = 0;
  bTrialFailed = 0;
  bCOMMineServeOn = 0;
  sLastPath = "A:\\";
  sAppPath = "";
  dwOpLevel = 0;
  iDaysLeft = 0;
  m_bUseCOM= 0;
  }

//---------------------------------------------------------------------------

CLicense::~CLicense()
  {
  Exit();
  }

//---------------------------------------------------------------------------

BOOL CLicense::Init(char* path /*=NULL*/)
  {
  CWaitCursor Wait;
#if !BYPASSLICENSING
  if (bDidInitCrypkey)
    {
    if (m_bUseCOM)
      {
      s_ptr->EndCrypKey();
      s_ptr.Release();
      }
    else
      EndCrypkey();
    }
#endif
  bDidInitCrypkey = 0;
  bMultiUserFailure = 0;
  OSVERSIONINFO VI;
  VI.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  BOOL IsWIN32 = (GetVersionEx(&VI) && (VI.dwPlatformId==VER_PLATFORM_WIN32_NT));
  /*if (IsWIN32 && CheckService()!=0)
    {
    Error("Error with CrypKey Licensing Service!\n\nPerhaps not installed or not running?");
    return FALSE;
    }*/

  if (path==NULL)
    path = GetAppPath(); //another chance to get another path!
  char LongPath[_MAX_PATH],ShortPath[_MAX_PATH];
  if (path==NULL)
    {
    ASSERT_RDB(path!=NULL, "Do not expect to get here!");
    if (GetModuleFileName(NULL, LongPath, sizeof(LongPath))<1)
      return FALSE;
    }
  else
    {
    strcpy(LongPath, path);
    if (strlen(LongPath)==0 || LongPath[strlen(LongPath)-1]!='\\')
      strcat(LongPath, "\\");
    strcat(LongPath, CK_KeyFile);
    }
  if (!FileExists(LongPath))
    {
    Error("Specified license file (or folder) not found!\n\n%s\n\nCheck license location!", LongPath);
    return FALSE;
    }
  if (GetShortPathName(LongPath, ShortPath, sizeof(ShortPath))<1)
    return FALSE;
#if !BYPASSLICENSING
  int err = -1;
  if (m_bUseCOM)
    {
    s_ptr.CreateInstance ("CrypKey.SDK");
    err = s_ptr->InitCrypKey(ShortPath, CK_MASTER_KEY, CK_USER_KEY, FALSE, CK_NetworkChecktime);
    }
  else
    err = InitCrypkey(ShortPath, CK_MASTER_KEY, CK_USER_KEY, FALSE, CK_NetworkChecktime);

  if (err==-209)
    {
    Error("Missing license file CRP32001.NGN from application folder.");
    return FALSE;
    }

#if CK_SCDCOMLIC
  if (err==CKGeneralNetworkNoCKServe)
#else
  if (err==NETWORK_NO_CKSERVE)
#endif
    {
    if (IsWIN32)
      {//do this on Windows NT and Windows 2000...
      if (UpdateCrypkeyINI(LongPath))
        {//try again...
        if (m_bUseCOM)
          err = s_ptr->InitCrypKey(ShortPath, CK_MASTER_KEY, CK_USER_KEY, FALSE, CK_NetworkChecktime);
        else
          err = InitCrypkey(ShortPath, CK_MASTER_KEY, CK_USER_KEY, FALSE, CK_NetworkChecktime);
        if (err)
          {
          Error("Initialization Failure %d after Update INI\n%s for %s", err, MyExplainErr(EXP_INIT_ERR, err), LongPath);
          return FALSE;
          }
        }
      }
    }
  if (err)
    {
    if (err==NETWORK_NO_CKSERVE)
      Error("Unable to locate license for %s", LongPath);
    else
      {
      if (err==INIT_THUNK_LIB_NOT_FOUND) //win 95/98 error
        Error("Unable to find DLLs for licensing!\n\n'cryp95e.dll' and/or 'crp9516e.dll'");
      else
        Error("Initialization Failure %d\n%s for %s", err, MyExplainErr(EXP_INIT_ERR, err), LongPath);
      }
    return FALSE;
    }
  int Ver;
  if (m_bUseCOM)
    Ver = s_ptr->CrypKeyVersion();
  else
    Ver = CrypkeyVersion();
  if (Ver==41 || Ver==42)
    {//version 4.1 and 4.2 are old and not supported...
    Error("Old Crypkey version (%d) not supported", Ver);
    return FALSE;
    }
  else if (Ver==43)
    {//version 4.3 is old and not supported...
    Error("Old Crypkey version (%d) not supported", Ver);
    return FALSE;
    }
  else if (Ver==50) //Ver 5.0 : Sept 1999
    {//version 5.0 is old but should work...
    //Error("Old Crypkey version (%d)", Ver);
    //return FALSE;
    }
  else if (Ver==52) //Ver 5.2 : August 2000
    {//version 5.2 should work...
    //Error("Old Crypkey version (%d)", Ver);
    //return FALSE;
    }
  else if (Ver==55) //Ver 5.5 : November 2000
    {//version 5.5 should work...
    //Error("Old Crypkey version (%d)", Ver);
    //return FALSE;
    }
  else if (Ver==56) //Ver 5.6 : July 2001
    {//version 5.6 should work...
    //Error("Old Crypkey version (%d)", Ver);
    //return FALSE;
    }
  else if (Ver==57) //Ver 5.7 : December 2002
    {//version 5.7 should work...
    //Error("Old Crypkey version (%d)", Ver);
    //return FALSE;
    }
  else if (Ver==60) //Ver 6.0 : August 2003
    {//version 6.0 should work...
    //Error("Old Crypkey version (%d)", Ver);
    //return FALSE;
    }
  else if (Ver!=61) //Ver 6.1 : September 2003
    {//expect version 6.1
    Error("Incorrect Crypkey version (%d)", Ver);
    return FALSE;
    }
#endif
  bDidInitCrypkey = 1;
  return TRUE;
  }

//---------------------------------------------------------------------------

void CLicense::Exit()
  {
#if !BYPASSLICENSING
  if (bDidInitCrypkey)
    {
    if (m_bUseCOM)
      {
      s_ptr->EndCrypKey();
      s_ptr.Release();
      }
    else
      EndCrypkey();
    }
#endif
  bDidInitCrypkey = 0;
  }

//---------------------------------------------------------------------------

BOOL CLicense::Check(BOOL Prompt /*=FALSE*/)
  {
  ASSERT(bDidInitCrypkey);
  CWaitCursor Wait;
  bLicensed = 0;
  bMultiUserFailure = 0;
  dwOpLevel = 0;
  iDaysLeft = 0;

//#if CK_LICENSINGON

  int err;
  if (m_bUseCOM)
    err = s_ptr->GetAuthorization((long*)&dwOpLevel, 0); //check authorization, use up 0 runs
  else
    err = GetAuthorization(&dwOpLevel, 0); //check authorization, use up 0 runs
  
  //Use the challenge function to check the library is not an impostor
  //This only needs to be done if you are using the DLL
  //if (b
  if (err==0) //check this only if we think we are authorized
    {
#if !BYPASSLICENSING
    //generate some random numbers - this can be done any way you like
    //random1 = time(NULL);
    //random2 = random1*time(NULL);
    ULONG random1 = (time(NULL)<<2)*3;
    ULONG random2 = random1*time(NULL)*rand();
    ULONG result1 = Challenge32(CK_COMPANYNUM, CK_PASSNUM/2, random1, random2);
    ULONG result2;

    if (m_bUseCOM)
      result2 = s_ptr->CKChallenge32(random1, random2);
    else
      result2 = CKChallenge32(random1, random2);
    if (result1 != result2)
      {
      Error("Security Failure.  Challenge function failed");
      return FALSE;
      }
#endif
    bLicensed = 1;
    DWORD dw = (DWORD)pow(2.0, CK_NumDefinedLevels) - 1;
    bTrialMode = ((dwOpLevel & dw)==CK_TrialLevel);
    iDaysLeft = (Get1RestInfo(1)==0) ? CK_InfiniteDays : Get1RestInfo(2) - Get1RestInfo(3);
    bDemoMode = 0;
    ScdPFMachine.WrInt(LicINISection, "InDemoMode", bDemoMode);
    CheckForLiteModes();
    return TRUE;
    }
  else
    {
    dwOpLevel = 0;
    if (err>0)
      {
      bMultiUserFailure = 1;
      char Buff[2048];
      NetworkUsersInfo(Buff);
      //Error("More than %d users already using this license.\n\nNeed to wait for %d user%s to quit!\n\n%s", GetNumMultiUsers(), err, (err>1 ? "s" : ""), Buff);
      Error("More than %d users already using this license!\n\n%s", GetNumMultiUsers(), Buff);
      }
    if (Prompt && err!=AUTH_NOT_PRESENT && err<0)
      Error("Get Authorization failed!\nReturned %d : %s", err, MyExplainErr(EXP_AUTH_ERR, err));
    }
  CheckForLiteModes();
  dwOpLevel=FixOptions(dwOpLevel);

//#else
//
//  bLicensed = 1;
//  bTrialMode = 0;
//  iDaysLeft = CK_InfiniteDays ;
//  bDemoMode = 0;
//  ScdPFMachine.WrInt(LicINISection, "InDemoMode", bDemoMode);
//  CheckForLiteModes();
//
//#endif

  return FALSE;
  }

//---------------------------------------------------------------------------

BOOL CLicense::QuickCheck(byte CheckLevel/*=0*/)
  {
  ASSERT(bDidInitCrypkey);
  if (bBlocked)
    return FALSE;
  if (CheckLevel==0)
    {
    if (!bDemoMode && !bLicensed)
      {
      SetBlocked();
      Error("Security Failure.  License blocked\n\nThe majority of SysCAD commands and functions have been disabled.\n\nPlease exit SysCAD. (Save project if required)");
      AfxGetMainWnd()->PostMessage(WMU_UPDATEMAINWND, SUB_UPDMAIN_BACKGROUND, 0);
      return FALSE;
      }
    return TRUE;
    }

  if (bDemoMode)
    return TRUE;

  CWaitCursor Wait;
  bLicensed = 0;
  dwOpLevel = 0;
  int err = GetAuthorization(&dwOpLevel, 0); //check authorization, use up 0 runs
  int OtherErr = 0;

#if !BYPASSLICENSING
  if (err==0 && CheckLevel>1)
    {
    //generate some random numbers - this can be done any way you like
    ULONG random1 = (time(NULL)<<1)+9;
    ULONG random2 = random1+rand();
    ULONG result1 = Challenge32(CK_COMPANYNUM, CK_PASSNUM/2, random1, random2);
    ULONG result2;
    if (m_bUseCOM)
      result2 = s_ptr->CKChallenge32(random1, random2);
    else
      result2 = CKChallenge32(random1, random2);

    if (result1 != result2)
      {
      Error("Security Failure.  Challenge function failed");
      OtherErr = 1;
      }
    }

  if (err==AUTH_INIT_FAIL)
    {//this may occur on a Windows 95 PC due to a bug with crypkey ???; try again ...
    if (Init())
      {
      if (Check(TRUE))
        OtherErr = 0;
      else
        OtherErr = 2;
      }
    else
      OtherErr = 3;
    }
#endif

  if (err==0 && OtherErr==0)
    bLicensed = 1;
  else
    {
    char Buff[1024];
    dwOpLevel = 0;
    SetBlocked();
    if (OtherErr==0)
      sprintf(Buff, "Security Failure.  License blocked\n\nReturned %d : %s", err, MyExplainErr(EXP_AUTH_ERR, err));
    else
      sprintf(Buff, "Security Failure.  License blocked (%d)", OtherErr);
    strcat(Buff, "\n\nThe majority of SysCAD commands and functions have been disabled.\n\nPlease exit SysCAD. (Save project if required)");
    Error(Buff);
    AfxGetMainWnd()->PostMessage(WMU_UPDATEMAINWND, SUB_UPDMAIN_BACKGROUND, 0);
    return FALSE;
    }
  dwOpLevel=FixOptions(dwOpLevel);

  //#else
//  bLicensed = 1;
//  dwOpLevel = 0;
//#endif
  return TRUE;
  }

//---------------------------------------------------------------------------

int CLicense::Copies()
  {
  return GetNumCopies();
  }

//---------------------------------------------------------------------------

int CLicense::MultiUsers()
  {
  return GetNumMultiUsers();
  }

//---------------------------------------------------------------------------

BOOL CLicense::StartDialog()
  {
  CStartLicenseDlg Dlg;
  if (Dlg.DoModal()!=IDOK)
    return FALSE;
  return TRUE;
  }

//---------------------------------------------------------------------------

BOOL CLicense::IssueTrial(int NoOfDays, BOOL Prompt)
  {
#if !BYPASSLICENSING
  if (Prompt)
    {
    char Buff[256];
    sprintf(Buff, "This will attempt to issue a trial license for %d days\nThis can only be done once - Are you sure?", NoOfDays);
    if (AfxMessageBox(Buff, MB_YESNO|MB_ICONSTOP|MB_DEFBUTTON2)==IDNO)
      return FALSE;
    }
  CWaitCursor Wait;
  DWORD OpLevel = FixOptions(GetTrialOptions());
  int ret = readyToTryDays(OpLevel, NoOfDays, CK_TrialVerNo, 1);
  if (ret)
    {
    Error("Issue Trial License failed!\nReturned %d : %s", ret, MyExplainErr(EXP_RTT_ERR, ret));
    bTrialFailed = 1;
    return FALSE;
    }
#endif
  Check(); //re-check options etc
  return TRUE;
  }

//---------------------------------------------------------------------------

int CLicense::SetLocation(BOOL CheckAndInit/*=true*/)
  {
  int RetCode = 0;
  CLicenseLocationDlg Dlg;
  Dlg.m_AppPath = sAppPath;
  if (Dlg.DoModal()==IDOK)
    {
    if (_stricmp((const char*)sAppPath, (const char*)Dlg.m_AppPath)!=0)
      {
      CString PrevAppPath = sAppPath;
      sAppPath = Dlg.m_AppPath;
      if (CheckAndInit)
        {
        //check license on new location...
        CWaitCursor Wait;
        BOOL Failed = FALSE;
        if (!Init())
          Failed = TRUE;
        if (!Failed && !Check(TRUE)) //re-check options etc
          Failed = TRUE;
        if (Failed)
          {
          char Buff[1024];
          sprintf(Buff, "Set license to new location anyway?\n\nOld location : %s\nNew location : %s", (const char*)PrevAppPath, (const char*)sAppPath);
          if (AfxMessageBox(Buff, MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2)!=IDYES)
            {
            Wait.Restore();
            sAppPath = PrevAppPath;
#if !BYPASSLICENSING
            if (m_bUseCOM)
              {
              s_ptr->EndCrypKey();
              s_ptr.Release();
              }
            else
              EndCrypkey();
#endif
            bDidInitCrypkey = 0;
            if (!Init())
              Error("Change location\nFailed to re-initialize license!");
            else if (!Check(TRUE)) //re-check options etc
              Error("Change location\nFailed to re-initialize license!");
            AfxGetMainWnd()->PostMessage(WMU_UPDATEMAINWND, SUB_UPDMAIN_BACKGROUND, 0);
            return 2; //new location failed
            }
          else
            {
            #if CK_ALLOWDEMOMODE
            gs_License.SetDemoMode();
            #endif
            RetCode = 4; //new location failed - but set it anyway
            }
          }
        else
          RetCode = 1; //new location OK
        }
      else
        RetCode = 5; //location changed, no Init and check
      ScdPFMachine.WrStr(LicINISection, "LicenseLocation", (const char*)sAppPath);
      AfxGetMainWnd()->PostMessage(WMU_UPDATEMAINWND, SUB_UPDMAIN_BACKGROUND, 0);
      }
    else
      RetCode = 3; //unchanged
    }
  return RetCode; //cancel
  }


//---------------------------------------------------------------------------

void CLicense::SetUseCOM(BOOL On)
  {
  if (!bDidInitCrypkey)
    {
    m_bUseCOM=false;
    if (On)
      {
      //CString Path = GetAppPath();
      //????????


      m_bUseCOM=true;
      }
    }
  }

//---------------------------------------------------------------------------

BOOL CLicense::IssueLicense()
  {
  CAuthLicenseDlg Dlg;
#if !BYPASSLICENSING
  int err;
  if (m_bUseCOM)
    {
    _bstr_t SiteCode;
    err=s_ptr->GetSiteCode(&SiteCode.GetBSTR());
    Dlg.m_SiteCode = (LPCTSTR)SiteCode;
    }
  else
    {
    char SiteCode[64];
    err=GetSiteCode(SiteCode);
    Dlg.m_SiteCode = SiteCode;
    }
  if (err) //let the user authorize
    {
    Error("Get site code failed!\nReturned %d : %s", err, MyExplainErr(EXP_GET_SITECODE_ERR, err));
    return FALSE;
    }

  Dlg.DoModal();
  if (Dlg.bValid)
    {
//    CWinApp* pApp = AfxGetApp();
//    if (pApp)
    ScdPFMachine.WrStr(LicINISection, "LastSiteCode", Dlg.m_SiteCode);
    Check(); //re-check options etc
    return TRUE;
    }
  return FALSE;
#else
  return TRUE;
#endif
  }

//---------------------------------------------------------------------------

BOOL CLicense::DoIssue(char* key)
  {
  if (key==NULL || strlen(key)==0)
    {
    Error("Valid Site Key required");
    return FALSE;
    }
  char Buff[1024];
  sprintf(Buff, "Issue or update license at:\n\n%s", gs_License.GetAppPath());
  if (AfxMessageBox(Buff, MB_YESNO|MB_ICONSTOP|MB_DEFBUTTON2)!=IDYES)
    return FALSE;

  CWaitCursor Wait;
#if !BYPASSLICENSING
  int err;
  if (m_bUseCOM)
    err = s_ptr->SaveSiteKey(key);
  else
    err = SaveSiteKey(key);
  switch (err)
    {
    case AUTH_OK: // Valid Key
      {//Site Key Accepted
      ScdPFMachine.WrStr(LicINISection, "LastSiteKey", key);
      return TRUE;
      }
    case SITE_KEY_ENTRY_CHECK_FAIL: // Likely tried to reuse an old key
      Error("Site Key does not match most recent site code");
      break;
    case SITE_KEY_ENTRY_CRC_FAIL: // mistake in the key
      Error("Site Key CRC Failure - Likely a misstype");
      break;
    }
  return FALSE;
#else
  return TRUE;
#endif
  }

//---------------------------------------------------------------------------

BOOL CLicense::DoRegisterTransfer()
  {
#if !BYPASSLICENSING
  CTransferDlg Dlg("Register Transfer", "Enter path to place registration file:", "");
  Dlg.m_sPath = sLastPath;
  if (Dlg.DoModal()==IDOK)
    {
    CWaitCursor Wait;
    sLastPath = Dlg.m_sPath;
    int err;
    if (m_bUseCOM)
      err = s_ptr->RegisterTransfer((char*)(const char*)sLastPath);
    else
      err = RegisterTransfer((char*)(const char*)sLastPath);
    if (err==0)
      return TRUE;
    Error("Register Transfer failed!\n%d : %s", err, MyExplainErr(EXP_REG_ERR, err));
    }
  return FALSE;
#else
  return TRUE;
#endif
  }

//---------------------------------------------------------------------------

BOOL CLicense::DoTransferOut()
  {
#if !BYPASSLICENSING
  char Buff[256];
  sprintf(Buff, "Number of Copies allowed from this site : %d", GetNumCopies());
  CTransferDlg Dlg("Transfer Out", "Enter path find registration file:", Buff);
  Dlg.m_sPath = sLastPath;
  if (Dlg.DoModal()==IDOK)
    {
    CWaitCursor Wait;
    sLastPath = Dlg.m_sPath;
    int err;
    if (m_bUseCOM)
      err = s_ptr->TransferOut((char*)(const char*)sLastPath);
    else
      err = TransferOut((char*)(const char*)sLastPath);
    if (err==0)
      {
      Check(); //re-check options etc
      return TRUE;
      }
    Error("Transfer Out failed!\n%d : %s", err, MyExplainErr(EXP_TO_ERR, err));
    }
  return FALSE;
#else
  return TRUE;
#endif
  }

//---------------------------------------------------------------------------

BOOL CLicense::DoTransferIn()
  {
#if !BYPASSLICENSING
  char Buff[256];
  Buff[0] = 0;
  if (bLicensed)
    sprintf(Buff, "Number of Copies allowed from this site : %d", GetNumCopies());
  CTransferDlg Dlg("Transfer In", "Enter path find transfer files:", Buff);
  Dlg.m_sPath = sLastPath;
  if (Dlg.DoModal()==IDOK)
    {
    CWaitCursor Wait;
    sLastPath = Dlg.m_sPath;
    int err;
    if (m_bUseCOM)
      err = s_ptr->TransferIn((char*)(const char*)sLastPath);
    else
      err = TransferIn((char*)(const char*)sLastPath);
    if (err==0)
      {
      Check(); //re-check options etc
      return TRUE;
      }
    Error("Transfer In failed!\n%d : %s", err, MyExplainErr(EXP_TI_ERR, err));
    }
  return FALSE;
#else
  return TRUE;
#endif
  }

//---------------------------------------------------------------------------

BOOL CLicense::DoDirectTransfer()
  {
#if !BYPASSLICENSING
  char Buff[512];
  sprintf(Buff, "Enter the path of the copy of %s to transfer license to:", CK_AppName);
  char Buff1[256];
  sprintf(Buff1, "Number of Copies allowed from this site : %d", GetNumCopies());
  CTransferDlg Dlg("Direct Transfer", Buff, Buff1, TRUE);
  char LongPath[_MAX_PATH];
  char Drv[_MAX_DRIVE];
  char Dir[_MAX_DIR];
  strcpy(LongPath, gs_License.GetAppPath());
  _splitpath(LongPath, Drv, Dir, NULL, NULL);
  char Path[_MAX_PATH];
  sprintf(Path, "%s%s", Drv, Dir);
  Dlg.m_sPath = Path;
  if (Dlg.DoModal()==IDOK)
    {
    if (_stricmp(Path, (const char*)Dlg.m_sPath)==0)
      {
      Error("Direct Transfer\nCannot transfer license onto itself!");
      return FALSE;
      }
    CWaitCursor Wait;
    Exit();
    BOOL WillOverwrite = FALSE;
    BOOL DoTrans = TRUE;
    if (Init((char*)(const char*)Dlg.m_sPath))
      {
      if (Check())
        WillOverwrite = TRUE;
      Exit();
      }
    if (WillOverwrite)
      {
      sprintf(Buff, "Your destination %s folder already has authorization to run.\n", CK_AppName);
      strcat(Buff, "If you complete this tranfer process your existing license on the\n");
      strcat(Buff, "distination folder will be overwritten.   Continue?");
      if (AfxMessageBox(Buff, MB_YESNO|MB_ICONSTOP|MB_DEFBUTTON2)!=IDYES)
        DoTrans = FALSE;
      }

    //reinstall license back to us
    if (m_bUseCOM)
      {
      s_ptr->EndCrypKey();
      s_ptr.Release();
      }
    else
      EndCrypkey();
    bDidInitCrypkey = 0;
    if (!Init())
      {
      Error("Direct Transfer\nFailed to re-initialize license!");
      return FALSE;
      }
    if (!Check(TRUE)) //re-check options etc
      {
      Error("Direct Transfer\nFailed to re-initialize license!");
      return FALSE;
      }
    if (!DoTrans)
      return FALSE;
    int err;
    if (m_bUseCOM)
      err = s_ptr->DirectTransfer((char*)(const char*)Dlg.m_sPath);
    else
      err = DirectTransfer((char*)(const char*)Dlg.m_sPath);
    if (err==0)
      {
      Check(); //re-check options etc
      return TRUE;
      }
    Error("Direct Transfer failed!\n%d : %s", err, MyExplainErr(EXP_DT_ERR, err));
    }
  return FALSE;
#else
  return TRUE;
#endif
  }

//---------------------------------------------------------------------------

BOOL CLicense::Kill(CString& ConfirmCode)
  {
#if !BYPASSLICENSING
  if (AfxMessageBox("This will delete your license - Are you sure?", MB_YESNO|MB_ICONSTOP|MB_DEFBUTTON2)==IDYES)
    {
    CWaitCursor Wait;
    ConfirmCode = "";
    CString s;
    int ret;
    if (m_bUseCOM)
      {
      _bstr_t ss;
      ret = s_ptr->KillLicense(&ss.GetBSTR());
      s = (LPCTSTR)ss;
      }
    else
      {
      char ss[64];
      ret = KillLicense(ss);
      s = ss;
      }
    if (ret==0)
      {
      ConfirmCode = s;
      ScdPFMachine.WrStr(LicINISection, "LastKillCode", s);
      Check(); //re-check options etc
      return TRUE;
      }
    else
      Error("Kill License failed!\n%d : ", ret, MyExplainErr(EXP_KL_ERR, ret));
    }
  return FALSE;
#else
  return TRUE;
#endif
  }

//---------------------------------------------------------------------------

void CLicense::SetDemoMode()
  {
  if (!bDemoMode)
    {
    bDemoMode = 1;
    /*CWinApp* pApp = AfxGetApp();
    if (pApp)
      {
      //if (ScdPFMachine.RdInt(LicINISection, "InDemoMode", 1)==0)
      //  Error("%s no longer licensed. Now in demo mode.", CK_AppName);
      ScdPFMachine.RdInt(LicINISection, "InDemoMode", bDemoMode);
      }*/
    }
  dwOpLevel = FixOptions(GetDemoOptions());
  }

//---------------------------------------------------------------------------

void CLicense::Error(char * fmt, ...)
  {
  char Buff[2048];
  va_list argptr;
  va_start(argptr, fmt);
  vsprintf(Buff, fmt, argptr);
  va_end(argptr);
  AfxMessageBox(Buff);
  }

//---------------------------------------------------------------------------

/**************************************************************************
 *FUNCTION: challenge32()
 *PURPOSE : does the calculation for the CrypKey ckChallenge32 verification
 *ARGUMENT: companyNum - assigned by Kenonic
            passNumDiv2- assigned by Kenonic
            random1    - make one up yourself
            random2    - make one up yourself
 *RETURNS : non zero if error
 *NOTES:    This function uses longs, not unsigned longs, to be VB friendly
            If unsigned longs are accidently used, it will not work
 **************************************************************************/
long CLicense::Challenge32(long companyNum, long passNumDiv2, long random1, long random2)
  {
  long ret = 1;
  for (int i=2; i<11; i++)
    {
    ret = ret%32769 * ( (random1/i)%32769 + (companyNum/i)%32769);
    ret = ret%32769 * ( (random2/i)%32769 + (passNumDiv2/i)%32769);
    }

  return ret;
  }
/*UINT CLicense::Challenge(ULONG companyNum, ULONG passNum, UINT random1, UINT random2)
  {
  ULONG r1 = (ULONG)random1;
  ULONG r2 = (ULONG)random2;
  ULONG ret = 0;

  for (int i=0; i<11; i++)
    ret = (ret * r1 + companyNum)%16381L + (ret * r2 + passNum)%16369L;

  return (UINT)ret;
  }*/

//---------------------------------------------------------------------------

#define SERVICE_RESYNC_THREADS 128+5

int CLicense::CheckService()
  {
  CWaitCursor Wait;
  SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
  if (schSCManager == NULL)
    return -1;
  SC_HANDLE schService = OpenService(schSCManager, "Crypkey License", SERVICE_ALL_ACCESS);
  if (schService == NULL)
    {
    CloseServiceHandle(schSCManager);
    return -2;
    }
  int rtn = 0;
  SERVICE_STATUS servicestat;
  if (!ControlService(schService, SERVICE_CONTROL_INTERROGATE, &servicestat))
    {
    rtn = GetLastError();
    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
    return -3;
    }
  CloseServiceHandle(schService);
  CloseServiceHandle(schSCManager);
  return (servicestat.dwCurrentState==SERVICE_RUNNING) ? 0 : -4;
  }

//---------------------------------------------------------------------------

BOOL CLicense::ResetService(BOOL StopAndStart)
  {
  CWaitCursor Wait;
  int rtn = 0;
  //Force CrypKey licence service to re-read crypkey.ini
  SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
  if (schSCManager == NULL)
    {
    rtn = GetLastError();
    if (rtn==5)
      Error("User does not have permissions to access system Services\nOpenSCManager failed! Error:%d", rtn);
    else
      Error("OpenSCManager failed! Error:%d", rtn);
    return FALSE;
    }
  
  SC_HANDLE schService;
  SERVICE_STATUS servicestat;
  if (StopAndStart)
    {
    schService = OpenService(schSCManager, "Crypkey License", SERVICE_ALL_ACCESS);
    if (schService == NULL)
      {
      rtn = GetLastError();
      Error("OpenService 'Crypkey License' failed! Error:%d", rtn);
      CloseServiceHandle(schSCManager);
      return FALSE;
      }

    if (!ControlService(schService, SERVICE_CONTROL_STOP, &servicestat))
      {
      rtn = GetLastError();
      Error("ControlService 'Stop' failed! Error:%d", rtn);
      //CloseServiceHandle(schService);
      //CloseServiceHandle(schSCManager);
      //return FALSE;
      }

    int Cnt = 12;
    while (Cnt>0)
      {
      Sleep(250);
      rtn = 0;
      if (!ControlService(schService, SERVICE_CONTROL_INTERROGATE, &servicestat))
        {
        rtn = GetLastError();
        //Error("ControlService 'Interrogate' failed! Error:%d", rtn);
        //CloseServiceHandle(schService);
        //CloseServiceHandle(schSCManager);
        //return FALSE;
        }
      if (rtn==0 && servicestat.dwCurrentState==SERVICE_STOPPED)
        Cnt = 0;
      Cnt--;
      }

    if (!StartService(schService, 0, NULL))
      {
      rtn = GetLastError();
      Error("StartService failed! Error:%d", rtn);
      //CloseServiceHandle(schService);
      //CloseServiceHandle(schSCManager);
      //return FALSE;
      }

    Cnt = 12;
    while (Cnt>0)
      {
      Sleep(250);
      rtn = 0;
      if (!ControlService(schService, SERVICE_CONTROL_INTERROGATE, &servicestat))
        {
        rtn = GetLastError();
        //Error("ControlService 'Interrogate' failed! Error:%d", rtn);
        //CloseServiceHandle(schService);
        //CloseServiceHandle(schSCManager);
        //return FALSE;
        }
      if (rtn==0 && servicestat.dwCurrentState==SERVICE_RUNNING)
        Cnt = 0;
      Cnt--;
      }

    CloseServiceHandle(schService);
    }

  schService = OpenService(schSCManager, "Crypkey License", SERVICE_ALL_ACCESS);
  if (schService == NULL)
    {
    rtn = GetLastError();
    Error("OpenService 'Crypkey License' failed! Error:%d", rtn);
    CloseServiceHandle(schSCManager);
    return FALSE;
    }

  if (!ControlService(schService, SERVICE_RESYNC_THREADS, &servicestat))
    {
    rtn = GetLastError();
    Error("ControlService 'Resync Threads' failed! Error:%d", rtn);
    }
  CloseServiceHandle(schService);
  CloseServiceHandle(schSCManager);
  return (rtn==0);
  }

//---------------------------------------------------------------------------

BOOL CLicense::UpdateCrypkeyINI(char* path)
  {
  //This is only needed on Windows NT / Windows 2000 machines; 
  //It effectivly does what 'CKCONFIG.EXE' does, but the user 
  //doesn't have to do it themselves!
  
  //find crypkey.ini ourselves...
  char Buff[_MAX_PATH];
  DWORD len = GetEnvironmentVariable("WINDIR", Buff, sizeof(Buff));
  if (len==0)
    {
    Error("Environment Variable WINDIR not found.");
    return FALSE;
    }
  if (Buff[strlen(Buff)-1]=='\\')
    Buff[strlen(Buff)-1] = 0;
  strcat(Buff, "\\crypkey.ini");
  if (!FileExists(Buff))
    {//ensure ini file exists...
    WritePrivateProfileString("Watch", "dir0", "xXx", Buff);
    }
  
  //add path to crypkey.ini (if required)...
  char Drv[_MAX_DRIVE];
  char Dir[_MAX_DIR];
  _splitpath(path, Drv, Dir, NULL, NULL);
  char PathBuff[_MAX_PATH];
  sprintf(PathBuff, "%s%s", Drv, Dir);
  if (PathBuff[strlen(PathBuff)-1]=='\\')
    PathBuff[strlen(PathBuff)-1] = 0;
  const int PathBuffLen = strlen(PathBuff);
  int i=0;
  char Buff1[_MAX_PATH];
  char Key[16];
  char NotFoundKey[16];
  BOOL Done = FALSE;
  int NotFoundCnt = 0;
  while (!Done)
    {
    sprintf(Key, "dir%d", i);
    int len = GetPrivateProfileString("Watch", Key, "xXx", Buff1, sizeof(Buff1), Buff);
    if (len==0)
      {
      Error("Unable to alter file '%s'", Buff);
      return FALSE;
      }
    if (len==3 && strcmp(Buff1, "xXx")==0)
      {
      if (NotFoundCnt++==0)
        strcpy(NotFoundKey, Key);
      if (NotFoundCnt>3)
        {
        WritePrivateProfileString("Watch", NotFoundKey, PathBuff, Buff);
        Done = TRUE;
        char bbb[1024];
        sprintf(bbb, "Updated file '%s' entry '%s' set to:\n%s", Buff, NotFoundKey, PathBuff);
        AfxMessageBox(bbb);
        }
      }
    else if (len==PathBuffLen && _strnicmp(PathBuff, Buff1, PathBuffLen)==0)
      {//path is allready in INI file!
      //return FALSE;
      Done = TRUE; //Force service to restart anyway...
      }
    i++;
    }

  return ResetService(false);
  }

//---------------------------------------------------------------------------

BOOL CLicense::NetworkUsersInfo()
  {
#if !BYPASSLICENSING
  FLS_REC fls[64];
  int Cnt = 0;
  int err;
  if (m_bUseCOM)
    {
    _asm int 3;
    //err = s_ptr->FloatingLicenseTakeSnapshot((sizeof(fls), &Cnt, &fls[0]);
    }
  else
    err = FloatingLicenseSnapshot(sizeof(fls), &Cnt, &fls[0]);
  if (err<0)
    {
    Error("Get Floating License Snapshot Failure %d\n%s", err, MyExplainErr(EXP_INIT_ERR, err));
    return FALSE;
    }
  char Buff[2048];
  Buff[0] = 0;
  for (int i=0; i<Cnt; i++)
    sprintf(Buff, "%s%s: %s  %s   %s\n", Buff, fls[i].status==0 ? "OK" : "Blocked", fls[i].userName, fls[i].computerName, SGTime(fls[i].starttime));
  AfxMessageBox(Buff);
#endif
  return TRUE;
  }

//---------------------------------------------------------------------------

BOOL CLicense::NetworkUsersInfo(char* Buff)
  {
#if !BYPASSLICENSING
  Buff[0] = 0;
  FLS_REC fls[64];
  int Cnt = 0;
  int err;
  if (m_bUseCOM)
    {
    _asm int 3;
    //err = s_ptr->FloatingLicenseSnapshot(sizeof(fls), &Cnt, &fls[0]);
    }
  else
    err = FloatingLicenseSnapshot(sizeof(fls), &Cnt, &fls[0]);
  if (err<0)
    {
    //Error("Get Floating License Snapshot Failure %d\n%s", err, MyExplainErr(EXP_INIT_ERR, err));
    return FALSE;
    }
  for (int i=0; i<Cnt; i++)
    sprintf(Buff, "%s%s: %s  %s   %s\n", Buff, fls[i].status==0 ? "OK" : "Blocked", fls[i].userName, fls[i].computerName, SGTime(fls[i].starttime));
#endif
  return TRUE;
  }

//---------------------------------------------------------------------------

char* CLicense::SGTime(ULONG ltime) //ltime - seconds since jan 1, 1970
  {//converts ULONG in t time to a yy-mm-dd hh:mm string
  static char time_str[64];
  struct tm* t;

  #ifdef WIN32
  ltime -= 2209075200; // normilize this number to 1/1/1970
  #endif

  t = gmtime((const time_t*)&ltime);
  if (t!=NULL)
    sprintf(time_str, "%02d-%02d-%02d %02d:%02d", t->tm_year%100, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min);
  else
    strcpy(time_str, "predates 70-01-01 00:00");
  return time_str;
  }

//---------------------------------------------------------------------------

void CLicense::Info()
  {
  CWaitCursor Wait;
  //char* authTypes[] = {"NONE", "TIME(DAYS)", "RUNS"};
  char Buff[4096];
  strcpy(Buff,"License information:\n\n");
  sprintf(Buff, "%s  Number of Copies allowed from this site : %d\n", Buff, GetNumCopies());
  //sprintf(Buff, "%s  Number of Network Users allowed from this site : %d\n", Buff, GetNumMultiUsers());
  //sprintf(Buff, "%s  Site Code : %s\n", Buff, GetSiteCode2());

//NBNB TODO CHECK THIS LEVEL !!!!

  sprintf(Buff, "%s  Level : %d\n", Buff, GetLevel(CK_NumDefinedLevels));
  /*sprintf(Buff, "%s  Level : (1) %d\n", Buff, get_level(1));
  sprintf(Buff, "%s  Level : (2) %d\n", Buff, get_level(2));
  sprintf(Buff, "%s  Level : (3) %d\n", Buff, get_level(3));
  sprintf(Buff, "%s  Level : (4) %d\n", Buff, get_level(4));
  sprintf(Buff, "%s  Level : (5) %d\n", Buff, get_level(5));*/

  if (Get1RestInfo(1)==0)
    strcat(Buff, "  Time Restrictions : None\n");
  else
    sprintf(Buff, "%s  Time Restrictions : No of days allowed:%d  No of days used:%d\n", Buff, Get1RestInfo(2), Get1RestInfo(3));
  int authopt = 0;
  int num_allowed = 0;
  int num_used = 0;
  ULONG start_date = 0;
  int ret = GetRestrictionInfo(&authopt, &start_date, &num_allowed, &num_used);
  if (ret==0)
    {
    sprintf(Buff, "%s  Date license last updated : %s\n", Buff, SGTime(start_date));
    //sprintf(Buff, "%s  Restrictions: Type:%s\n              Start:%s\n", Buff, (authopt<0) ? "N/A" : authTypes[authopt], SGTime(start_date));
    //sprintf(Buff, "%s              Number allowed:%d\n              Number used:%d\n", Buff, num_allowed, num_used);
    }
  strcat(Buff, "  Options :");
  for (int i=1; i<=32/*CK_NumDefinedOpts*/; i++)
    {
    if ((i-1)%8==0)
      strcat(Buff, " ");
    strcat(Buff, GetOption(32, i) ? "1" : "0");
    }
  DWORD d;
  int err = GetAuthorization(&d, 0);
  d=FixOptions(d);
  if (err==0)
    sprintf(Buff, "%s (%04X %04X)\n", Buff, HIWORD(d), LOWORD(d));
  else
    strcat(Buff, "\n");
  
  AfxMessageBox(Buff);
  }

//===========================================================================
//=== SysCAD Specific Code...

class CLicenseInfoDlg : public CDialog
  {
  public:
    CLicenseInfoDlg(CWnd* pParent = NULL);   // standard constructor
    //{{AFX_DATA(CLicenseInfoDlg)
    enum { IDD = IDD_CK_LICENSEINFODLG };
    CListCtrl m_List;
    //}}AFX_DATA
    //{{AFX_VIRTUAL(CLicenseInfoDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  protected:
    //{{AFX_MSG(CLicenseInfoDlg)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  };

//---------------------------------------------------------------------------

CLicenseInfoDlg::CLicenseInfoDlg(CWnd* pParent /*=NULL*/)
  : CDialog(CLicenseInfoDlg::IDD, pParent)
  {
  //{{AFX_DATA_INIT(CLicenseInfoDlg)
  //}}AFX_DATA_INIT
  }

//---------------------------------------------------------------------------

void CLicenseInfoDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CLicenseInfoDlg)
  DDX_Control(pDX, IDC_CK_OPTIONSLIST, m_List);
  //}}AFX_DATA_MAP
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CLicenseInfoDlg, CDialog)
  //{{AFX_MSG_MAP(CLicenseInfoDlg)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CLicenseInfoDlg::OnInitDialog() 
  {
  CWaitCursor Wait;
  CDialog::OnInitDialog();
  char Buff[256];
  int authopt = 0;
  int num_allowed = 0;
  int num_used = 0;
  ULONG start_date = 0;
  int ret = GetRestrictionInfo(&authopt, &start_date, &num_allowed, &num_used);
  if (ret==0)
    sprintf(Buff, "Date and time license last updated : %s", gs_License.SGTime(start_date));
  else
    sprintf(Buff, "Date and time license last updated : Unknown");
  SetDlgItemText(IDC_CK_TXT_DATE, Buff);
  sprintf(Buff, "Number of Copies allowed from this site : %d", gs_License.DemoMode() ? 0 : GetNumCopies());
  SetDlgItemText(IDC_CK_TXT_COPIES, Buff);
  sprintf(Buff, "Number of multi-users that the site has been granted : %d", gs_License.DemoMode() ? 0 : GetNumMultiUsers());
  SetDlgItemText(IDC_CK_TXT_USERS, Buff);
  //sprintf(Buff, "Number of current network users : %d", GetCurrentNetUsers());
  //SetDlgItemText(IDC_CK_TXT_CURRENTUSERS, Buff);
  if (gs_License.DemoMode())
    sprintf(Buff, "Time Restrictions : None (using SysCAD Demo Mode)");
  else
    {
    if (Get1RestInfo(1)==0)
      sprintf(Buff, "Time Restrictions : None");
    else
      sprintf(Buff, "Time Restrictions : No of days allowed:%d  No of days used:%d", Get1RestInfo(2), Get1RestInfo(3));
    }
  SetDlgItemText(IDC_CK_TXT_TIME, Buff);
  if (gs_License.ProbalUnitsAllowed()==CK_InfiniteUnits)
    sprintf(Buff, "Limit on number of ProBal units allowed : None");
  else
    sprintf(Buff, "Limit on number of ProBal units allowed : %d", gs_License.ProbalUnitsAllowed());
  SetDlgItemText(IDC_CK_TXT_PROBALUNITS, Buff);
  if (gs_License.DynUnitsAllowed()==CK_InfiniteUnits)
    sprintf(Buff, "Limit on number of Dynamic units allowed : None");
  else
    sprintf(Buff, "Limit on number of Dynamic units allowed : %d", gs_License.DynUnitsAllowed());
  SetDlgItemText(IDC_CK_TXT_DYNUNITS, Buff);
  if (gs_License.MaxHistSizeAllowed()==CK_InfiniteHistSize)//gs_License.AllowFullHist())
    sprintf(Buff, "Limit on historian size : None");
  else
    {
    long Ttl = gs_License.MaxHistSizeAllowed() * gs_License.MaxHistFilesAllowed();
    sprintf(Buff, "Historian file limits (kilo-bytes) : %d * %d = %d   (%.1f Meg)", gs_License.MaxHistFilesAllowed(), gs_License.MaxHistSizeAllowed(), Ttl, (double)Ttl/1024.0);
    }
  SetDlgItemText(IDC_CK_TXT_HISTSIZE, Buff);
  sprintf(Buff, "Limits on number of trend and graphics windows : %s", (gs_License.DemoMode() || gs_License.TrialMode()) ? "Yes" : "None");
  SetDlgItemText(IDC_CK_TXT_WNDLIMITS, Buff);
  if (gs_License.DemoMode())
    sprintf(Buff, "Demo mode");
  else
    sprintf(Buff, "%s level", CK_LevelNames[gs_License.Level()]);
  SetDlgItemText(IDC_CK_TXT_LEVEL, Buff);
  CString LicUserVer;
  LicUserVer = (gs_License.DemoMode() || GetNumMultiUsers()==0) ? "Stand Alone License" : "Multi-User Network License";
  SetDlgItemText(IDC_CK_TXT_STANDALONE, (const char*)LicUserVer);
  sprintf(Buff, "Location: %s", gs_License.GetAppPath());
  SetDlgItemText(IDC_CK_TXT_LOCATION, Buff);
  int Ver = CrypkeyVersion();
  if (Ver>0)
    sprintf(Buff, "Using CrypKey Version : %d.%d", (int)floor(Ver/10.0), (int)(Ver - (floor(Ver/10.0)*10.0)));
  else
    sprintf(Buff, "CrypKey Version : Unknown");
  SetDlgItemText(IDC_CK_TXT_CRYPKEYVER, Buff);
  m_List.InsertColumn(0, "Option", LVCFMT_LEFT, 240);
  m_List.InsertColumn(1, "Allowed", LVCFMT_CENTER, 64);
  //m_List.DeleteAllItems();
  LVITEM Item;
  Item.mask = LVIF_TEXT;
  Item.state = 0;
  Item.stateMask = 0;
  Item.cchTextMax = 0;
  Item.iImage = 0;
  //DWORD msk = 0x80000000;
  for (int i=0; i<CK_NoOfOptions; i++)
    {
    DWORD msk = (DWORD)pow(2.0, 31-CK_OptionNames[i].iBitNumber);
    bool IsYes = ((gs_License.OpLevel() & msk)!=0);
    if (CK_OptionNames[i].fNormallyShow || IsYes)
      {
      int j = m_List.InsertItem(i, CK_OptionNames[i].pName);
      Item.iItem = j;
      Item.iSubItem = 1;
      Item.pszText = (IsYes ? "Yes" : "No");
      m_List.SetItem(&Item);
      }
    //msk = msk >> 1;
    }
  sprintf(Buff, "SysCAD License Information   [ %s ]", (const char*)LicUserVer);
  SetWindowText(Buff);
  //UpdateData(FALSE);
  //UpdateDialogControls(this, FALSE);
  return TRUE;
  }

//===========================================================================

CSysCADLicense::CSysCADLicense()
  {
  bDynLiteMode = 0;
  bProbalLiteMode = 0;
  bCOMMineServeOn = 0;
  pSecOpt = (CK_SysCADSecurity*)&dwOpLevel;
  }

//---------------------------------------------------------------------------

CSysCADLicense::~CSysCADLicense()
  {
  }

//---------------------------------------------------------------------------

char* CSysCADLicense::GetAppVersion()
  {
  //FullCompany();
  return FullVersion();
  }

//---------------------------------------------------------------------------

DWORD CSysCADLicense::GetDemoOptions()
  {
  CK_SysCADSecurity Opt;
  Opt.m_OpLevel = 0; //clears all bits
  Opt.m_Opts.Level = 0;
  //version options...
  Opt.m_Opts.Ver82 = 0;
  Opt.m_Opts.Ver90 = 1;
  //standard options...
  Opt.m_Opts.Mode_ProBal = 1;
  Opt.m_Opts.Mode_DynamicFlow = 1;
  Opt.m_Opts.Mode_DynamicFull = 1;
  Opt.m_Opts.Mode_Electrical = 0;
  //Opt.m_Opts.Only_SteadyState = 0;
  Opt.m_Opts.Func_FullEdit = 1;
  Opt.m_Opts.Func_COM = 0;
  Opt.m_Opts.Func_COMProp = 0;
  Opt.m_Opts.Func_Drivers = 0;
  Opt.m_Opts.Func_OPCServer = 0;
  //model add-ons...
  Opt.m_Opts.Mdls_Electrical = 0;
  Opt.m_Opts.Mdls_BlackBox = 0;
  Opt.m_Opts.Mdls_HeatExtra = 0;
  Opt.m_Opts.Mdls_HeatBal = 1;
  Opt.m_Opts.Mdls_Alumina = 0;
  Opt.m_Opts.Mdls_SizeDist = 0;
  //other...
  Opt.m_Opts.Old_Dynamic = 0;
  Opt.m_Opts.Old_Probal = 0;
  Opt.m_Opts.Old_Dynamic82 = 0;
  Opt.m_Opts.Old_Probal82 = 0;
  Opt.m_Opts.Client_Other = 0;

  return Opt.m_OpLevel;
  }

//---------------------------------------------------------------------------

DWORD CSysCADLicense::GetTrialOptions()
  {
  CK_SysCADSecurity Opt;
  Opt.m_OpLevel = 0; //clears all bits
  Opt.m_Opts.Level = 0;
  //version options...
  Opt.m_Opts.Ver82 = 0;
  Opt.m_Opts.Ver90 = 1;
  //standard options...
  Opt.m_Opts.Mode_ProBal = 1;
  Opt.m_Opts.Mode_DynamicFlow = 1;
  Opt.m_Opts.Mode_DynamicFull = 1;
  Opt.m_Opts.Mode_Electrical = 0;
  //Opt.m_Opts.Only_SteadyState = 0;
  Opt.m_Opts.Func_FullEdit = 1;
  Opt.m_Opts.Func_COM = 1;
  Opt.m_Opts.Func_COMProp = 1;
  Opt.m_Opts.Func_Drivers = 1;
  Opt.m_Opts.Func_OPCServer = 1;
  //model add-ons...
  Opt.m_Opts.Mdls_Electrical = 1;
  Opt.m_Opts.Mdls_BlackBox = 1;
  Opt.m_Opts.Mdls_HeatExtra = 1;
  Opt.m_Opts.Mdls_HeatBal = 1;
  Opt.m_Opts.Mdls_Alumina = 1;
  Opt.m_Opts.Mdls_SizeDist = 1;
  //other...
  Opt.m_Opts.Client_Other = 0;
  Opt.m_Opts.Old_Dynamic = 0;
  Opt.m_Opts.Old_Probal = 0;
  Opt.m_Opts.Old_Dynamic82 = 0;
  Opt.m_Opts.Old_Probal82 = 0;
  return Opt.m_OpLevel;
  }

//---------------------------------------------------------------------------

void CSysCADLicense::CheckForLiteModes()
  {
  bDynLiteMode = 0;
  bProbalLiteMode = 0;
  if (bDidInitCrypkey && !Blocked() && !DemoMode())
    {
    if (AllowProBal() && !AllowDynamicFlow() && !AllowDynamicFull())
      {
      bDynLiteMode = 1;
      }
    else if (!AllowProBal() && (AllowDynamicFlow() || AllowDynamicFull()))
      {
      bProbalLiteMode = 1;
      }
    }
  }

//---------------------------------------------------------------------------

void CSysCADLicense::Info()
  {
  CLicenseInfoDlg Dlg;
  Dlg.DoModal();
  }

//---------------------------------------------------------------------------

int CSysCADLicense::ProbalUnitsAllowed()
  {
  if (bDemoMode)
    return CK_Demo_ProbalUnits;
  if (bProbalLiteMode)
    return CK_Lite_ProbalUnits;
  if (pSecOpt->m_Opts.Level==CK_TrialLevel)
    return CK_Trial_ProbalUnits;
  return CK_InfiniteUnits; 
  }

//---------------------------------------------------------------------------

int CSysCADLicense::DynUnitsAllowed()
  {
  if (bDemoMode)
    return CK_Demo_DynUnits;
  if (bDynLiteMode)
    return CK_Lite_DynUnits;
  if (pSecOpt->m_Opts.Level==CK_TrialLevel)
    return CK_Trial_DynUnits;
  return CK_InfiniteUnits; 
  }

//---------------------------------------------------------------------------

long CSysCADLicense::MaxHistSizeAllowed()
  {
  if (bDemoMode)
    return CK_DemoHistSize;
  //if (AllowFullHist())
  //  return CK_InfiniteHistSize; 
  if (pSecOpt->m_Opts.Level==CK_TrialLevel)
    return CK_TrialHistSize;
  return CK_InfiniteHistSize;
  //return CK_NormalHistSize;
  }

//---------------------------------------------------------------------------

UINT CSysCADLicense::MaxHistFilesAllowed()
  {
  if (bDemoMode)
    return CK_DemoHistFiles;
  //if (AllowFullHist())
  //  return CK_InfiniteHistFiles; 
  if (pSecOpt->m_Opts.Level==CK_TrialLevel)
    return CK_TrialHistFiles;
  return CK_InfiniteHistFiles;
  //return CK_NormalHistFiles;
  }

//---------------------------------------------------------------------------

int CSysCADLicense::TrendWindowsAllowed() 
  { 
  if (bDemoMode)
    return CK_DemoTrends;
  if (pSecOpt->m_Opts.Level==CK_TrialLevel)
    return CK_TrialTrends;
  return CK_InfiniteTrends; 
  }

//---------------------------------------------------------------------------

int CSysCADLicense::GraphicWindowsAllowed() 
  {
  if (bDemoMode)
    return CK_DemoGrfs;
  if (pSecOpt->m_Opts.Level==CK_TrialLevel)
    return CK_TrialGrfs;
  return CK_InfiniteGrfs;
  }

//---------------------------------------------------------------------------

DWORD CSysCADLicense::LicCatagories()
  {
  DWORD dw=TOC_STD_KENWALT;
  if (AllowMdlsHeatBal())
    dw |= TOC_HEATBAL;
  if (AllowMdlsHeatExtra())
    dw |= TOC_HEATEXTRA;
  if (AllowMdlsSizeDist())
    dw |= TOC_SIZEDIST;
  if (AllowMdlsAlumina())
    dw |= TOC_ALUMINA;
  if (AllowMdlsBlackBox())
    dw |= TOC_BLACKBOX;
  if (AllowMdlsElec())
    dw |= TOC_POWERDIST;

  if (AllowMdlsUser())
    dw |= TOC_USER;
  if (AllowMdlsQAL())
    dw |= TOC_QAL;
  if (AllowMdlsQALExtra())
    dw |= TOC_QALEXTRA;
  if (ALSOALLOWMINESERVEMDLS || ForMineServe())
    dw |= TOC_MINESERVE;
  if (AllowMdlsAlcan())
    dw |= TOC_ALCAN;
  if (AllowMdlsRTTS())
    dw |= TOC_RTTS;

  if (AllowProBal())
    dw |= TOC_PROBAL;
  if (AllowDynamicFlow())
    dw |= TOC_DYNAMICFLOW;
  if (AllowDynamicFull())
    dw |= TOC_DYNAMICFULL;
  return dw;
  }

//---------------------------------------------------------------------------

void CSysCADLicense::SetForMineServe(bool On)  
  { 
  bCOMMineServeOn=On; 
  Check();

  //CScdCOCmdBase::s_hWnd4Msgs=NULL;

  //AfxGetMainWnd()->PostMessage(WMU_UPDATEMAINWND, SUB_UPDMAIN_BACKGROUND, 0);
  };               

//---------------------------------------------------------------------------

void CSysCADLicense::SetAsRunTime()            { pSecOpt->m_Opts.Func_FullEdit = 0; };
BOOL CSysCADLicense::IsRunTime()               { return !pSecOpt->m_Opts.Func_FullEdit; };
BOOL CSysCADLicense::AllowVer90()              { return pSecOpt->m_Opts.Ver90 && !bBlocked; };
BOOL CSysCADLicense::AllowMdlsRTTS()           { return pSecOpt->m_Opts.Client_RTTS && !bBlocked; };
BOOL CSysCADLicense::ForMineServe()            { return pSecOpt->m_Opts.Client_MineServe && !bBlocked; };
BOOL CSysCADLicense::AllowMdlsAlcan()          { return pSecOpt->m_Opts.Client_Alcan && !bBlocked; };
BOOL CSysCADLicense::AllowMdlsQALExtra()       { return pSecOpt->m_Opts.Client_QALExtra && !bBlocked; };
BOOL CSysCADLicense::AllowMdlsQAL()            { return pSecOpt->m_Opts.Client_QAL && !bBlocked; };
BOOL CSysCADLicense::AllowMdlsUser()           { return pSecOpt->m_Opts.Client_Other && !bBlocked; };
BOOL CSysCADLicense::AllowMdlsBlackBox()       { return pSecOpt->m_Opts.Mdls_BlackBox && !bBlocked; };
BOOL CSysCADLicense::AllowMdlsHeatExtra()      { return pSecOpt->m_Opts.Mdls_HeatExtra && !bBlocked; };
BOOL CSysCADLicense::AllowMdlsHeatBal()        { return pSecOpt->m_Opts.Mdls_HeatBal && !bBlocked; };
BOOL CSysCADLicense::AllowMdlsAlumina()        { return pSecOpt->m_Opts.Mdls_Alumina && !bBlocked; };
BOOL CSysCADLicense::AllowMdlsSizeDist()       { return pSecOpt->m_Opts.Mdls_SizeDist && !bBlocked; };
BOOL CSysCADLicense::AllowMdlsElec()           { return pSecOpt->m_Opts.Mdls_Electrical && !bBlocked; };
BOOL CSysCADLicense::AllowProBal()             { return pSecOpt->m_Opts.Mode_ProBal && !bBlocked; };
BOOL CSysCADLicense::AllowDynamicFlow()        { return pSecOpt->m_Opts.Mode_DynamicFlow && !bBlocked; };
BOOL CSysCADLicense::AllowDynamicFull()        { return pSecOpt->m_Opts.Mode_DynamicFull && !bBlocked; };
BOOL CSysCADLicense::AllowProBalLite()         { return bProbalLiteMode && !bBlocked; };
BOOL CSysCADLicense::AllowDynamicLite()        { return bDynLiteMode && !bBlocked; };
//BOOL CSysCADLicense::OnlySteadyState()         { return pSecOpt->m_Opts.Only_SteadyState; }
BOOL CSysCADLicense::AllowFullLic()            { return pSecOpt->m_Opts.Func_FullEdit && !bBlocked; };
BOOL CSysCADLicense::AllowFullLicFlag()        { return pSecOpt->m_Opts.Func_FullEdit; };
BOOL CSysCADLicense::AllowDrivers()            { return pSecOpt->m_Opts.Func_Drivers && !bBlocked; };
BOOL CSysCADLicense::AllowOPCServer()          { return pSecOpt->m_Opts.Func_OPCServer && !bBlocked; };
BOOL CSysCADLicense::AllowCOMInterface()       { return pSecOpt->m_Opts.Func_COM && !bBlocked; };
BOOL CSysCADLicense::AllowCOMProps()           { return pSecOpt->m_Opts.Func_COMProp && !bBlocked; };
UCHAR CSysCADLicense::Level()                  { return (UCHAR)(pSecOpt->m_Opts.Level); };

#if CK_SCDCOMLIC
BOOL CSysCADLicense::AllowCOMMdl()     { return AllowCOMProps(); };
BOOL CSysCADLicense::AllowCOMSlv()     { return AllowCOMProps(); };
BOOL CSysCADLicense::AllowCOMApp()     { return AllowCOMInterface(); };
#else
BOOL CSysCADLicense::AllowCOMMdl()     { return 1; };
BOOL CSysCADLicense::AllowCOMSlv()     { return 1; };
BOOL CSysCADLicense::AllowCOMApp()     { return 1; };
#endif

//===========================================================================

CSysCADLicense gs_License;

//===========================================================================
//place code similar to this in application init instance...

//  #if CK_LICENSINGON
//  if (!gs_License.Init())
//    return FALSE;
//  if (!gs_License.Check())
//    {
//    #if CK_ALLOWDEMOMODE
//    gs_License.SetDemoMode(TRUE);
//    #else
//    if (!gs_License.StartDialog())
//      return FALSE;
//    if (!gs_License.Check()) //check again!
//      return FALSE;
//    #endif
//    }
//  #endif




/*

  ProblemID:270 
   
Product: CrypKey Version 6.0 
  
Problem: Error starting program - occurs when InitCrypkey is called 
  
Description: This problem occurs in debug mode. Outside of the development environment the program 
runs normally. 
  
Solution: You cannot debug CrypKey functions unless you use the COM object. If you want to debug using 
the other CrypKey libraries then, disable Crypkey functions or employ another method of debugging such 
as running in non-debug mode and using messageboxes or output file to log results from Crypkey functions. 
You could also exclude CrypKey functions by using #ifdef to determine process execution based on the 
configuration: #ifdef _DEBUG intializeWithOutCrypKey(); #else initializeWithCrypKey(); #endif 

  */
