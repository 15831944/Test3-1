//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "syscadlicver.h"
#include "license.h"

#if CK_LICENSINGON
#include "resource.h"
#if CK_USE6525
#include "crypkey.6525.h"
#elif CK_USE6134 
#include "crypkey.6134.h"
#else
#include "crypkey.57.h"
#endif
#include "winsvc.h"
#include "math.h"

//===========================================================================
//=== Generic Code...

char* CK_KeyFile = "syscad.exe";
char* CK_AppName = "SysCAD";

/*//for example.exe...
#define CK_USER_KEY   "D050 815C D1A2 A79D B1"
#define CK_MASTER_KEY "2A5D 57C4 1B4C 135B F09E 17F7 600B 2D70 79E8 F275 C36A"

#define CK_COMPANYNUM 79560      //these are the correct numbers for example.exe
#define CK_PASSNUM    984534120  //you can use them in your tests if you need to
*/

#if CK_USE6525

#define CK_USER_KEY   "DE5E 81ED E83A 12EB 81D2 C0FA 1B"
#define CK_MASTER_KEY "b4135aa5ec82997f53c5efcd0567ae710af3de57c4a72a798183\
b5ea90b391591a6c2f8863b89b2b7be27bd2553e3e2557d2bec1daad173a09bf256da1e7d92\
b9bb0377dbc5e431d7fbb95ba17d8806560be6355949e144dc84cd8e72f2732c8aa3048874f\
75de558a920b0454c75c047574aa890d2423bef09575995f30d05"

#elif CK_USE6134 

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


//const int CK_NetworkChecktime = 600;
const int CK_NetworkChecktime = 900;
const int CK_TrialVerNo = 1;          //Trial license version number
const int CK_NumDefinedLevels = 2; //number of bits to be used for level
const int CK_NumDefinedOpts = 30;

#if CK_USE6525
#pragma comment(lib, "CRP32M60.6525.lib")            
#elif CK_USE6134 
#pragma comment(lib, "CRP32M60.6134.lib")            
#else
#pragma comment(lib, "CRP32M60.57.lib")
#endif

//===========================================================================
//=== KWA Specific Code...

const int CK_NoOfOptions = 28;
struct CK_OptionDetails
  {
  unsigned char iBitNumber;
  char* pName;
  bool fNormallyShow;
  };

//require off    : 0,1,2,3,6,12,13,20,21
//require on     : 19
//SMDK units     : 14
//SMDK properties: 15

/*CK_OptionDetails CK_OptionNames[CK_NoOfOptions] =
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
  {  4, "Unused", false },
  {  5, "Unused", false },
  { 12, "Unused", false },
  { 13, "Unused", false },
  { 20, "Unused", false },
  { 21, "Unused", false },
  { 27, "Unused", false },
    };

char* CK_LevelNames[2] =
  { "Trial",
    "Normal"
    };*/

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
  SetDlgItemText(IDC_CK_APP_VERSION, Lic.GetAppVersion());
  sprintf(Buff, "Issue &Trial License (%d days)", CK_NoOfTrialDays);
  SetDlgItemText(IDC_CK_TRIAL, Buff);
  return TRUE;
  }

//---------------------------------------------------------------------------

void CStartLicenseDlg::OnIssue() 
  {
  if (Lic.IssueLicense())
    OnOK();
  }

//---------------------------------------------------------------------------

void CStartLicenseDlg::OnTrial() 
  {
  if (Lic.IssueTrial(CK_NoOfTrialDays, TRUE))
    OnOK();
  }

//---------------------------------------------------------------------------

void CStartLicenseDlg::OnRegtrans() 
  {
  Lic.DoRegisterTransfer();
  }

//---------------------------------------------------------------------------

void CStartLicenseDlg::OnTransin() 
  {
  if (Lic.DoTransferIn())
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
    int m_LocationOpt;
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
    afx_msg void OnUpdateLocation(CCmdUI* pCmdUi);
    DECLARE_MESSAGE_MAP()
  };

