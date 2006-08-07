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

// Force search of Version.Lib for Version control Functions
#pragma comment(lib, "version.lib")

#define CK_USE6134        01    
                       
#define USESERVERVERSION  1

#if USESERVERVERSION 
#import "..\..\..\License\CrypKeyCOMServer.exe" no_namespace named_guids
static ICrypKeySDKServerPtr s_ptr;
#else
#import "..\..\..\License\CrypKeyCOM.dll" no_namespace named_guids
static ICrypKeySDKPtr s_ptr;
#endif

#if CK_USE6134 
#include "crypkey.6134.h"
#else
#include "crypkey.57.h"
#endif

#undef ReadyToTry 
#undef ReadyToTryDays 
#undef ReadyToTryRuns 

#endif

#include "winsvc.h"

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

struct CK_OptionDetails
  {
  int     m_iBitNumber;                          
  char  * m_pName;
  bool    m_fNormallyShow;
  };

CK_OptionDetails CK_OptionNames[] =
{
  {  0, "Solver Mode: ProBal", true },
  {  1, "Solver Mode: Dynamic Transfer (Flow)", true },
  {  2, "Solver Mode: Dynamic Full", true },
  {  3, "Solver Mode: Electrical", false },
  {  6, "Full License (No=runtime only)", true },
  {  7, "Academic License", false },
  {  8, "Application COM Interface", true },
  {  9, "COM Specie Properties", false },//true },
  { 10, "Marshal (OPC Client)", true },
  { 11, "OPC Server", true },
  { 14, "Models: Heat Exchange", true },
  { 15, "Models: Heat Exchange Extra", true },
  { 16, "Models: Size Distribution", true },
  { 17, "Models: Alumina", true },
  { 18, "Models: Electrical", false },
  { 19, "Models: SMDK Runtime", true },
  { 22, "Client: Alcan", false },
  { 23, "Client: Gijima MineServe", false },
  { 24, "Client: QAL", false },
  { 25, "Client: RioTinto", false },
  { 26, "Client: Custom Models", false },
  { -1},
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
int GetAuthorization(DWORD * dwOpLevel, int dec)  
  { 
  CK_SysCADSecurity Opt;

  Opt.m_OpLevel = 0; //clears all bits

  Strng Fn(ProgFiles());
  Fn+="License.Temp.ini";
  CProfINIFile PF(Fn());

  Opt.m_Opts.Level            = /* 1; */        PF.RdInt("Options", "Level            ",    1  );

  Opt.m_Opts.Reserved82         = PF.RdInt("Options", "Reserved82       ",    1  );
  Opt.m_Opts.Reserved90         = PF.RdInt("Options", "Reserved90       ",    1  );
  Opt.m_Opts.Spare_28           = PF.RdInt("Options", "Spare_28         ",    1  );
  Opt.m_Opts.Client_Other       = PF.RdInt("Options", "Client_Other     ",    1  );
  Opt.m_Opts.Client_QAL         = PF.RdInt("Options", "Client_QAL       ",    1  );
  Opt.m_Opts.Client_RTTS        = PF.RdInt("Options", "Client_RTTS      ",    1  );
  Opt.m_Opts.Client_Alcan       = PF.RdInt("Options", "Client_Alcan     ",    1  );
  Opt.m_Opts.Client_MineServe   = PF.RdInt("Options", "Client_MineServe ",    1  );
  Opt.m_Opts.Spare_22           = PF.RdInt("Options", "Spare_22         ",    1  );
  Opt.m_Opts.Spare_21           = PF.RdInt("Options", "Spare_21         ",    0  );
  Opt.m_Opts.Mdls_SMDKRuntime   = PF.RdInt("Options", "Mdls_SMDKRuntime ",    0  );
  Opt.m_Opts.Mdls_Electrical    = PF.RdInt("Options", "Mdls_Electrical  ",    0  );
  Opt.m_Opts.Mdls_Alumina       = PF.RdInt("Options", "Mdls_Alumina     ",    1  );
  Opt.m_Opts.Mdls_SizeDist      = PF.RdInt("Options", "Mdls_SizeDist    ",    1  );
  Opt.m_Opts.Mdls_HeatExtra     = PF.RdInt("Options", "Mdls_HeatExtra   ",    1  );
  Opt.m_Opts.Mdls_HeatBal       = PF.RdInt("Options", "Mdls_HeatBal     ",    1  );
  Opt.m_Opts.Func_Spare2        = PF.RdInt("Options", "Func_Spare2      ",    1  );
  Opt.m_Opts.Func_Spare1        = PF.RdInt("Options", "Func_Spare1      ",    1  );
  Opt.m_Opts.Func_OPCServer     = PF.RdInt("Options", "Func_OPCServer   ",    0  );
  Opt.m_Opts.Func_Drivers       = PF.RdInt("Options", "Func_Drivers     ",    0  );
  Opt.m_Opts.Func_COMProp       = PF.RdInt("Options", "Func_COMProp     ",    0  );
  Opt.m_Opts.Func_COM           = PF.RdInt("Options", "Func_COM         ",    0  );
  Opt.m_Opts.Func_Academic      = PF.RdInt("Options", "Func_Academic    ",    0  );
  Opt.m_Opts.Func_FullEdit      = PF.RdInt("Options", "Func_FullEdit    ",    0  );
  Opt.m_Opts.Mode_Spare         = PF.RdInt("Options", "Mode_Spare       ",    0  );
  Opt.m_Opts.Only_SteadyState   = PF.RdInt("Options", "Only_SteadyState ",    0  );
  Opt.m_Opts.Mode_Electrical    = PF.RdInt("Options", "Mode_Electrical  ",    0  );
  Opt.m_Opts.Mode_DynamicFull   = PF.RdInt("Options", "Mode_DynamicFull ",    0  );
  Opt.m_Opts.Mode_DynamicFlow   = PF.RdInt("Options", "Mode_DynamicFlow ",    0  );
  Opt.m_Opts.Mode_ProBal        = PF.RdInt("Options", "Mode_ProBal      ",    0  );



  *dwOpLevel=Opt.m_OpLevel; 

  return (Opt.m_Opts.Mode_DynamicFull || Opt.m_Opts.Mode_DynamicFlow || Opt.m_Opts.Mode_ProBal) ? 0 : -777;
  };

int CrypkeyVersion()                              { return 11; };
LPTSTR ExplainErr(int Func, int err)              { return "Explanation"; };
#else

#endif

//===========================================================================
//
//
//
//===========================================================================

int MyGetOption(int No, int Index)
  {
  if (gs_License.UseCOM())
    return s_ptr->GetOption(No, Index);
  return GetOption(No, Index);
  }

int MyGetAuthorization(long * dwOpLevel, int dec)
  {
  if (gs_License.UseCOM())
    return s_ptr->GetAuthorization(dwOpLevel, dec);
  return GetAuthorization((DWORD*)dwOpLevel, dec);
  }

ULONG MyCKChallenge32(ULONG random1, ULONG random2)
  { 
  if (gs_License.UseCOM())
    return s_ptr->CKChallenge32(random1, random2);
#if BYPASSLICENSING
  return 0;
#else
  return CKChallenge32(random1, random2);
#endif
  };

LPTSTR MyExplainErr(int Func, int err)              
  { 
  if (gs_License.UseCOM())
    return s_ptr->ExplainErr(CKExplainEnum(Func), err);
  return ExplainErr(Func, err);
  };

int MyGetRestrictionInfo(int * authopt, ULONG * start_date, int * num_allowed, int * num_used)
  { 
  if (gs_License.UseCOM())
    return s_ptr->GetRestrictionInfo((long*)authopt, (long*)start_date, (long*)num_allowed, (long*)num_used);
  return GetRestrictionInfo(authopt, start_date, num_allowed, num_used);  
  };

int MyGet1RestInfo(int X)
  { 
  if (gs_License.UseCOM())
    return s_ptr->Get1RestInfo(CKWhichEnum(X));
  return Get1RestInfo(X);  
  };

int MyGetNumMultiUsers()
  { 
  if (gs_License.UseCOM())
    return s_ptr->GetNumMultiUsers();
  return GetNumMultiUsers();  
  };

ULONG MyGetLevel(int NoLevels)  
  {
  if (gs_License.UseCOM())
    return s_ptr->GetLevel(NoLevels);
  return GetLevel(NoLevels);
  }

ULONG MyGetNumCopies()  
  {
  if (gs_License.UseCOM())
    return s_ptr->GetNumCopies();
  return GetNumCopies();
  }

int MyCrypKeyVersion()  
  {
  if (gs_License.UseCOM())
    return s_ptr->CrypKeyVersion();
  return CrypkeyVersion();
  }
                  
int MyGetSiteCode(char far *site_code)
  {
  if (gs_License.UseCOM())
    {
    _bstr_t SiteCode;
    int err=s_ptr->GetSiteCode(&SiteCode.GetBSTR());
    strcpy(site_code, (LPCTSTR)SiteCode);
    return err;
    }
#if BYPASSLICENSING
  return 0;
#else
  return GetSiteCode(site_code);
#endif
  };
char * MyGetSiteCode2(void)
  {
  if (gs_License.UseCOM())
    return s_ptr->GetSiteCode2();
#if BYPASSLICENSING
  return NULL;
#else
  return GetSiteCode2();
#endif
  };
int MyInitCrypkey(char far *filepath, char far *masterkey, char far *userkey, int allow_floppy, unsigned network_max_checktime)
  {
  if (gs_License.UseCOM())
    return s_ptr->InitCrypKey(filepath, masterkey, userkey, allow_floppy, network_max_checktime);
#if BYPASSLICENSING
  return 0;
#else
  return InitCrypkey(filepath, masterkey, userkey, allow_floppy, network_max_checktime);
#endif
  };
int MyEndCrypkey()
  {
  if (gs_License.UseCOM())
    return s_ptr->EndCrypKey();
#if BYPASSLICENSING
#else
  EndCrypkey();
#endif
  return 0;
  };
int MyReadyToTry(unsigned long authlevel, int numDays)
  {
  if (gs_License.UseCOM())
    return s_ptr->ReadyToTry(authlevel, numDays);
#if BYPASSLICENSING
  return 1;
#else
  return readyToTry(authlevel, numDays);
#endif
  };
int MyReadyToTryDays(unsigned long authlevel, int numDays, int version, int copies)
  {
  if (gs_License.UseCOM())
    return s_ptr->ReadyToTryDays(authlevel, numDays, version, copies);
#if BYPASSLICENSING
  return 100;
#else
  return readyToTryDays(authlevel, numDays, version, copies);
#endif
  };
int MyReadyToTryRuns(unsigned long authlevel, int numRuns, int version, int copies)
  {
  if (gs_License.UseCOM())
    return s_ptr->ReadyToTryRuns(authlevel, numRuns, version, copies);
#if BYPASSLICENSING
  return 100;
#else
  return readyToTryRuns(authlevel, numRuns, version, copies);
#endif
  };
int MyRegisterTransfer(char far *target)
  {
  if (gs_License.UseCOM())
    return s_ptr->RegisterTransfer(target);
#if BYPASSLICENSING
  return 0;
#else
  return RegisterTransfer(target);
#endif
  };
int MySaveSiteKey(char far *sitekey)
  {
  if (gs_License.UseCOM())
    return s_ptr->SaveSiteKey(sitekey);
#if BYPASSLICENSING
  return 0;
#else
  return SaveSiteKey(sitekey);
#endif
  };
int MyDirectTransfer(char far *target)
  {
  if (gs_License.UseCOM())
    return s_ptr->DirectTransfer(target);
#if BYPASSLICENSING
  return 0;
#else
  return DirectTransfer(target);
#endif
  };

void MySetNetHandle(unsigned short net_handle)
  {
  if (gs_License.UseCOM())
    s_ptr->SetNetHandle(net_handle);
#if BYPASSLICENSING
  return ;
#else
  else
    SetNetHandle(net_handle);
#endif
  };
int MyTransferIn(char far *target)
  {
  if (gs_License.UseCOM())
    return s_ptr->TransferIn(target);
#if BYPASSLICENSING
  return 0;
#else
  return TransferIn(target);
#endif
  };
int MyTransferOut(char far *target)
  {
  if (gs_License.UseCOM())
    return s_ptr->TransferOut(target);
#if BYPASSLICENSING
  return 0;
#else
  return TransferOut(target);
#endif
  };


//===========================================================================
//
//
//
//===========================================================================

#define dbgTimeLicensing 1

#if dbgTimeLicensing
class CStackTimer : public CStopWatch 
  {
  public:
    CStackTimer(LPCTSTR Name)
      {
      m_Name=Name;
      Start();
      }

    ~CStackTimer()
      {
      dbgpln("License:  %16.2f %s", LapTime()*1e6, m_Name);
      }

    CString m_Name;
  };
#endif


dword CSysCADLicense::FixOptions(dword dwOpLevel) 
  { 
  CK_SysCADSecurity Opt;
  Opt.m_OpLevel = dwOpLevel; 

  if (m_State.m_bCOMMineServeOn || FORCEMINESERVE)
    {
    //Opt.m_OpLevel = 0;
    Opt.m_Opts.Client_MineServe = 1;

    }

  if (Opt.m_Opts.Client_MineServe)
    {
    if (m_State.m_bTrialMode)
      Opt.m_OpLevel |= GetTrialOptions();
    if (m_State.m_bDemoMode)
      Opt.m_OpLevel |= GetDemoOptions();

    m_State.m_bLicensed  = 1;
    m_State.m_bTrialMode = 0;
    m_State.m_bDemoMode  = 0;

    Opt.m_Opts.Level            = 1;
    //Opt.m_Opts.xVer90            |= 1;
    Opt.m_Opts.Mode_DynamicFlow |= 1;
    Opt.m_Opts.Mode_DynamicFull |= 1;
    Opt.m_Opts.Func_COMProp     |= 1;
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
  m_bDidInitCrypkey = 0;
  m_sLastPath = "A:\\";
  m_sAppPath = "";
  m_iDaysLeft = 0;
  m_bUseCOM= 0;
  m_iStackLvl=0;

  m_State.m_bMultiUserFailure = 0;
  m_State.m_bLicensed = 0;
  m_State.m_bDemoMode = 0;
  m_State.m_bTrialMode = 0;
  m_State.m_bTrialFailed = 0;
  m_State.m_bCOMMineServeOn = 0;
  m_State.m_dwOpLevel = 0;
  }

//---------------------------------------------------------------------------

CLicense::~CLicense()
  {
  Exit();
  }

//---------------------------------------------------------------------------

BOOL CLicense::CheckLicenseVersionOK(LPCTSTR SysCADEXEPath)
  {
#if CK_USE6134
  const int ReqdSysCADEXEVersion = 2;
#else
  const int ReqdSysCADEXEVersion = 1;
#endif

  CString EXE(SysCADEXEPath);
  if (EXE.Right(10).CompareNoCase("syscad.exe")!=0)
    EXE+="SysCAD.exe";
  bool SysCADEXEVerOK=false;
  int ErrCode=1;
  DWORD dwHandle;
  DWORD dwVerSize=::GetFileVersionInfoSize(EXE, &dwHandle);
  //dbgpln("Lic:%i  %s", dwVerSize, EXE);
  if (dwVerSize)
    {
    //dbgpln("Lic:AAA");
    char * Buff=new char[dwVerSize];
    if (::GetFileVersionInfo(EXE, dwHandle, dwVerSize, Buff))
      {
      //dbgpln("Lic:BBB");
      VS_FIXEDFILEINFO * pInfo;
      UINT InfoLen;
      if (::VerQueryValue(Buff, "\\", (LPVOID*)&pInfo, &InfoLen))
        {
        //dbgpln("Lic:CCC %i %i", HIWORD(pInfo->dwFileVersionMS), ReqdSysCADEXEVersion);
        SysCADEXEVerOK=(HIWORD(pInfo->dwFileVersionMS)==ReqdSysCADEXEVersion);
        }
      }
    delete[] Buff;
    }

  if (SysCADEXEVerOK)
    return true;

  if (AfxMessageBox("SysCAD License: Incorrect Version!\n\n"
    "Continuing will destroy the current license\n\n"
    "Continue ?", MB_ICONEXCLAMATION|MB_YESNO)==IDYES)
    return true;

  return false;
  }

//---------------------------------------------------------------------------

inline void CLicense::SetAppPath(char* p)
  {
  m_sAppPath = p; 
  };

//---------------------------------------------------------------------------

void CLicense::PushState()
  {
  dbgpln("CLicense::PushState() %i", m_iStackLvl);
  m_iStackLvl++;
  int MaxStk=sizeof(m_StateStack)/sizeof(m_StateStack[0]);
  if (m_iStackLvl>=MaxStk)
    {
    m_iStackLvl = MaxStk-1;
    LogError("License", 0, "CLicense::PushState() Stack Overflow");
    }
  m_StateStack[m_iStackLvl]=m_State;
  if (ScdMainWnd())
    ScdMainWnd()->PostMessage(WMU_UPDATEMAINWND, SUB_UPDMAIN_BACKGROUND, 0);
  }

void CLicense::PopState()
  {
  m_iStackLvl--;
  if (m_iStackLvl<0)
    {
    m_iStackLvl=0;
    LogError("License", 0, "CLicense::PopState() Stack Underflow");
    }
  m_State=m_StateStack[m_iStackLvl];
  if (ScdMainWnd())
    ScdMainWnd()->PostMessage(WMU_UPDATEMAINWND, SUB_UPDMAIN_BACKGROUND, 0);
  }

//---------------------------------------------------------------------------

void CLicense::SetBlocked(BOOL Block)
  {
  dbgpln("CLicense::SetBlocked %s", Block?"ON":"Off");
  m_State.m_bBlocked = Block;
  };

BOOL CLicense::Blocked()
  {
  return m_State.m_bBlocked && !m_State.m_bCOMMineServeOn; //m_iStackLvl==0; 
  };

//---------------------------------------------------------------------------

BOOL CLicense::Init(char* path /*=NULL*/)
  {
#if dbgTimeLicensing
  CStackTimer TM("Init");
#endif
  CWaitCursor Wait;
#if !BYPASSLICENSING
  if (m_bDidInitCrypkey)
    {
    MyEndCrypkey();
    if (m_bUseCOM)
      s_ptr.Release();
    }
#endif
  m_bDidInitCrypkey = 0;
  m_State.m_bMultiUserFailure = 0;
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

  if (!CheckLicenseVersionOK(ShortPath))
    return false;
  
#if !BYPASSLICENSING
  int err = -1;
  if (m_bUseCOM)
    {                 
#if CK_USE6134
    s_ptr.CreateInstance ("CrypKey.SDKServer");
#else
    s_ptr.CreateInstance ("CrypKey.SDK");
#endif
    if (s_ptr==NULL)
      {
      Error("Check if 'CrypKeyCOMServer' is running correctly?");
      return FALSE;
      }
    }
  err = MyInitCrypkey(ShortPath, CK_MASTER_KEY, CK_USER_KEY, FALSE, CK_NetworkChecktime);

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
        err = MyInitCrypkey(ShortPath, CK_MASTER_KEY, CK_USER_KEY, FALSE, CK_NetworkChecktime);
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
  int Ver=MyCrypKeyVersion();
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
  m_bDidInitCrypkey = 1;
  return TRUE;
  }

//---------------------------------------------------------------------------

void CLicense::Exit()
  {
#if dbgTimeLicensing
  CStackTimer TM("Exit");
#endif

#if !BYPASSLICENSING
  if (m_bDidInitCrypkey)
    {
    MyEndCrypkey();
    if (m_bUseCOM)
      s_ptr.Release();
    }
#endif
  m_bDidInitCrypkey = 0;
  }

//---------------------------------------------------------------------------

BOOL CLicense::Check(BOOL Prompt /*=FALSE*/)
  {
#if dbgTimeLicensing
  CStackTimer TM("Check");
#endif
  ASSERT(m_bDidInitCrypkey);
  CWaitCursor Wait;
  m_State.m_bLicensed = 0;
  m_State.m_bMultiUserFailure = 0;
  m_State.m_dwOpLevel = 0;
  m_iDaysLeft = 0;

//#if CK_LICENSINGON

  int err= MyGetAuthorization((long*)&m_State.m_dwOpLevel, 0); //check authorization, use up 0 runs

  dbgpln("CrypKey GetAuth A:%08x", m_State.m_dwOpLevel);

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

    result2 = MyCKChallenge32(random1, random2);
    if (result1 != result2)
      {
      Error("Security Failure.  Challenge function failed");
      return FALSE;
      }
#endif
    m_State.m_bLicensed = 1;
    DWORD dw = (DWORD)pow(2.0, CK_NumDefinedLevels) - 1;
    m_State.m_bTrialMode = ((m_State.m_dwOpLevel & dw)==CK_TrialLevel);
    m_iDaysLeft = (MyGet1RestInfo(1)==0) ? CK_InfiniteDays : MyGet1RestInfo(2) - MyGet1RestInfo(3);
    m_State.m_bDemoMode = 0;
    ScdPFMachine.WrInt(LicINISection, "InDemoMode", m_State.m_bDemoMode);
    CheckForLiteModes();

#if BYPASSLICENSING
    if (1)
      {
      Strng Fn(ProgFiles());
      Fn+="License.Temp.ini";
      CProfINIFile PF(Fn());
      m_State.m_bDemoMode=PF.RdInt("Options", "Demo",    0  );
      }
#endif

    return TRUE;
    }
  else
    {
    m_State.m_dwOpLevel = 0;
    if (err>0)
      {
      m_State.m_bMultiUserFailure = 1;
      char Buff[2048];
      NetworkUsersInfo(Buff);
      //Error("More than %d users already using this license.\n\nNeed to wait for %d user%s to quit!\n\n%s", MyGetNumMultiUsers(), err, (err>1 ? "s" : ""), Buff);
      Error("More than %d users already using this license!\n\n%s", MyGetNumMultiUsers(), Buff);
      }
    if (Prompt && err!=AUTH_NOT_PRESENT && err<0)
      Error("Get Authorization failed!\nReturned %d : %s", err, MyExplainErr(EXP_AUTH_ERR, err));
    }
  CheckForLiteModes();
  m_State.m_dwOpLevel=FixOptions(m_State.m_dwOpLevel);

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
#if dbgTimeLicensing
  CStackTimer TM("QuickCheck");
#endif

  ASSERT(m_bDidInitCrypkey);
  if (m_State.m_bBlocked)
    return FALSE;
  if (CheckLevel==0)
    {
    if (!m_State.m_bDemoMode && !m_State.m_bLicensed && !m_State.m_bCOMMineServeOn)
      {
      SetBlocked();
      Error("Security Failure.  License blocked\n\nThe majority of SysCAD commands and functions have been disabled.\n\nPlease exit SysCAD. (Save project if required)");
      ScdMainWnd()->PostMessage(WMU_UPDATEMAINWND, SUB_UPDMAIN_BACKGROUND, 0);
      return FALSE;
      }
    return TRUE;
    }

  if (m_State.m_bDemoMode)
    return TRUE;

  CWaitCursor Wait;
  m_State.m_bLicensed = 0;
  m_State.m_dwOpLevel = 0;
  int err = MyGetAuthorization((long*)&m_State.m_dwOpLevel, 0); //check authorization, use up 0 runs
  int OtherErr = 0;
  dbgpln("CrypKey GetAuth B:%08x", m_State.m_dwOpLevel);

#if !BYPASSLICENSING
  if (err==0 && CheckLevel>1)
    {
    //generate some random numbers - this can be done any way you like
    ULONG random1 = (time(NULL)<<1)+9;
    ULONG random2 = random1+rand();
    ULONG result1 = Challenge32(CK_COMPANYNUM, CK_PASSNUM/2, random1, random2);
    ULONG result2;
    result2 = MyCKChallenge32(random1, random2);

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
    {
    m_State.m_bLicensed = 1;
    }
  else
    {
    char Buff[1024];
    m_State.m_dwOpLevel = 0;
    SetBlocked();
    if (OtherErr==0)
      sprintf(Buff, "Security Failure.  License blocked\n\nReturned %d : %s", err, MyExplainErr(EXP_AUTH_ERR, err));
    else
      sprintf(Buff, "Security Failure.  License blocked (%d)", OtherErr);
    strcat(Buff, "\n\nThe majority of SysCAD commands and functions have been disabled.\n\nPlease exit SysCAD. (Save project if required)");
    Error(Buff);
    ScdMainWnd()->PostMessage(WMU_UPDATEMAINWND, SUB_UPDMAIN_BACKGROUND, 0);
    return FALSE;
    }
  m_State.m_dwOpLevel=FixOptions(m_State.m_dwOpLevel);

  //#else
//  bLicensed = 1;
//  dwOpLevel = 0;
//#endif
  return TRUE;
  }

//---------------------------------------------------------------------------

int CLicense::Copies()
  {
  return MyGetNumCopies();
  }

//---------------------------------------------------------------------------

int CLicense::MultiUsers()
  {
  return MyGetNumMultiUsers();
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
  int ret = MyReadyToTryDays(OpLevel, NoOfDays, CK_TrialVerNo, 1);
  if (ret)
    {
    Error("Issue Trial License failed!\nReturned %d : %s", ret, MyExplainErr(EXP_RTT_ERR, ret));
    m_State.m_bTrialFailed = 1;
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
  Dlg.m_AppPath = m_sAppPath;
  if (Dlg.DoModal()==IDOK)
    {
    if (_stricmp((const char*)m_sAppPath, (const char*)Dlg.m_AppPath)!=0)
      {
      CString PrevAppPath = m_sAppPath;
      m_sAppPath = Dlg.m_AppPath;
      if (CheckAndInit)
        {
        //check license on new location...

        if (!CheckLicenseVersionOK(m_sAppPath))
          {
          m_sAppPath = PrevAppPath;
          return RetCode;
          }

        CWaitCursor Wait;
        BOOL Failed = FALSE;
        if (!Init())
          Failed = TRUE;
        if (!Failed && !Check(TRUE)) //re-check options etc
          Failed = TRUE;
        if (Failed)
          {
          char Buff[1024];
          sprintf(Buff, "License Initialisation Failed:\n\n"
                        "Set license to new location anyway?\n\n"
                        "Old location : %s\n"
                        "New location : %s", 
            (const char*)PrevAppPath, (const char*)m_sAppPath);
          if (AfxMessageBox(Buff, MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2)!=IDYES)
            {
            Wait.Restore();
            m_sAppPath = PrevAppPath;
#if !BYPASSLICENSING
            MyEndCrypkey();
            if (m_bUseCOM)
              s_ptr.Release();
#endif
            m_bDidInitCrypkey = 0;
            if (!Init())
              Error("Change location\nFailed to re-initialize license!");
            else if (!Check(TRUE)) //re-check options etc
              Error("Change location\nFailed to re-initialize license!");
            ScdMainWnd()->PostMessage(WMU_UPDATEMAINWND, SUB_UPDMAIN_BACKGROUND, 0);
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
      ScdPFMachine.WrStr(LicINISection, "LicenseLocation", (const char*)m_sAppPath);
      ScdMainWnd()->PostMessage(WMU_UPDATEMAINWND, SUB_UPDMAIN_BACKGROUND, 0);
      }
    else
      RetCode = 3; //unchanged
    }
  return RetCode; //cancel
  }


//---------------------------------------------------------------------------

void CLicense::SetUseCOM(BOOL On)
  {
  if (!m_bDidInitCrypkey)
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
  char SiteCode[64];
  int err=MyGetSiteCode(SiteCode);
  Dlg.m_SiteCode = SiteCode;
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
  Strng Path = gs_License.GetAppPath();
  Path.Upper();
  unsigned int MB = (Path[0]=='C' || Path[0]=='D') ? MB_YESNO|MB_DEFBUTTON2 : MB_YESNO|MB_ICONSTOP|MB_DEFBUTTON2;
  sprintf(Buff, "Issue or update license at:\n\n%s", gs_License.GetAppPath());
  if (AfxMessageBox(Buff, MB)!=IDYES)
    return FALSE;

  CWaitCursor Wait;
#if !BYPASSLICENSING
  int err=MySaveSiteKey(key);
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
  Dlg.m_sPath = m_sLastPath;
  if (Dlg.DoModal()==IDOK)
    {
    CWaitCursor Wait;
    m_sLastPath = Dlg.m_sPath;
    int err=MyRegisterTransfer((char*)(const char*)m_sLastPath);
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
  sprintf(Buff, "Number of Copies allowed from this site : %d", MyGetNumCopies());
  CTransferDlg Dlg("Transfer Out", "Enter path find registration file:", Buff);
  Dlg.m_sPath = m_sLastPath;
  if (Dlg.DoModal()==IDOK)
    {
    CWaitCursor Wait;
    m_sLastPath = Dlg.m_sPath;
    int err=MyTransferOut((char*)(const char*)m_sLastPath);
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
  if (m_State.m_bLicensed)
    sprintf(Buff, "Number of Copies allowed from this site : %d", MyGetNumCopies());
  CTransferDlg Dlg("Transfer In", "Enter path find transfer files:", Buff);
  Dlg.m_sPath = m_sLastPath;
  if (Dlg.DoModal()==IDOK)
    {
    CWaitCursor Wait;
    m_sLastPath = Dlg.m_sPath;
    int err=MyTransferIn((char*)(const char*)m_sLastPath);
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
  sprintf(Buff1, "Number of Copies allowed from this site : %d", MyGetNumCopies());
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
    MyEndCrypkey();
    if (m_bUseCOM)
      s_ptr.Release();
    
    m_bDidInitCrypkey = 0;
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
    int err=MyDirectTransfer((char*)(const char*)Dlg.m_sPath);
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
  if (!m_State.m_bDemoMode)
    {
    m_State.m_bDemoMode = 1;
    /*CWinApp* pApp = AfxGetApp();
    if (pApp)
      {
      //if (ScdPFMachine.RdInt(LicINISection, "InDemoMode", 1)==0)
      //  Error("%s no longer licensed. Now in demo mode.", CK_AppName);
      ScdPFMachine.RdInt(LicINISection, "InDemoMode", bDemoMode);
      }*/
    }
  m_State.m_dwOpLevel = FixOptions(GetDemoOptions());
  }

//---------------------------------------------------------------------------

void CLicense::Error(char * fmt, ...)
  {
  char Buff[2048];
  va_list argptr;
  va_start(argptr, fmt);
  vsprintf(Buff, fmt, argptr);
  va_end(argptr);
  if (1 || m_bEmbedded)
    {
    LogError("License", 0, "%s", Buff);
    }
  else
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

    ICrypKeySDKFloatRecordServerPtr ptrFloat;
    ptrFloat.CreateInstance ("CrypKey.SDKFloatRecordServer");
    //printf ("\nptrFloat:%x\n", ptrFloat);

    // check FloatingLicenseSnapshot  (0.03)
    long nEntries;
    err= s_ptr->FloatingLicenseTakeSnapshot (&nEntries);
    Cnt=nEntries;
    //printf ("\nReturn from FloatingLicenseTakeSnapshot: %d  (%d)\n", nError, nEntries);

    for (int i=0; i<nEntries;i++)
      {
      //CComBSTR cbName = "Test";


    //ptrFloat->put_strComputerName (cbName);
      err = s_ptr->FloatingLicenseGetRecord (i, (ICrypKeySDKFloatRecordServer *) ptrFloat);
      //cbError.Empty ();
      //cbError.AppendBSTR (ptr->GetLastError ());
    //strError = cbError;
    //printf ("\nReturn from FloatingLicenseGetRecord: (%d) %s\n", nError, (LPCTSTR) strError);

      if (ptrFloat != NULL)
        {
        ptrFloat->get_nId((long*)&fls[i].id);
        ptrFloat->get_nUpdate((long*)&fls[i].update);
        long sw;
        ptrFloat->get_nStatus(&sw);
        fls[i].status=(short)sw;
        
        // ?? ptrFloat->get_nTimestamp((long*)&fls[i].starttime);
        fls[i].starttime=0;

        _bstr_t bsUser;
        ptrFloat->get_strUserName (&bsUser.GetBSTR());
        strcpy(fls[i].userName, (LPCTSTR)bsUser);

        _bstr_t bsComp;
        ptrFloat->get_strComputerName (&bsComp.GetBSTR());
        strcpy(fls[i].computerName, (LPCTSTR)bsComp);
        }
      }

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
    //err = s_ptr->FloatingLicenseTakeSnapshot((sizeof(fls), &Cnt, &fls[0]);
    //// Also GetRecord
    //err = s_ptr->FloatingLicenseGetRecord(sizeof(fls), &Cnt, &fls[0]);
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
  sprintf(Buff, "%s  Number of Copies allowed from this site : %d\n", Buff, MyGetNumCopies());
  //sprintf(Buff, "%s  Number of Network Users allowed from this site : %d\n", Buff, MyGetNumMultiUsers());
  //sprintf(Buff, "%s  Site Code : %s\n", Buff, GetSiteCode2());

//NBNB TODO CHECK THIS LEVEL !!!!

  sprintf(Buff, "%s  Level : %d\n", Buff, MyGetLevel(CK_NumDefinedLevels));
  /*sprintf(Buff, "%s  Level : (1) %d\n", Buff, get_level(1));
  sprintf(Buff, "%s  Level : (2) %d\n", Buff, get_level(2));
  sprintf(Buff, "%s  Level : (3) %d\n", Buff, get_level(3));
  sprintf(Buff, "%s  Level : (4) %d\n", Buff, get_level(4));
  sprintf(Buff, "%s  Level : (5) %d\n", Buff, get_level(5));*/

  if (MyGet1RestInfo(1)==0)
    strcat(Buff, "  Time Restrictions : None\n");
  else
    sprintf(Buff, "%s  Time Restrictions : No of days allowed:%d  No of days used:%d\n", Buff, MyGet1RestInfo(2), MyGet1RestInfo(3));
  int authopt = 0;
  int num_allowed = 0;
  int num_used = 0;
  ULONG start_date = 0;
  int ret = MyGetRestrictionInfo(&authopt, &start_date, &num_allowed, &num_used);
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
    strcat(Buff, MyGetOption(32, i) ? "1" : "0");
    }
  long d;
  int err = MyGetAuthorization(&d, 0);
  dbgpln("CrypKey GetAuth C:%08x", d);
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
  int ret = MyGetRestrictionInfo(&authopt, &start_date, &num_allowed, &num_used);
  if (ret==0)
    sprintf(Buff, "Date and time license last updated : %s", gs_License.SGTime(start_date));
  else
    sprintf(Buff, "Date and time license last updated : Unknown");
  SetDlgItemText(IDC_CK_TXT_DATE, Buff);
  sprintf(Buff, "Number of Copies allowed from this site : %d", gs_License.DemoMode() ? 0 : MyGetNumCopies());
  SetDlgItemText(IDC_CK_TXT_COPIES, Buff);
  sprintf(Buff, "Number of multi-users that the site has been granted : %d", gs_License.DemoMode() ? 0 : MyGetNumMultiUsers());
  SetDlgItemText(IDC_CK_TXT_USERS, Buff);
  //sprintf(Buff, "Number of current network users : %d", GetCurrentNetUsers());
  //SetDlgItemText(IDC_CK_TXT_CURRENTUSERS, Buff);
  if (gs_License.DemoMode())
    sprintf(Buff, "Time Restrictions : None (using SysCAD Demo Mode)");
  else
    {
    if (MyGet1RestInfo(1)==0)
      sprintf(Buff, "Time Restrictions : None");
    else
      sprintf(Buff, "Time Restrictions : No of days allowed:%d  No of days used:%d", MyGet1RestInfo(2), MyGet1RestInfo(3));
    }
  SetDlgItemText(IDC_CK_TXT_TIME, Buff);
  int iLast=0;
  if (gs_License.MaxNodesAllowed(false)==CK_InfiniteUnits)
    iLast=sprintf(Buff, "Limit on number of ProBal units allowed : None");
  else
    {
    iLast=sprintf(Buff, "Limit on number of ProBal units allowed : %d", gs_License.MaxNodesAllowed(false));
    if (DefNetProbalMode())
      iLast+=sprintf(&Buff[iLast], " (%d Used)", gs_License.NodeCount());
    if (gs_License.ProbalLiteMode())
      iLast+=sprintf(&Buff[iLast], " Trial");
    }
  SetDlgItemText(IDC_CK_TXT_PROBALUNITS, Buff);

  if (gs_License.MaxNodesAllowed(true)==CK_InfiniteUnits)
    iLast=sprintf(Buff, "Limit on number of Dynamic units allowed : None");
  else
    {
    iLast=sprintf(Buff, "Limit on number of Dynamic units allowed : %d", gs_License.MaxNodesAllowed(true));
    if (DefNetDynamicMode())
      iLast+=sprintf(&Buff[iLast], " (%d Used)", gs_License.NodeCount());
    if (gs_License.ProbalLiteMode())
      iLast+=sprintf(&Buff[iLast], " Trial");
    }
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
  LicUserVer = (gs_License.DemoMode() || MyGetNumMultiUsers()==0) ? "Stand Alone License" : "Multi-User Network License";
  SetDlgItemText(IDC_CK_TXT_STANDALONE, (const char*)LicUserVer);
  sprintf(Buff, "Location: %s", gs_License.GetAppPath());
  SetDlgItemText(IDC_CK_TXT_LOCATION, Buff);
  int Ver = MyCrypKeyVersion();
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
  for (int i=0; CK_OptionNames[i].m_iBitNumber>=0; i++)
    {
    DWORD msk = (1<<(31-CK_OptionNames[i].m_iBitNumber));
    bool IsYes = ((gs_License.OpLevel() & msk)!=0);
    if (CK_OptionNames[i].m_fNormallyShow || IsYes)
      {
      int j = m_List.InsertItem(i, CK_OptionNames[i].m_pName);
      Item.iItem = j;
      Item.iSubItem = 1;
      Item.pszText = (IsYes ? "Yes" : "");//"No");
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
  m_bDynLiteMode = 0;
  m_bProbalLiteMode = 0;
  m_State.m_bCOMMineServeOn = 0;
  m_pSecOpt = (CK_SysCADSecurity*)&m_State.m_dwOpLevel;

  m_NodeCount = 0;
  m_IllegalNodeCount = 0;
  m_IllegalModelCount = 0;

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
  //Opt.m_Opts.xVer82 = 0;
  //Opt.m_Opts.xVer90 = 1;
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
  Opt.m_Opts.Mdls_SMDKRuntime = 0;
  Opt.m_Opts.Mdls_HeatExtra = 0;
  Opt.m_Opts.Mdls_HeatBal = 1;
  Opt.m_Opts.Mdls_Alumina = 0;
  Opt.m_Opts.Mdls_SizeDist = 0;
  //other...
  //Opt.m_Opts.xOld_Dynamic = 0;
  //Opt.m_Opts.xOld_Probal = 0;
  //Opt.m_Opts.xOld_Dynamic82 = 0;
  //Opt.m_Opts.xOld_Probal82 = 0;
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
  //Opt.m_Opts.xVer82 = 0;
  //Opt.m_Opts.xVer90 = 1;
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
  Opt.m_Opts.Mdls_SMDKRuntime = 1;
  Opt.m_Opts.Mdls_HeatExtra = 1;
  Opt.m_Opts.Mdls_HeatBal = 1;
  Opt.m_Opts.Mdls_Alumina = 1;
  Opt.m_Opts.Mdls_SizeDist = 1;
  //other...
  Opt.m_Opts.Client_Other = 0;
  //Opt.m_Opts.xOld_Dynamic = 0;
  //Opt.m_Opts.xOld_Probal = 0;
  //Opt.m_Opts.xOld_Dynamic82 = 0;
  //Opt.m_Opts.xOld_Probal82 = 0;
  return Opt.m_OpLevel;
  }

//---------------------------------------------------------------------------

void CSysCADLicense::CheckForLiteModes()
  {
  m_bDynLiteMode = 0;
  m_bProbalLiteMode = 0;
  if (m_bDidInitCrypkey && !Blocked() && !DemoMode())
    {
    if (AllowProBal() && !AllowDynamicFlow() && !AllowDynamicFull())
      {
      m_bDynLiteMode = 1;
      }
    else if (!AllowProBal() && (AllowDynamicFlow() || AllowDynamicFull()))
      {
      m_bProbalLiteMode = 1;
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

//int CSysCADLicense::xProbalUnitsAllowed()
//  {
//  if (bDemoMode)
//    return CK_Demo_ProbalUnits;
//  if (m_bProbalLiteMode)
//    return CK_Lite_ProbalUnits;
//  if (m_pSecOpt->m_Opts.Level==CK_TrialLevel)
//    return CK_Trial_ProbalUnits;
//  return CK_InfiniteUnits; 
//  }
//
////---------------------------------------------------------------------------
//
//int CSysCADLicense::xDynUnitsAllowed()
//  {
//  if (bDemoMode)
//    return CK_Demo_DynUnits;
//  if (m_bDynLiteMode)
//    return CK_Lite_DynUnits;
//  if (m_pSecOpt->m_Opts.Level==CK_TrialLevel)
//    return CK_Trial_DynUnits;
//  return CK_InfiniteUnits; 
//  }

//---------------------------------------------------------------------------

void CSysCADLicense::BumpNodeCount(int Count, LPTSTR Tag)
  { 
  m_NodeCount+=Count; 
  dbgpln("%5i) %+2i %s", m_NodeCount, Count, Tag); 
  };

//---------------------------------------------------------------------------

int CSysCADLicense::MaxNodesAllowed(BOOL ForDynamic)
  {
  if (m_State.m_bDemoMode)
    return ForDynamic ? CK_Demo_DynUnits : CK_Demo_ProbalUnits;

  if (ForDynamic && m_bDynLiteMode)
    return CK_Lite_DynUnits;

  if (!ForDynamic && m_bProbalLiteMode)
    return CK_Lite_ProbalUnits;

  if (m_pSecOpt->m_Opts.Level==CK_TrialLevel)
    return ForDynamic ? CK_Trial_DynUnits :CK_Trial_ProbalUnits;
  return CK_InfiniteUnits; 
  }

//---------------------------------------------------------------------------

int CSysCADLicense::MaxNodesAllowed()
  {
  return MaxNodesAllowed(DefNetDynamicMode()); 
  };

//---------------------------------------------------------------------------

int CSysCADLicense::NodeCount()
  {
  return m_NodeCount; 
  };

//---------------------------------------------------------------------------

BOOL CSysCADLicense::NodeCountExceeded(int Extra, eLicOptions Opts)
  {
  BOOL X = NodeCount()+Extra > MaxNodesAllowed();
  if (X && Opts!=eLic_None)
    LogError("SysCAD", (Opts==eLic_MsgBox? LF_DoAfxMsgBox:0)|LF_Exclamation, "Maximum number of units allowed exceeds limit of %d allowed by %s license", MaxNodesAllowed(), (DefNetProbalMode() ? "ProBal" : "Dynamic"));
  return X;
  };

//---------------------------------------------------------------------------

int  CSysCADLicense::IllegalNodeCount(eLicOptions Opts)                 
  { 
  if (m_IllegalNodeCount>0 && Opts!=eLic_None)
    LogError("SysCAD", (Opts==eLic_MsgBox? LF_DoAfxMsgBox:0)|LF_Exclamation, "Project contains %d units not allowed by %s license", m_IllegalNodeCount, (DefNetProbalMode() ? "ProBal" : "Dynamic"));
  return m_IllegalNodeCount; 
  };

//---------------------------------------------------------------------------

void CSysCADLicense::BumpIllegalNodeCount(int Count, LPTSTR ClassId, eLicOptions Opts)
  { 
  m_IllegalNodeCount+=Count; 
  //dbgpln("%5i) %+2i %s", m_IllegalNodeCount, Count); 
  if (Opts!=eLic_None)
    LogNote("SysCAD", (Opts==eLic_MsgBox? LF_DoAfxMsgBox:0)|LF_Exclamation, "Class %s not Licensed", ClassId);
  };

//---------------------------------------------------------------------------

int  CSysCADLicense::IllegalModelCount(eLicOptions Opts)                 
  { 
  if (m_IllegalModelCount>0 && Opts!=eLic_None)
    LogError("SysCAD", (Opts==eLic_MsgBox? LF_DoAfxMsgBox:0)|LF_Exclamation, "Project contains %d Models not allowed by %s license", m_IllegalModelCount, (DefNetProbalMode() ? "ProBal" : "Dynamic"));
  return m_IllegalModelCount; 
  };

//---------------------------------------------------------------------------

void CSysCADLicense::BumpIllegalModelCount(int Count, LPTSTR ClassId, eLicOptions Opts)
  { 
  m_IllegalModelCount+=Count; 
  //dbgpln("%5i) %+2i %s", m_IllegalModelCount, Count, Tag); 
  if (Opts!=eLic_None)
    LogNote("SysCAD", (Opts==eLic_MsgBox? LF_DoAfxMsgBox:0)|LF_Exclamation, "Class %s not Licensed", ClassId);
  };

//---------------------------------------------------------------------------

long CSysCADLicense::MaxHistSizeAllowed()
  {
  if (m_State.m_bDemoMode)
    return CK_DemoHistSize;
  //if (AllowFullHist())
  //  return CK_InfiniteHistSize; 
  if (m_pSecOpt->m_Opts.Level==CK_TrialLevel)
    return CK_TrialHistSize;
  return CK_InfiniteHistSize;
  //return CK_NormalHistSize;
  }

//---------------------------------------------------------------------------

UINT CSysCADLicense::MaxHistFilesAllowed()
  {
  if (m_State.m_bDemoMode)
    return CK_DemoHistFiles;
  //if (AllowFullHist())
  //  return CK_InfiniteHistFiles; 
  if (m_pSecOpt->m_Opts.Level==CK_TrialLevel)
    return CK_TrialHistFiles;
  return CK_InfiniteHistFiles;
  //return CK_NormalHistFiles;
  }

//---------------------------------------------------------------------------

int CSysCADLicense::TrendWindowsAllowed() 
  { 
  if (m_State.m_bDemoMode)
    return CK_DemoTrends;
  if (m_pSecOpt->m_Opts.Level==CK_TrialLevel)
    return CK_TrialTrends;
  return CK_InfiniteTrends; 
  }

//---------------------------------------------------------------------------

int CSysCADLicense::GraphicWindowsAllowed() 
  {
  if (m_State.m_bDemoMode)
    return CK_DemoGrfs;
  if (m_pSecOpt->m_Opts.Level==CK_TrialLevel)
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
  if (AllowMdlsSMDKRuntime())
    dw |= TOC_SMDKRUNTIME;
  if (AllowMdlsElec())
    dw |= TOC_POWERDIST;

  if (AllowMdlsUser())
    dw |= TOC_USER;
  if (AllowMdlsQAL())
    dw |= TOC_QAL;
  if (AllowMdlsQAL()) //  if (AllowMdlsQALExtra())
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
  if (m_bUseCOM)
    {
    bool Done=true;
    ScdMainWnd()->PostMessage(WMU_SETLICENSE, 0x0100|(On?1:0), (LPARAM)&Done); 
    while (!Done)
      Sleep(50);
    }
  else
    {
    m_State.m_bCOMMineServeOn = On; 
    m_State.m_dwOpLevel=FixOptions(m_State.m_dwOpLevel);
    Check();
    if (ScdMainWnd())
      ScdMainWnd()->PostMessage(WMU_UPDATEMAINWND, SUB_UPDMAIN_BACKGROUND, 0);
    }
  };               

//---------------------------------------------------------------------------

void CSysCADLicense::SetForMineServeMsg(WPARAM wParam, LPARAM lParam)  
  { 
  m_State.m_bCOMMineServeOn = (wParam&01)!=0; 
  m_State.m_dwOpLevel=FixOptions(m_State.m_dwOpLevel);
  Check();
  *((bool*)lParam)=true;
  if (ScdMainWnd())
    ScdMainWnd()->PostMessage(WMU_UPDATEMAINWND, SUB_UPDMAIN_BACKGROUND, 0);
  };               

//---------------------------------------------------------------------------

void CSysCADLicense::SetAsRunTime()            { m_pSecOpt->m_Opts.Func_FullEdit = 0; };
BOOL CSysCADLicense::IsRunTime()               { return !m_pSecOpt->m_Opts.Func_FullEdit; };
//BOOL CSysCADLicense::AllowVer90()              { return m_pSecOpt->m_Opts.Ver90 && !m_State.m_bBlocked; };
BOOL CSysCADLicense::AllowMdlsRTTS()           { return m_pSecOpt->m_Opts.Client_RTTS && !m_State.m_bBlocked; };
BOOL CSysCADLicense::ForMineServe()            { return m_pSecOpt->m_Opts.Client_MineServe && !m_State.m_bBlocked; };
BOOL CSysCADLicense::AllowMdlsAlcan()          { return m_pSecOpt->m_Opts.Client_Alcan && !m_State.m_bBlocked; };
//BOOL CSysCADLicense::AllowMdlsQALExtra()       { return m_pSecOpt->m_Opts.Client_QALExtra && !m_State.m_bBlocked; };
BOOL CSysCADLicense::AllowMdlsQAL()            { return m_pSecOpt->m_Opts.Client_QAL && !m_State.m_bBlocked; };
BOOL CSysCADLicense::AllowMdlsUser()           { return m_pSecOpt->m_Opts.Client_Other && !m_State.m_bBlocked; };
BOOL CSysCADLicense::AllowMdlsSMDKRuntime()    { return m_pSecOpt->m_Opts.Mdls_SMDKRuntime && !m_State.m_bBlocked; };
BOOL CSysCADLicense::AllowMdlsHeatExtra()      { return m_pSecOpt->m_Opts.Mdls_HeatExtra && !m_State.m_bBlocked; };
BOOL CSysCADLicense::AllowMdlsHeatBal()        { return m_pSecOpt->m_Opts.Mdls_HeatBal && !m_State.m_bBlocked; };
BOOL CSysCADLicense::AllowMdlsAlumina()        { return m_pSecOpt->m_Opts.Mdls_Alumina && !m_State.m_bBlocked; };
BOOL CSysCADLicense::AllowMdlsSizeDist()       { return m_pSecOpt->m_Opts.Mdls_SizeDist && !m_State.m_bBlocked; };
BOOL CSysCADLicense::AllowMdlsElec()           { return m_pSecOpt->m_Opts.Mdls_Electrical && !m_State.m_bBlocked; };
BOOL CSysCADLicense::AllowProBal()             { return m_pSecOpt->m_Opts.Mode_ProBal && !m_State.m_bBlocked; };
BOOL CSysCADLicense::AllowDynamicFlow()        { return m_pSecOpt->m_Opts.Mode_DynamicFlow && !m_State.m_bBlocked; };
BOOL CSysCADLicense::AllowDynamicFull()        { return m_pSecOpt->m_Opts.Mode_DynamicFull && !m_State.m_bBlocked; };
BOOL CSysCADLicense::AllowProBalLite()         { return m_bProbalLiteMode && !m_State.m_bBlocked; };
BOOL CSysCADLicense::AllowDynamicLite()        { return m_bDynLiteMode && !m_State.m_bBlocked; };
BOOL CSysCADLicense::OnlySteadyState()         { return m_pSecOpt->m_Opts.Only_SteadyState; }
BOOL CSysCADLicense::IsAcademic()              { return m_pSecOpt->m_Opts.Func_Academic; }
BOOL CSysCADLicense::AllowFullLic()            { return m_pSecOpt->m_Opts.Func_FullEdit && !m_State.m_bBlocked; };
BOOL CSysCADLicense::AllowFullLicFlag()        { return m_pSecOpt->m_Opts.Func_FullEdit; };
BOOL CSysCADLicense::AllowDrivers()            { return m_pSecOpt->m_Opts.Func_Drivers && !m_State.m_bBlocked; };
BOOL CSysCADLicense::AllowOPCServer()          { return m_pSecOpt->m_Opts.Func_OPCServer && !m_State.m_bBlocked; };
BOOL CSysCADLicense::AllowCOMInterface()       { return m_pSecOpt->m_Opts.Func_COM && !m_State.m_bBlocked; };
BOOL CSysCADLicense::AllowCOMProps()           { return m_pSecOpt->m_Opts.Func_COMProp && !m_State.m_bBlocked; };
UCHAR CSysCADLicense::Level()                  { return (UCHAR)(m_pSecOpt->m_Opts.Level); };


BOOL CSysCADLicense::AllowDynamic()            { return AllowDynamicFlow() || AllowDynamicFull() ; };
BOOL CSysCADLicense::ProBalOK()                { return AllowProBal() || AllowProBalLite(); };
BOOL CSysCADLicense::DynamicOK()               { return AllowDynamic() || AllowDynamicLite(); };

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