//---------------------------------------------------------------------------

CLicenseLocationDlg::CLicenseLocationDlg(CWnd* pParent /*=NULL*/)
  : CDialog(CLicenseLocationDlg::IDD, pParent)
  {
  //{{AFX_DATA_INIT(CLicenseLocationDlg)
  m_AppPath = _T("");
  m_LocationOpt = 0;
  //}}AFX_DATA_INIT
  }

//---------------------------------------------------------------------------

void CLicenseLocationDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CLicenseLocationDlg)
  DDX_Text(pDX, IDC_CK_APPPATH, m_AppPath);
  DDX_Radio(pDX, IDC_CK_LOCATIONOPTION, m_LocationOpt);
  //}}AFX_DATA_MAP
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CLicenseLocationDlg, CDialog)
  //{{AFX_MSG_MAP(CLicenseLocationDlg)
  ON_BN_CLICKED(IDC_CK_BROWSE, OnBrowse)
  ON_BN_CLICKED(IDC_CK_LOCATIONOPTION, OnLocationOption)
  ON_BN_CLICKED(IDC_CK_LOCATIONOPTION2, OnLocationOption)
  //}}AFX_MSG_MAP
  ON_UPDATE_COMMAND_UI(IDC_CK_BROWSE, OnUpdateLocation)
  ON_UPDATE_COMMAND_UI(IDC_CK_APPPATH, OnUpdateLocation)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CLicenseLocationDlg::OnInitDialog() 
  {
  CDialog::OnInitDialog();
  UpdateDialogControls(this, FALSE);
  return TRUE;
  }

//---------------------------------------------------------------------------

void CLicenseLocationDlg::OnUpdateLocation(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(m_LocationOpt==1);
  }

//---------------------------------------------------------------------------

void CLicenseLocationDlg::OnLocationOption() 
  {
  UpdateData(TRUE);
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

void CLicenseLocationDlg::OnBrowse() 
  {
  UpdateData(TRUE);
  char Buff[128];
  sprintf(Buff, "%s (%s)|%s||", CK_AppName, CK_KeyFile, CK_KeyFile);

  CFileDialog Dlg(TRUE, NULL, CK_KeyFile, OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, Buff);
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
  if (Lic.GetAppPath())
    sprintf(Buff, "Location: Remote %s", Lic.GetAppPath());
  else
    sprintf(Buff, "Location: Current");
  SetDlgItemText(IDC_CK_TXT_LOCATION, Buff);
  return TRUE;
  }

//---------------------------------------------------------------------------

void CAuthLicenseDlg::OnOK() 
  {
  UpdateData(TRUE);
  bValid = (Lic.DoIssue((char*)(const char*)m_SiteKey));
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
  if (Lic.GetAppPath())
    {
    sLicLocation = "Location: Remote ";
    sLicLocation += Lic.GetAppPath();
    }
  else
    sLicLocation = "Location: Current";
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
  CFileDialog Dlg(TRUE, NULL, CK_KeyFile, OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, Buff);
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

CLicense::CLicense()
  {
  bDidInitCrypkey = 0;
  bMultiUserFailure = 0;
  bLicensed = 0;
  bSMDK_Units = 0;
  bSMDK_Props = 0;
  bDemoMode = 0;
  bTrialMode = 0;
  bTrialFailed = 0;
  sLastPath = "A:\\";
  sAppPath = "";
  dwOpLevel = 0;
  iDaysLeft = 0;
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
  if (bDidInitCrypkey)
    EndCrypkey();
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
  //if (!FileExists(LongPath))
  //  {
  //  Error("Specified license file (or folder) not found!\n\n%s\n\nCheck license location!", LongPath);
  //  return FALSE;
  //  }
  if (GetShortPathName(LongPath, ShortPath, sizeof(ShortPath))<1)
    return FALSE;
  int err = InitCrypkey(ShortPath, CK_MASTER_KEY, CK_USER_KEY, FALSE, CK_NetworkChecktime);
//char bbb[1024];
//sprintf(bbb, "InitCrypkey:%d  (Path:%s)", err, ShortPath);
//AfxMessageBox(bbb);
  if (err==NETWORK_NO_CKSERVE)
    {
    if (IsWIN32)
      {//do this on Windows NT and Windows 2000...
      if (UpdateCrypkeyINI(LongPath))
        {//try again...
        err = InitCrypkey(ShortPath, CK_MASTER_KEY, CK_USER_KEY, FALSE, CK_NetworkChecktime);
        if (err)
          {
          Error("Initialization Failure %d after Update INI\n%s for %s", err, ExplainErr(EXP_INIT_ERR, err), LongPath);
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
        Error("Initialization Failure %d\n%s for %s", err, ExplainErr(EXP_INIT_ERR, err), LongPath);
      }
    return FALSE;
    }

  int Ver = CrypkeyVersion();
  if (Ver==41 || Ver==42)
    {//version 4.1 and 4.2 are old and not supported...
    Error("Old Crypkey version (%d) not supported", Ver);
    return FALSE;
    }
  else if (Ver==43)
    {//version 4.3 is old but should work...
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
    {//version 5.6 should work...
    //Error("Old Crypkey version (%d)", Ver);
    //return FALSE;
    }
#if CK_USE6134 
  else if (Ver!=61) //Ver 6.1 : May 2006
    {//expect version 6.1
    Error("Incorrect Crypkey version (%d)", Ver);
    return FALSE;
    }
#elif CK_USE6525
  else if (Ver!=65) //Ver 6.5 : August 2006 
    {//expect version 6.5
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
  if (bDidInitCrypkey)
    EndCrypkey();
  bDidInitCrypkey = 0;
  }

//---------------------------------------------------------------------------

BOOL CLicense::Check(BOOL Prompt /*=FALSE*/)
  {
  ASSERT(bDidInitCrypkey);
  CWaitCursor Wait;
  bLicensed = 0;
  bSMDK_Units = 0;
  bSMDK_Props = 0;
  bMultiUserFailure = 0;
  dwOpLevel = 0;
  iDaysLeft = 0;
  int err = GetAuthorization(&dwOpLevel, 0); //check authorization, use up 0 runs
dwOpLevel=0x00021000 | 0x00010000;
//char bbb[1024];
//sprintf(bbb, "GetAuthorization:%d  (dwOpLevel:%08x)", err, dwOpLevel);
//AfxMessageBox(bbb);

  //Use the challenge function to check the library is not an impostor
  //This only needs to be done if you are using the DLL
  if (err==0) //check this only if we think we are authorized
    {
    //generate some random numbers - this can be done any way you like
    //random1 = time(NULL);
    //random2 = random1*time(NULL);
    ULONG random1 = (time(NULL)<<2)*5;
    ULONG random2 = random1*time(NULL)*rand();
    ULONG result1 = Challenge32(CK_COMPANYNUM, CK_PASSNUM/2, random1, random2);
    ULONG result2 = CKChallenge32(random1, random2);
    if (result1 != result2)
      {
      Error("Security Failure.  Challenge function failed");
      return FALSE;
      }

    bool SMDK_Err = CheckSMDK();
    if (SMDK_Err)
      {
      dwOpLevel = 0;
      Error("License error. Invalid options");
      return FALSE;
      }
    else
      {
      bLicensed = 1;
      DWORD dw = (DWORD)pow(2.0, CK_NumDefinedLevels) - 1;
      bTrialMode = 0;//((dwOpLevel & dw)==CK_TrialLevel);
      iDaysLeft = (Get1RestInfo(1)==0) ? CK_InfiniteDays : Get1RestInfo(2) - Get1RestInfo(3);
      bDemoMode = 0;
      CWinApp* pApp = AfxGetApp();
      if (pApp)
        pApp->WriteProfileInt("License", "InDemoMode", bDemoMode);
      }
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
      Error("Get Authorization failed!\nReturned %d : %s", err, ExplainErr(EXP_AUTH_ERR, err));
    }
  return FALSE;
  }

//---------------------------------------------------------------------------

BOOL CLicense::QuickCheck(UCHAR CheckLevel/*=0*/)
  {
  ASSERT(bDidInitCrypkey);
  if (bBlocked)
    return FALSE;
  if (CheckLevel==0)
    {
    if (!bDemoMode && !bLicensed)
      {
      SetBlocked();
      Error("Security Failure.  License blocked");
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

  if (err==0 && CheckLevel>1)
    {
    //generate some random numbers - this can be done any way you like
    ULONG random1 = (time(NULL)<<1)+9;
    ULONG random2 = random1+rand();
    ULONG result1 = Challenge32(CK_COMPANYNUM, CK_PASSNUM/2, random1, random2);
    ULONG result2 = CKChallenge32(random1, random2);
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

  if (err==0 && OtherErr==0)
    {
    bool SMDK_Err = CheckSMDK();
    if (SMDK_Err)
      {
      dwOpLevel = 0;
      Error("License error. Invalid options");
      return FALSE;
      }
    else
      bLicensed = 1;
    }
  else
    {
    dwOpLevel = 0;
    SetBlocked();
    if (OtherErr==0)
      Error("Security Failure.  License blocked\nReturned %d : %s", err, ExplainErr(EXP_AUTH_ERR, err));
    else
      Error("Security Failure.  License blocked (%d)", OtherErr);
    return FALSE;
    }
  return TRUE;
  }

//---------------------------------------------------------------------------

BOOL CLicense::CheckSMDK()
  {
  //require off    : 0,1,2,3,6,12,13,20,21
  //require on     : 19
  //SMDK units     : 14
  //SMDK properties: 15
  /*DWORD msk = (DWORD)pow(2.0, 31-0) |
                (DWORD)pow(2.0, 31-1) |
                (DWORD)pow(2.0, 31-2) |
                (DWORD)pow(2.0, 31-3) |
                (DWORD)pow(2.0, 31-6) |
                (DWORD)pow(2.0, 31-12) |
                (DWORD)pow(2.0, 31-13) |
                (DWORD)pow(2.0, 31-20) |
                (DWORD)pow(2.0, 31-21);*/
  bSMDK_Units = 0;
  bSMDK_Props = 0;
  bool LicBitErr = ((dwOpLevel & 0xf20c0c00)!=0); //require off : 0,1,2,3,6,12,13,20,21
  if (!LicBitErr)
    LicBitErr = ((dwOpLevel & 0x00001000)!=0x00001000); //require on : 19
  if (!LicBitErr)
    {
    bSMDK_Units = ((dwOpLevel & 0x00020000)==0x00020000);
    bSMDK_Props = ((dwOpLevel & 0x00010000)==0x00010000);
    LicBitErr = (!bSMDK_Units && !bSMDK_Props);
    }
  return (!LicBitErr);
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
  if (Prompt)
    {
    char Buff[256];
    sprintf(Buff, "This will attempt to issue a trial license for %d days\nThis can only be done once - Are you sure?", NoOfDays);
    if (AfxMessageBox(Buff, MB_YESNO|MB_ICONSTOP|MB_DEFBUTTON2)==IDNO)
      return FALSE;
    }
  CWaitCursor Wait;
  DWORD OpLevel = GetTrialOptions();
  int ret = readyToTryDays(OpLevel, NoOfDays, CK_TrialVerNo, 1);
  if (ret)
    {
    Error("Issue Trial License failed!\nReturned %d : %s", ret, ExplainErr(EXP_RTT_ERR, ret));
    bTrialFailed = 1;
    return FALSE;
    }
  Check(); //re-check options etc
  return TRUE;
  }

//---------------------------------------------------------------------------

bool CLicense::ChangeLocation()
  {
  int RetCode = 0;
  int CurOption = (sAppPath.GetLength()==0 ? 0 : 1);
  CLicenseLocationDlg Dlg;
  Dlg.m_AppPath = sAppPath;
  Dlg.m_LocationOpt = CurOption;
  if (Dlg.DoModal()==IDOK)
    {
    if (CurOption!=Dlg.m_LocationOpt || (Dlg.m_LocationOpt==1 && _stricmp((const char*)sAppPath, (const char*)Dlg.m_AppPath)!=0))
      {
      CString PrevAppPath = sAppPath;
      if (Dlg.m_LocationOpt==1)
        sAppPath = Dlg.m_AppPath;
      else
        sAppPath = "";
      CWinApp* pApp = AfxGetApp();
      if (pApp)
        pApp->WriteProfileString("License", "LicenseLocation", (const char*)sAppPath);
      }
    return true;
    }
  return false;
  }

//---------------------------------------------------------------------------

int CLicense::SetLocation(BOOL CheckAndInit/*=true*/)
  {
  int RetCode = 0;
  int CurOption = (sAppPath.GetLength()==0 ? 0 : 1);
  CLicenseLocationDlg Dlg;
  Dlg.m_AppPath = sAppPath;
  Dlg.m_LocationOpt = CurOption;
  if (Dlg.DoModal()==IDOK)
    {
    if (CurOption!=Dlg.m_LocationOpt || (Dlg.m_LocationOpt==1 && _stricmp((const char*)sAppPath, (const char*)Dlg.m_AppPath)!=0))
      {
      CString PrevAppPath = sAppPath;
      if (Dlg.m_LocationOpt==1)
        sAppPath = Dlg.m_AppPath;
      else
        sAppPath = "";
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
          sprintf(Buff, "Set license to new location anyway?\n\nOld location : %s\nNew location : %s", PrevAppPath.GetLength()>0 ? (const char*)PrevAppPath : "current", sAppPath.GetLength()>0 ? (const char*)sAppPath : "current");
          if (AfxMessageBox(Buff, MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2)!=IDYES)
            {
            Wait.Restore();
            sAppPath = PrevAppPath;
            EndCrypkey();
            bDidInitCrypkey = 0;
            if (!Init())
              Error("Change location\nFailed to re-initialize license!");
            else if (!Check(TRUE)) //re-check options etc
              Error("Change location\nFailed to re-initialize license!");
            return 2; //new location failed
            }
          else
            {
            #if CK_ALLOWDEMOMODE
            Lic.SetDemoMode();
            #endif
            RetCode = 4; //new location failed - but set it anyway
            }
          }
        else
          RetCode = 1; //new location OK
        }
      else
        RetCode = 5; //location changed, no Init and check
      CWinApp* pApp = AfxGetApp();
      if (pApp)
        pApp->WriteProfileString("License", "LicenseLocation", (const char*)sAppPath);
      }
    else
      RetCode = 3; //unchanged
    }
  return RetCode; //cancel
  }

//---------------------------------------------------------------------------

BOOL CLicense::IssueLicense()
  {
  char SiteCode[64];
  int err = GetSiteCode(SiteCode);
  if (err) //let the user authorize
    {
    Error("Get site code failed!\nReturned %d : %s", err, ExplainErr(EXP_GET_SITECODE_ERR, err));
    return FALSE;
    }

  CAuthLicenseDlg Dlg;
  Dlg.m_SiteCode = SiteCode;
  Dlg.DoModal();
  if (Dlg.bValid)
    {
    CWinApp* pApp = AfxGetApp();
    if (pApp)
      pApp->WriteProfileString("License", "LastSiteCode", SiteCode);
    Check(); //re-check options etc
    return TRUE;
    }
  return FALSE;
  }

//---------------------------------------------------------------------------

BOOL CLicense::DoIssue(char* key)
  {
  if (key==NULL || strlen(key)==0)
    {
    Error("Valid Site Key required");
    return FALSE;
    }
  if (Lic.GetAppPath())
    {
    char Buff[1024];
    sprintf(Buff, "Issue/update license on remote location?\n\n%s", Lic.GetAppPath());
    if (AfxMessageBox(Buff, MB_YESNO|MB_ICONSTOP|MB_DEFBUTTON2)!=IDYES)
      return FALSE;
    }

  CWaitCursor Wait;
  int err = SaveSiteKey(key);
  switch (err)
    {
    case AUTH_OK: // Valid Key
      {//Site Key Accepted
      CWinApp* pApp = AfxGetApp();
      if (pApp)
        pApp->WriteProfileString("License", "LastSiteKey", key);
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
  }

//---------------------------------------------------------------------------

BOOL CLicense::DoRegisterTransfer()
  {
  CTransferDlg Dlg("Register Transfer", "Enter path to place registration file:", "");
  Dlg.m_sPath = sLastPath;
  if (Dlg.DoModal()==IDOK)
    {
    CWaitCursor Wait;
    sLastPath = Dlg.m_sPath;
    int err = RegisterTransfer((char*)(const char*)sLastPath);
    if (err==0)
      return TRUE;
    Error("Register Transfer failed!\n%d : %s", err, ExplainErr(EXP_REG_ERR, err));
    }
  return FALSE;
  }

//---------------------------------------------------------------------------

BOOL CLicense::DoTransferOut()
  {
  char Buff[256];
  sprintf(Buff, "Number of Copies allowed from this site : %d", GetNumCopies());
  CTransferDlg Dlg("Transfer Out", "Enter path find registration file:", Buff);
  Dlg.m_sPath = sLastPath;
  if (Dlg.DoModal()==IDOK)
    {
    CWaitCursor Wait;
    sLastPath = Dlg.m_sPath;
    int err = TransferOut((char*)(const char*)sLastPath);
    if (err==0)
      {
      Check(); //re-check options etc
      return TRUE;
      }
    Error("Transfer Out failed!\n%d : %s", err, ExplainErr(EXP_TO_ERR, err));
    }
  return FALSE;
  }

//---------------------------------------------------------------------------

BOOL CLicense::DoTransferIn()
  {
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
    int err = TransferIn((char*)(const char*)sLastPath);
    if (err==0)
      {
      Check(); //re-check options etc
      return TRUE;
      }
    Error("Transfer In failed!\n%d : %s", err, ExplainErr(EXP_TI_ERR, err));
    }
  return FALSE;
  }

//---------------------------------------------------------------------------

BOOL CLicense::DoDirectTransfer()
  {
  char Buff[512];
  sprintf(Buff, "Enter the path of the copy of %s to transfer license to:", CK_AppName);
  char Buff1[256];
  sprintf(Buff1, "Number of Copies allowed from this site : %d", GetNumCopies());
  CTransferDlg Dlg("Direct Transfer", Buff, Buff1, TRUE);
  char LongPath[_MAX_PATH];
  if (GetAppPath())
    {
    strcpy(LongPath, GetAppPath());
    }
  else
    {
    if (GetModuleFileName(NULL, LongPath, sizeof(LongPath))<1)
      return FALSE;
    }
  char Drv[_MAX_DRIVE];
  char Dir[_MAX_DIR];
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
    int err = DirectTransfer((char*)(const char*)Dlg.m_sPath);
    if (err==0)
      {
      Check(); //re-check options etc
      return TRUE;
      }
    Error("Direct Transfer failed!\n%d : %s", err, ExplainErr(EXP_DT_ERR, err));
    }
  return FALSE;
  }

//---------------------------------------------------------------------------

BOOL CLicense::Kill(CString& ConfirmCode)
  {
  if (AfxMessageBox("This will delete your license - Are you sure?", MB_YESNO|MB_ICONSTOP|MB_DEFBUTTON2)==IDYES)
    {
    CWaitCursor Wait;
    ConfirmCode = "";
    char s[64];
    int ret = KillLicense(s);
    if (ret==0)
      {
      ConfirmCode = s;
      CWinApp* pApp = AfxGetApp();
      if (pApp)
        pApp->WriteProfileString("License", "LastKillCode", s);
      Check(); //re-check options etc
      return TRUE;
      }
    else
      Error("Kill License failed!\n%d : ", ret, ExplainErr(EXP_KL_ERR, ret));
    }
  return FALSE;
  }

//---------------------------------------------------------------------------

void CLicense::SetDemoMode()
  {
  if (!bDemoMode)
    {
    bDemoMode = 1;
    CWinApp* pApp = AfxGetApp();
    if (pApp)
      {
      //if (pApp->GetProfileInt("License", "InDemoMode", 1)==0)
      //  Error("%s no longer licensed. Now in demo mode.", CK_AppName);
      pApp->WriteProfileInt("License", "InDemoMode", bDemoMode);
      }
    }
  dwOpLevel = GetDemoOptions();
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
  WIN32_FIND_DATA FileInfo;
  HANDLE H = FindFirstFile(Buff, &FileInfo);
  BOOL Valid = (H != INVALID_HANDLE_VALUE);
  if (!Valid)
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
  FLS_REC fls[64];
  int Cnt = 0;
  int err = FloatingLicenseSnapshot(sizeof(fls), &Cnt, &fls[0]);
  if (err<0)
    {
    Error("Get Floating License Snapshot Failure %d\n%s", err, ExplainErr(EXP_INIT_ERR, err));
    return FALSE;
    }
  char Buff[2048];
  Buff[0] = 0;
  for (int i=0; i<Cnt; i++)
    sprintf(Buff, "%s%s: %s  %s   %s\n", Buff, fls[i].status==0 ? "OK" : "Blocked", fls[i].userName, fls[i].computerName, SGTime(fls[i].starttime));
  AfxMessageBox(Buff);
  return TRUE;
  }

//---------------------------------------------------------------------------

BOOL CLicense::NetworkUsersInfo(char* Buff)
  {
  Buff[0] = 0;
  FLS_REC fls[64];
  int Cnt = 0;
  int err = FloatingLicenseSnapshot(sizeof(fls), &Cnt, &fls[0]);
  if (err<0)
    {
    //Error("Get Floating License Snapshot Failure %d\n%s", err, ExplainErr(EXP_INIT_ERR, err));
    return FALSE;
    }
  for (int i=0; i<Cnt; i++)
    sprintf(Buff, "%s%s: %s  %s   %s\n", Buff, fls[i].status==0 ? "OK" : "Blocked", fls[i].userName, fls[i].computerName, SGTime(fls[i].starttime));
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
  if (err==0)
    sprintf(Buff, "%s (%04X %04X)\n", Buff, HIWORD(d), LOWORD(d));
  else
    strcat(Buff, "\n");
  
  AfxMessageBox(Buff);
  }

//===========================================================================
//=== App License Specific Code...

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
    sprintf(Buff, "Date and time license last updated : %s", Lic.SGTime(start_date));
  else
    sprintf(Buff, "Date and time license last updated : Unknown");
  SetDlgItemText(IDC_CK_TXT_DATE, Buff);
  sprintf(Buff, "Number of Copies allowed from this site : %d", Lic.DemoMode() ? 0 : GetNumCopies());
  SetDlgItemText(IDC_CK_TXT_COPIES, Buff);
  sprintf(Buff, "Number of multi-users that the site has been granted : %d", Lic.DemoMode() ? 0 : GetNumMultiUsers());
  SetDlgItemText(IDC_CK_TXT_USERS, Buff);
  //sprintf(Buff, "Number of current network users : %d", GetCurrentNetUsers());
  //SetDlgItemText(IDC_CK_TXT_CURRENTUSERS, Buff);
  if (Lic.DemoMode())
    sprintf(Buff, "Time Restrictions : None (using Demo Mode)");
  else
    {
    if (Get1RestInfo(1)==0)
      sprintf(Buff, "Time Restrictions : None");
    else
      sprintf(Buff, "Time Restrictions : No of days allowed:%d  No of days used:%d", Get1RestInfo(2), Get1RestInfo(3));
    }
  SetDlgItemText(IDC_CK_TXT_TIME, Buff);
  /*if (Lic.DemoMode())
    sprintf(Buff, "Demo mode");
  else
    sprintf(Buff, "%s level", CK_LevelNames[Lic.Level()]);
  SetDlgItemText(IDC_CK_TXT_LEVEL, Buff);*/
  CString LicUserVer;
  LicUserVer = (Lic.DemoMode() || GetNumMultiUsers()==0) ? "Stand Alone License" : "Multi-User Network License";
  SetDlgItemText(IDC_CK_TXT_STANDALONE, (const char*)LicUserVer);
  if (Lic.GetAppPath())
    sprintf(Buff, "Location: Remote %s", Lic.GetAppPath());
  else
    sprintf(Buff, "Location: Current");
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
  /*for (int i=0; i<CK_NoOfOptions; i++)
    {
    DWORD msk = (DWORD)pow(2.0, 31-CK_OptionNames[i].iBitNumber);
    bool IsYes = ((Lic.OpLevel() & msk)!=0);
    if (CK_OptionNames[i].fNormallyShow || IsYes)
      {
      int j = m_List.InsertItem(i, CK_OptionNames[i].pName);
      Item.iItem = j;
      Item.iSubItem = 1;
      Item.pszText = (IsYes ? "Yes" : "No");
      m_List.SetItem(&Item);
      }
    }*/
  int j = m_List.InsertItem(0, "SMDK Units");
  Item.iItem = j;
  Item.iSubItem = 1;
  Item.pszText = (Lic.SMDK_Units() ? "Yes" : "No");
  m_List.SetItem(&Item);
  j = m_List.InsertItem(1, "SMDK Properties");
  Item.iItem = j;
  Item.iSubItem = 1;
  Item.pszText = (Lic.SMDK_Props() ? "Yes" : "No");
  m_List.SetItem(&Item);

  sprintf(Buff, "License Information   [ %s ]", (const char*)LicUserVer);
  SetWindowText(Buff);
  //UpdateData(FALSE);
  //UpdateDialogControls(this, FALSE);
  return TRUE;
  }

//===========================================================================

CKWALicense::CKWALicense()
  {
  pSecOpt = (CK_KWASecurity*)&dwOpLevel;
  }

//---------------------------------------------------------------------------

char* CKWALicense::GetAppVersion()
  {
  //FullCompany();
  //return FullVersion();
  return "TODO";
  }

//---------------------------------------------------------------------------

DWORD CKWALicense::GetDemoOptions()
  {
  CK_KWASecurity Opt;
  Opt.OpLevel = 0; //clears all bits
  Opt.Opts.PBOpt1 = 0;
  Opt.Opts.PBOpt2 = 0;
  return Opt.OpLevel;
  }

//---------------------------------------------------------------------------

DWORD CKWALicense::GetTrialOptions()
  {
  CK_KWASecurity Opt;
  Opt.OpLevel = 0; //clears all bits
  Opt.Opts.PBOpt1 = 0;
  Opt.Opts.PBOpt2 = 0;
  return Opt.OpLevel;
  }

//---------------------------------------------------------------------------

void CKWALicense::Info()
  {
  CLicenseInfoDlg Dlg;
  Dlg.DoModal();
  }

//---------------------------------------------------------------------------
#endif

CKWALicense Lic;

//===========================================================================
//place code similar to this in application init instance...

//  #if CK_LICENSINGON
//  if (!Lic.Init())
//    return FALSE;
//  if (!Lic.Check())
//    {
//    #if CK_ALLOWDEMOMODE
//    Lic.SetDemoMode(TRUE);
//    #else
//    if (!Lic.StartDialog())
//      return FALSE;
//    if (!Lic.Check()) //check again!
//      return FALSE;
//    #endif
//    }
//  #endif


