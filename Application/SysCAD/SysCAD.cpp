// SysCAD.cpp : Defines the class behaviors for the application.

#include "stdafx.h"
#include "SysCAD.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "SysCADDoc.h"
#include "SysCADView.h"
#include <initguid.h>
#include "SysCAD_i.c"
#include "ScdApplication.h"
#include "ScdAppTag.h"
#include "ScdAppTags.h"
#include "ScdGraphic.h"
#include "ScdGraphics.h"
#include "ScdTrend.h"
#include "ScdTrends.h"
#include "ScdHistorian.h"
#include "ScdLicenseApp.h"
#include "ScdOptions.h"
#include "ScdProject.h"
#include "ScdReplay.h"
#include "ScdReport.h"
#include "ScdReports.h"
#include "ScdSnapshot.h"
#include "ScdOPCServer.h"
#include "ScdIOMarshal.h"
#include "ScdDDEServer.h"

#include "accnode.h"
#include "docroot.h"
#include "grfdoc.h"
#include "prjdoc.h"
#include "prjview.h"
#include "project.h"
#include "tagvdlgs.h"
#include "tagvsplt.h"
#include "tagvtext.h"
#include "ordwnd.h"
#include "statswnd.h"
#include "msgwnd.h"
#include "mainfrm.h"
#include "helpinfo.h"
#include "scd_wm.h"
#include "dbgmngr.h"
#include "basecmds.h"
#include "ole_mngr.h"
#include "rep_mngr.h"
#include "cmd_mngr.h"
#include "dlgbusy.h"
#include "drv_mngr.h"
#include "marshal.h"
#include "archmngr.h"
#include "hstmain.h"
#include "wndslct.h"
#include "explorescd.h"
#include "scdcmdif.h"
#include "licbase.h"
#include "filedlgs.h"
#include "revhist.h"
#include "electricalimport.h"
#include "elnkwiring.h"
#include "wirepanel.h"
#include ".\syscad.h"
#include ".\opcsrvrwrapper.h"
#include "scdver.h"
//#include "prjsettingsdlg.h"
#include "slvcfg.h"

//===========================================================================

//===========================================================================

#define SECURITY_WIN32
#include "security.h"
#pragma comment(lib, "Secur32.lib")

//#include "optoff.h"

#import "c:\program files\common files\system\ado\msado15.dll" rename("EOF", "adEOF") implementation_only
#import "c:\Program Files\Common Files\system\ado\msadox.dll" implementation_only

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//===========================================================================

#define BREAKINTODEBUGGER     0

//===========================================================================

void StartAppComThread(DWORD EvtMsgID);
void StopAppComThread();

//CMDIChildWnd* gs_pAccFrame=NULL;
//AccNodeWnd*   gs_pAccWnd=NULL;
BaseCmdBlk*   gs_pBaseCmds=NULL;
COleExec*     gs_pXOle=NULL;
//CCmdExec*     gs_pXCmd=NULL;
CRepExec*     gs_pXRep=NULL;
CAccDoc       gs_AccWndDoc; //access window MUST have a document for printing

extern DWORD gs_ServerWait; // Defined in ScdApplication.cpp

//===========================================================================

class CFrm : public CMDIChildWnd
  {
  DECLARE_DYNCREATE(CFrm)
  public:
    //{{AFX_VIRTUAL(CFrm)
    //}}AFX_VIRTUAL
  protected:
    //{{AFX_MSG(CFrm)
    afx_msg void OnClose();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  };

IMPLEMENT_DYNCREATE(CFrm, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CFrm, CMDIChildWnd)
  //{{AFX_MSG_MAP(CFrm)
  ON_WM_CLOSE()
  ON_WM_SIZE()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CFrm::OnClose()
  {
  //CMDIChildWnd::OnClose(); Do NOT call base, we are not allowed to close this window!
  }

void CFrm::OnSize(UINT nType, int cx, int cy)
  {
  CMDIChildWnd::OnSize(nType, cx, cy);
  if (nType==SIZE_MAXIMIZED)
    CWindowLists::SetWndMaxMode(true);
  else if (nType==SIZE_MINIMIZED || nType==SIZE_RESTORED)
    CWindowLists::SetWndMaxMode(false);
  }

/////////////////////////////////////////////////////////////////////////////
// CSysCADApp

//===========================================================================

#define WINNT_ONLY 0

//===========================================================================

inline flag EnablePrjOK() { return gs_pPrj && gs_pPrj->pPrjDoc; };
inline flag EnableNoPrj() { return gs_pPrj && !gs_pPrj->pPrjDoc; };
inline flag EnableFiling() { return gs_Exec.Initialised() && gs_Exec.Filing(); };
inline flag EnableNotFiling() { return gs_Exec.Initialised() && !gs_Exec.Filing(); };
inline flag EnableBusy() { return gs_pPrj && gs_pPrj->pPrjDoc && gs_Exec.Initialised() && gs_Exec.Busy(); };
inline flag EnableNotBusy() { return gs_pPrj && gs_pPrj->pPrjDoc && gs_Exec.Initialised() && !gs_Exec.Busy(); };//Running(); //&& !gs_Exec.SolvingPB(); };
inline flag EnableAnalysing() { return gs_pPrj && gs_pPrj->pPrjDoc && gs_pPrj->bBusyAnalysing; };
inline flag EnableNotAnalysing() { return gs_pPrj && gs_pPrj->pPrjDoc && !gs_pPrj->bBusyAnalysing; };
inline flag EnableStopped() { return gs_pCmd && gs_pCmd->IsDlgBusy(); };
inline flag EnableNotStopped() { return gs_pCmd && !gs_pCmd->IsDlgBusy(); };
inline flag EnableActive() { return (gs_pPrj && gs_pPrj->pPrjDoc && gs_Exec.Initialised() && gs_Exec.Busy()) || EnableFiling() || EnableStopped(); };

BEGIN_MESSAGE_MAP(CSysCADApp, CWinApp)
  //{{AFX_MSG_MAP(CSysCADApp)
  ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
  ON_COMMAND(ID_HELP_SYSCAD, OnHelpSyscad)
  ON_COMMAND(ID_HELP_PGMLANGUAGE, OnHelpPgm)
  ON_COMMAND(ID_HELP_UserDocs, OnHelpModels)
  ON_COMMAND(ID_PROJECT_SAVEALL, OnProjectSaveall)
  ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
#ifdef SYSCAD10
  ON_COMMAND(ID_FILE_DUMMYINSERT, DummyInsert)
  ON_COMMAND(ID_FILE_DUMMYDELETE, DummyDelete)
#endif
  ON_COMMAND(ID_FILE_NEW, OnFileNew)
#ifdef SYSCAD10
  ON_UPDATE_COMMAND_UI(ID_FILE_DUMMYINSERT, OnUpdateDummyInsert)
  ON_UPDATE_COMMAND_UI(ID_FILE_DUMMYDELETE, OnUpdateDummyDelete)
#endif
  ON_UPDATE_COMMAND_UI(ID_FILE_NEW, OnUpdateFileNew)
  ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, OnUpdateFileOpen)
  ON_UPDATE_COMMAND_UI(ID_PROJECT_SAVEALL, OnUpdateProjectSaveall)
  ON_COMMAND(ID_PROJECT_OPEN, OnProjectOpen)
  ON_UPDATE_COMMAND_UI(ID_PROJECT_OPEN, OnUpdateProjectOpen)
  ON_COMMAND(ID_PROJECT_OPENLOCAL, OnProjectOpenLocal)
  ON_UPDATE_COMMAND_UI(ID_PROJECT_OPENLOCAL, OnUpdateProjectOpenLocal)
  ON_COMMAND(ID_PROJECT_CLOSE, OnProjectClose)
  ON_UPDATE_COMMAND_UI(ID_PROJECT_CLOSE, OnUpdateProjectClose)
  ON_COMMAND(ID_PROJECT_CLOSE_NOSAVE, OnProjectCloseNoSave)
  ON_UPDATE_COMMAND_UI(ID_PROJECT_CLOSE_NOSAVE, OnUpdateProjectCloseNoSave)
  ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
  ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateFileSaveAs)
  ON_UPDATE_COMMAND_UI(ID_FILE_CLOSE, OnUpdateFileClose)
  ON_UPDATE_COMMAND_UI(ID_APP_EXIT, OnUpdateAppExit)
  ON_COMMAND(ID_PROJECT_NEW, OnProjectNew)
  ON_UPDATE_COMMAND_UI(ID_PROJECT_NEW, OnUpdateProjectNew)
  //}}AFX_MSG_MAP
  // Standard file based document commands
  //ON_COMMAND(ID_FILE_NEW, CScdWinApp::OnFileNew)
  //ON_COMMAND(ID_FILE_OPEN, CScdWinApp::OnFileOpen)
  // Standard print setup command
  ON_COMMAND(ID_FILE_PRINT_SETUP, OnFilePrintSetup)
  ON_COMMAND_RANGE(ID_CK_LIC_NETUSERS, ID_CK_LIC_RESETSERVICE, OnLicense)
  ON_UPDATE_COMMAND_UI_RANGE(ID_CK_LIC_NETUSERS, ID_CK_LIC_RESETSERVICE, OnUpdateLicense)
  ON_COMMAND_EX_RANGE(ID_FILE_MRU_FILE1, ID_FILE_MRU_FILE16, OnOpenRecentFile)
  // Global help commands
  ON_COMMAND(ID_HELP_INDEX, OnHelpIndex)
  ON_COMMAND(ID_HELP_USING, OnHelpUsing)
  ON_COMMAND(ID_HELP, OnHelp)
  ON_COMMAND(ID_CONTEXT_HELP, OnContextHelp)
  ON_COMMAND(ID_DEFAULT_HELP, OnHelpIndex)
  ON_COMMAND(ID_PROJECT_EDIT_SETTINGS, OnProjectEditSettings)
  ON_UPDATE_COMMAND_UI(ID_PROJECT_EDIT_SETTINGS, OnUpdateProjectEditSettings)

  //  ON_COMMAND(ID_APP_EXIT, OnAppExit)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSysCADApp construction

CSysCADApp::CSysCADApp()
  {
  memset(pTemplate, 0, sizeof(pTemplate)/sizeof(pTemplate[0]));
  m_TimeOfLastIdle=0;
  m_WithLocalCopy=false;
  m_HoldAddToRecentFileList=0;
  }

/////////////////////////////////////////////////////////////////////////////
// The one and only CSysCADApp object

CSysCADApp gs_SysCADApp;

// This identifier was generated to be statistically unique for your app.
// You may change it if you prefer to choose a specific identifier.

// {74E84C3E-62E2-4758-A403-B62A1BB7093B}
//TODO Get this correct

static const CLSID clsid =
  { 0x74e84c3e, 0x62e2, 0x4758, { 0xa4, 0x3, 0xb6, 0x2a, 0x1b, 0xb7, 0x9, 0x3b } };

/////////////////////////////////////////////////////////////////////////////
// CSysCADApp initialization

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
  {
  public:
    CAboutDlg();

    // Dialog Data
    //{{AFX_DATA(CAboutDlg)
    enum { IDD = IDD_ABOUTBOX };
    //}}AFX_DATA

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CAboutDlg)
  protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

    // Implementation
  protected:
    //{{AFX_MSG(CAboutDlg)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  };

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
  {
  //{{AFX_DATA_INIT(CAboutDlg)
  //}}AFX_DATA_INIT
  }

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CAboutDlg)
  //}}AFX_DATA_MAP
  }

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
  //{{AFX_MSG_MAP(CAboutDlg)
  // No message handlers
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog()
  {
  CDialog::OnInitDialog();
  Strng s;
  SetDlgItemText(IDC_SYSCAD_VERSION, FullVersion());
  SetDlgItemText(IDC_ACOPYRIGHT, CopyrightNotice());
  SetDlgItemText(IDC_ACOMPANY, FullCompany());
  SetDlgItemText(IDC_WEBLOCATION, "www.syscad.net");
  s.Set("Build date: %s", BuildDate());
  SetDlgItemText(IDC_BUILDDATE, s());
  s.Set("Distributed by %s", FullCompany2());
  SetDlgItemText(IDC_ACOMPANY2, s());
  s.Set("First issue: Build %d.%d (%s)", SCD_BUILD_NUM, SCD_BUILD_SVN, SCD_BUILD_DATE);
  SetDlgItemText(IDC_PATCH, s());
  return TRUE;  // return TRUE  unless you set the focus to a control
  }

//---------------------------------------------------------------------------

class CMyCommandLineInfo : public CCommandLineInfo
  {
  public:
    bool m_bRegServer;

    CMyCommandLineInfo ()
      {
      m_bRegServer=false;
      };
    virtual void ParseParam(const char* pszParam, BOOL bFlag, BOOL bLast)
      {
      if (_stricmp(pszParam, "RegServer")==0)
        m_bRegServer=true;
      else
        CCommandLineInfo::ParseParam(pszParam, bFlag, bLast);
      };
  };

// App command to run the dialog
void CSysCADApp::OnAppAbout()
  {
  CAboutDlg aboutDlg;
  aboutDlg.DoModal();
  }

//---------------------------------------------------------------------------

flag DirectoryExists(char * Root, char *File)
  {
  flag Found=0;
  char DataLib[1024];
  strcpy(DataLib, Root);
  strcat(DataLib, "\\");
  strcat(DataLib, File);

  WIN32_FIND_DATA  FindFileData;
  HANDLE FFH=FindFirstFile(DataLib, &FindFileData);
  if (FFH!=INVALID_HANDLE_VALUE)
    {
    Found=1;
    FindClose(FFH);
    }
  return Found;
  }

//---------------------------------------------------------------------------

int RegisterReqdOCX(LPCSTR pszDllName)
  {
  int iReturn = 0;
  HRESULT (STDAPICALLTYPE * lpDllEntryPoint)(void);

  LPCSTR pszDllEntryPoint = "DllRegisterServer";

  SetErrorMode(SEM_FAILCRITICALERRORS);       // Make sure LoadLib fails.
  //HINSTANCE hLib = LoadLibraryEx(pszDllName, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
  HINSTANCE hLib = AfxLoadLibrary(pszDllName);

  if (hLib < (HINSTANCE)HINSTANCE_ERROR)
    {
    char buff[256];
    sprintf(buff, "'%s' not Loaded", pszDllName);
    ::MessageBox(NULL, buff, "SysCAD", MB_OK|MB_ICONEXCLAMATION|MB_SETFOREGROUND|MB_TASKMODAL);
    strcat(buff, "\n");
    TRACE(buff);
    // LogError("SysCAD", LF_Exclamation, "'%s' not Loaded", pszDllName);
    iReturn = 1;
    goto CleanupOle;
    }

  // Find the entry point.
  (FARPROC&)lpDllEntryPoint = GetProcAddress(hLib, pszDllEntryPoint);

  if (lpDllEntryPoint == NULL)
    {
    char buff[256];
    sprintf(buff, "Entry point 'DllRegisterServer' not found in '%s'", pszDllName);
    ::MessageBox(NULL, buff, "SysCAD", MB_OK|MB_ICONEXCLAMATION|MB_SETFOREGROUND|MB_TASKMODAL);
    strcat(buff, "\n");
    TRACE(buff);
    //LogError("SysCAD", LF_Exclamation, "Entry point 'DllRegisterServer' not found in '%s'", pszDllName);
    iReturn = 2;
    goto CleanupLibrary;
    }

  // Call the entry point.
  if (FAILED((*lpDllEntryPoint)()))
    {
    char buff[256];
    sprintf(buff, "DLL Registration failed '%s'\n\nDo you have Administrator rights ?", pszDllName);
    ::MessageBox(NULL, buff, "SysCAD", MB_OK|MB_ICONEXCLAMATION|MB_SETFOREGROUND|MB_TASKMODAL);
    strcat(buff, "\n");
    TRACE(buff);
    //LogError("SysCAD", LF_Exclamation, "DLL Registration failed '%s'", pszDllName);
    iReturn = 3;
    goto CleanupLibrary;
    }

CleanupLibrary:
  if (lDoFreeLibraries)
    AfxFreeLibrary(hLib);

CleanupOle:
  //<VISIO>  OleUninitialize();

  return iReturn;
  }

//---------------------------------------------------------------------------

struct tRqdVersionNos
  {
  char * ModuleName;
  DWORD MSH, MSL, LSH, LSL;
  BOOL InstallMessage;
  };

static BOOL ModuleVersionOK(tRqdVersionNos & RV)
  {
  HMODULE hMSVCRT=GetModuleHandle(RV.ModuleName);
  //ASSERT(hMSVCRT);
  char Fn[1024];
  if (hMSVCRT && GetModuleFileName(hMSVCRT, Fn, sizeof(Fn))>0)
    {
    DWORD dwHandle;
    DWORD dwVerSize=::GetFileVersionInfoSize(Fn, &dwHandle);
    if (dwVerSize)
      {
      char * Buff=new char[dwVerSize];
      if (::GetFileVersionInfo(Fn, dwHandle, dwVerSize, Buff))
        {
        VS_FIXEDFILEINFO * pInfo;
        UINT InfoLen;
        if (::VerQueryValue(Buff, "\\", (LPVOID*)&pInfo, &InfoLen))
          {
          DWORD MS = (DWORD)MAKELONG(RV.MSL, RV.MSH);
          DWORD LS = (DWORD)MAKELONG(RV.LSL, RV.LSH);

          if ((pInfo->dwFileVersionMS>MS) ||
            (pInfo->dwFileVersionMS==MS) && (pInfo->dwFileVersionLS>=LS))
            {
            delete[] Buff;
            return true;
            }
          else
            {
            Strng Msg;
            if (RV.InstallMessage)
              Msg.Set("Incorrect version for module : %s\n(Version number should be >= %i.%i.%i.%i)\n\nAdministrator privileges required to install this file,\nSysCAD should be installed by a user local admin rights!\nRestart after install required?",/*\n\nRun setup utility for %s and restart computer.",*/
              RV.ModuleName, RV.MSH, RV.MSL, RV.LSH, RV.LSL);//, RV.ModuleName);
            else
              Msg.Set("Incorrect version for module : %s\n\n\n(Version number should be >= %i.%i.%i.%i)\n\nPossible problem is that SysCAD was not installed by a user with local Administrator privileges.",
              RV.ModuleName, RV.MSH, RV.MSL, RV.LSH, RV.LSL);
            AfxMessageBox(Msg());
            return false;
            }
          }
        }
      delete[] Buff;
      }
    }
  Strng Msg;
  Msg.Set("Unable to get Version information for %s", RV.ModuleName);
  AfxMessageBox(Msg());
  return false;
  }

//---------------------------------------------------------------------------
// This identifier was generated to be statistically unique for your app.
// You may change it if you prefer to choose a specific identifier.

// {AA4335C0-C827-11d3-A3F9-00A0C9ED9512} : SysCAD
static const GUID SysCADAppclsid =
  { 0xaa4335c0, 0xc827, 0x11d3, { 0xa3, 0xf9, 0x0, 0xa0, 0xc9, 0xed, 0x95, 0x12 } };
// {AA4335C1-C827-11d3-A3F9-00A0C9ED9512} : Automation Dispatch
// {AA4335C2-C827-11d3-A3F9-00A0C9ED9512} : SysCAD.AutoDocument
// {AA4335C3-C827-11d3-A3F9-00A0C9ED9512} : VisGrfDoc Dispatch
// {AA4335C4-C827-11d3-A3F9-00A0C9ED9512} : SysCAD.Document
// {AA4335C5-C827-11d3-A3F9-00A0C9ED9512} : SysCAD Library
// {AA4335C6-C827-11d3-A3F9-00A0C9ED9512} : Spares
// {AA4335C7-C827-11d3-A3F9-00A0C9ED9512}
// {AA4335C8-C827-11d3-A3F9-00A0C9ED9512}

//---------------------------------------------------------------------------

static void* SpaceBuffer=NULL;

//---------------------------------------------------------------------------

int handle_program_memory_depletion( size_t size )
  {
  if (SpaceBuffer)
    {
    free(SpaceBuffer);
    SpaceBuffer=0;
    //    return 1;
    }
  AfxThrowMemoryException();
  return 0;
  }

//---------------------------------------------------------------------------

#ifdef   _DEBUG
#define  SET_CRT_DEBUG_FIELD(a) \
  _CrtSetDbgFlag((a) | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG))
#define  CLEAR_CRT_DEBUG_FIELD(a) \
  _CrtSetDbgFlag(~(a) & _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG))
#else
#define  SET_CRT_DEBUG_FIELD(a)   ((void) 0)
#define  CLEAR_CRT_DEBUG_FIELD(a) ((void) 0)
#endif

//---------------------------------------------------------------------------

BOOL CSysCADApp::InitInstStartup()
  {
  struct tRqdVersionNos ReqdVersions[] =
    {
#ifdef _DEBUG
    //{"msvcrtd.dll",  6,0,8337,0,  false},//sp3
    //{"mfc42d.dll",   6,0,8267,0,  false},//sp3
#if !DotNet
      {"msvcrtd.dll",  6,0,8797,0,  false},//sp5
      {"mfc42d.dll",   6,0,8665,0,  false},//sp5
#elif _MSC_VER >= 1400 
      {"msvcr80d.dll",  8,0,50727,42,  true },//.net
      {"mfc80d.dll",    8,0,50727,42,  true },//.net
#else
      {"msvcr71d.dll", 7,0,9466,0,  false},//.net
      {"mfc71d.dll",   7,0,9466,0,  false},//.net
#endif
#else
    //{"msvcrt.dll",   6,0,8337,0,  true },//sp3
    //{"mfc42.dll",    6,0,8267,0,  false},//sp3
#if (!DotNet)
      {"msvcrt.dll",   6,0,8797,0,  true },//sp5
      {"mfc42.dll",    6,0,8665,0,  true},//sp5
#elif (_MSC_VER >= 1400)
      {"msvcr80.dll",  8,0,50727,42,  true },//.net
      {"mfc80.dll",    8,0,50727,42,  true },//.net
#else
      {"msvcr71.dll",  7,0,0x24fa,0,  true },//.net
      {"mfc71.dll",    7,0,0x24fa,0,  true },//.net
#endif
#endif
      //{"msvcrt.dll",  7,0,2600,0,   false},//.net
      {NULL},
    };

  for (int iRV=0; ReqdVersions[iRV].ModuleName; iRV++)
    if (!ModuleVersionOK(ReqdVersions[iRV]))
      return false;

  SpaceBuffer=calloc(1024, sizeof(char));

  // Register existence of a new memory handler.
  _set_new_handler( handle_program_memory_depletion );
  _set_new_mode(1);

#if (_MSC_VER>=1400)
  // 1016 = MAX_ALLOC_DATA_SIZE will fail if larger
  _set_sbh_threshold((size_t)1016);
#else
  _set_sbh_threshold((size_t)1920);
#endif

  SET_CRT_DEBUG_FIELD( _CRTDBG_LEAK_CHECK_DF );

  return TRUE;
  }

//---------------------------------------------------------------------------

BOOL CSysCADApp::InitIniFile()
  {
  Strng RegKey;

  if (!CnmVersion())
    RegKey.Set("SysCAD %i.%i", SCD_VERINFO_V0, SCD_VERINFO_V1);
  else
    RegKey.Set("SysCAD Build %i", SCD_BUILD_NUM);

  free((void*)m_pszProfileName);
  m_pszProfileName=_strdup(RegKey());

#if (USEREGISTRY)
    {
    ScdPFUser.SetUseRegistry(true, HKEY_CURRENT_USER, "Kenwalt", m_pszProfileName);
    ScdPFMachine.SetUseRegistry(true, HKEY_LOCAL_MACHINE, "Kenwalt", m_pszProfileName);
    }
#else
    {
    char UName[4096];
    ULONG Len=sizeof(UName)-1;
    BOOL GotIt=GetUserNameEx(NameSamCompatible, &UName[0], &Len);
    ASSERT_ALWAYS(GotIt, "NameSamCompatible not available", __FILE__, __LINE__);

      char * pName=strchr(UName, '\\');
    ASSERT_ALWAYS(pName!=NULL, "Bad User Name ", __FILE__, __LINE__);
      CString UFn, MFn;
    UFn.Format("%sSysCAD.User.%s.ini", BaseCfgFiles(), pName+1);
    MFn.Format("%sSysCAD.Machine.ini", BaseCfgFiles());
    ScdPFUser.SetProfFilename(UFn);
    ScdPFMachine.SetProfFilename(MFn);
    }
#endif

    const long Cnt = ScdPFMachine.RdLong("General", "StartCount", 0) + 1;
    ScdPFMachine.WrLong("General", "StartCount", Cnt);
    const long CheckCnt = ScdPFMachine.RdLong("General", "StartCount", -1);
    if (CheckCnt!=Cnt)
      {
      LogError("SysCAD", LF_Exclamation, "Unable to write to file in folder '%s'\nCheck read-only attributes or user permissions!", BaseCfgFiles());
      return FALSE;
      }
    //todo: perhaps test ability to create file in this folder
    return TRUE;
  }

//---------------------------------------------------------------------------

BOOL CSysCADApp::InitInstRegistry()
  {
  HKEY hKey;
  LONG H = RegOpenKeyEx(HKEY_CLASSES_ROOT, "SSDataWidgets.SSDBGridCtrl.3", 0, KEY_QUERY_VALUE, &hKey);
  if (H==ERROR_SUCCESS)
    {//todo: check if registered properly and if it exists...
    /*xxx
    if (NotExists)
    {
    RegCloseKey(hKey);
    RegisterReqdOCX("ssdw3b32.ocx");
    }*/
    }
  else
    {
    RegCloseKey(hKey);
    //todo: Only try after testing if user has permision to write to registry
    if (RegisterReqdOCX("ssdw3b32.ocx")>0)
      return false;
    }

  //SysCADCMD Registration
#if WITHCOMCMD

  // Call DllRegisterServer()

  flag ScdCOMDLLOK = false;
  if (1)
    {
    LONG OldErrMode = SetErrorMode(SEM_FAILCRITICALERRORS);
    HMODULE hDLL=NULL;
    Strng E;
    DWORD Err=0;
    char ErrBuff[512];

    hDLL=AfxLoadLibrary("ScdCOM.DLL");
    if (hDLL==NULL)
      {
      Err=GetLastError();
      E.GetSystemErrorStr(Err);
      sprintf(ErrBuff, "AfxLoadLibrary(ScdCOM.DLL) failed!\nError:%d  : %s", Err, E());
      AfxMessageBox(ErrBuff);
      hDLL=LoadLibrary("ScdCOM.DLL");//try again...
      }
    if (hDLL)
      {
      typedef void (*FnP)(void);
      FnP fp = (FnP)GetProcAddress((struct HINSTANCE__ *)hDLL, "DllRegisterServer");
      if (fp)
        {
        (*fp)();
        ScdCOMDLLOK = true;
        }
      else
        AfxMessageBox(_T("ScdCOM.DLL Not registered"));
      }
    else
      {
      Err=GetLastError();
      E.GetSystemErrorStr(Err);
      sprintf(ErrBuff, "LoadLibrary(ScdCOM.DLL) failed!\nError:%d  : %s", Err, E());
      //AfxMessageBox(ErrBuff);
      //AfxMessageBox(_T("ScdCOM.DLL Not loaded"));
      }
    SetErrorMode(OldErrMode);
    }

  if (ScdCOMDLLOK)
    {
    //todo: Only try after testing if user has permision to write to registry
    gs_ScdCmdIF.Register(WMU_COMEVT);
    HRESULT hr=gs_ScdCmdIF.UpdateRegistry(m_hInstance, TRUE);//, OLESTR("AAA"), OLESTR("TTT"));
    if( FAILED(hr) )
      AfxMessageBox(_T("SysCADCmd RegisterServer Failed"));
    }
#endif

  return TRUE;
  }

//---------------------------------------------------------------------------

BOOL CSysCADApp::InitInstDocTemplate()
  {
  // Register the application's document templates.  Document templates
  //  serve as the connection between documents, frame windows and views.

  pTemplate[iProjectTemplate]=new CMultiDocTemplate(
    IDR_PRJVIEW,
    RUNTIME_CLASS(CPrjDoc),
    RUNTIME_CLASS(CPrjFrameWnd),
    RUNTIME_CLASS(CPrjForm));

  pTemplate[iGraphTemplate]=new CMultiDocTemplate(
    IDR_GRFVIEW,
    RUNTIME_CLASS(CGrfDoc),
    RUNTIME_CLASS(CGrfFrameWnd),
    RUNTIME_CLASS(CGrfWnd));

#if WITHVISIODOC
  // Visio Linking Document
  pTemplate[iVisioTemplate] = new CMultiDocTemplate(
    IDR_VISIOGRF,
    RUNTIME_CLASS(CVisGrfDoc),
    RUNTIME_CLASS(CVisGrfFrame), // custom MDI child frame
    RUNTIME_CLASS(CVisGrfView));
  VisioTemplate().SetContainerInfo(IDR_VISIOGRF_CNTR_IP);
#else
  pTemplate[iVisioTemplate] = NULL;
#endif

  /*
  AddDocTemplate(new CMultiDocTemplate(IDR_INSVIEW,
  RUNTIME_CLASS(InsDoc),
  RUNTIME_CLASS(CMDIChildWnd),        // standard MDI child frame
  RUNTIME_CLASS(CGrfWnd)));
  */
  pTemplate[iTrendTemplate]=new CMultiDocTemplate(
    IDR_TRENDVIEW,
    RUNTIME_CLASS(CTagVwDoc),
    RUNTIME_CLASS(CTagVwSplit),
    RUNTIME_CLASS(CTagVwText));

  pTemplate[iControlTemplate]=new CMultiDocTemplate(
    IDR_CTRENDVIEW,
    RUNTIME_CLASS(CTagVwDocC),
    RUNTIME_CLASS(CTagVwSplitC),
    RUNTIME_CLASS(CTagVwTextC));

#if WITHAUTOMATION
  pTemplate[iAutomationTemplate]=new CMultiDocTemplate(IDR_SYSCADAUTOTYPE,
    RUNTIME_CLASS(CAutoDoc),
    RUNTIME_CLASS(CAutoFrame),
    RUNTIME_CLASS(CAutoView));
#else
  pTemplate[iAutomationTemplate] = NULL;
#endif

  for (int i=0; i<getTemplateCount(); i++)
    if (pTemplate[i])
      AddDocTemplate(pTemplate[i]);

  // Connect the COleTemplateServer to the document template.
  //  The COleTemplateServer creates new documents on behalf
  //  of requesting OLE containers by using information
  //  specified in the document template.
#if WITHVISIODOC
  m_VisioServer.ConnectTemplate(SysCADAppclsid, pTemplate[iVisioTemplate], FALSE);
#endif

#if WITHAUTOMATION
  m_AutoServer.ConnectTemplate(SysCADAppclsid, pTemplate[iAutomationTemplate], FALSE);
#endif

  // Register all OLE server factories as running.  This enables the
  //  OLE libraries to create objects from other applications.
  COleTemplateServer::RegisterAll();
  // Note: MDI applications register all server objects without regard
  //  to the /Embedding or /Automation on the command line.

  return TRUE;
  }

//---------------------------------------------------------------------------

BOOL CSysCADApp::InitInstCmdLineOnly()
  {
  if (m_lpCmdLine[0] != '\0')
    {
    CString MainFrameName;
    MainFrameName.LoadString(IDR_MAINFRAME);

    // Check for other versions of syscad
    HWND DT=::GetDesktopWindow();
    HWND TW=::GetTopWindow(DT);
    HWND ScdWnd=NULL;
    while (TW!=NULL && ScdWnd==NULL)
      {
      char Title[512];
      if (GetWindowText(TW, Title, sizeof(Title)))
        if (strstr(Title, (const char *)MainFrameName)!=NULL)
          ScdWnd=TW;
      TW=::GetNextWindow(TW, GW_HWNDNEXT);
      }
    if (ScdWnd)
      {
      // We found another version of ourself.

      // We could communicate any action that our "twin"
      // should now perform based on how the user tried to
      // execute us.

      m_CLH.Parse(m_lpCmdLine);
      if (!m_CLH.bForceNewCopy && !m_CLH.bAutomation)
        {
        //TODO Only some comand line options should be passed on! (eg scripts)
        // Let's defer to it:
        if (m_CLH.bForegroundWnd)
          {
          if (IsIconic(ScdWnd))
            ShowWindow(ScdWnd, SW_RESTORE);
          SetForegroundWindow(ScdWnd);
          }
        // pass command line to twin...
        Strng Tmp;
        if (m_CLH.sCmdLine[0]=='"' || m_CLH.sCmdLine[0]=='\'' || m_CLH.sCmdLine[m_CLH.sCmdLine.Len()-1]=='"' || m_CLH.sCmdLine[m_CLH.sCmdLine.Len()-1]=='\'')
          {
          if (m_CLH.sCmdLine[0]=='"' || m_CLH.sCmdLine[0]=='\'')
            {
            Tmp = "++Quote++";
            Tmp += &(m_CLH.sCmdLine[1]);
            }
          else
            Tmp = m_CLH.sCmdLine;
          if (Tmp[Tmp.Len()-1]=='"' || Tmp[Tmp.Len()-1]=='\'')
            {
            Tmp[Tmp.Len()-1] = '+';
            Tmp += "+Quote++";
            }
          }
        else
          Tmp = m_CLH.sCmdLine;

        ScdPFUser.WrStr("General", "CmdLine", Tmp());
        ::PostMessage(ScdWnd, WMU_CMD, SUB_CMD_CMDLINE, 0);
        return false;
        }
      // continue -- Allow a second version
      }
    }

  return true;
  }

//---------------------------------------------------------------------------

BOOL CSysCADApp::InitInstFolder()
  {
  // Set up Directories ...
  char WDir[1024], PDir[1024];
  bool Found=false, Err1=false,Err2=false,Err3=false;
  if (GetCurrentDirectory(sizeof(WDir), WDir)>0)
    {
    int wdirlen = strlen(WDir);
    if (wdirlen>0 && WDir[wdirlen-1]!='\\')
      {
      strcat(WDir, "\\");
      wdirlen++;
      }
    Strng DirTest(WDir);
    DirTest += "SysCAD.EXE";
    if (wdirlen>4 && _stricmp(&WDir[wdirlen-4], "bin\\")==0 && FileExists(DirTest()))
      WDir[wdirlen-4] = 0;
    SetStartupDirectory(WDir);
    }
  else
    Err1=true;

  if (GetModuleFileName(NULL, PDir, sizeof(PDir))>0)
    {
    SetExeFile(PDir);
    Strng ProgPath;
    ProgPath.FnDrivePath(PDir);
    SetProgFiles(ProgPath());

    // Extend Path
    Strng XPath;
    XPath=ProgFiles();
    XPath+="Secure;";
    XPath+=ProgFiles();
    XPath+="Depends;";

    char OrgPathBuff[16384];
    char NewPathBuff[16384];
    OrgPathBuff[0]=0;
    NewPathBuff[0]=0;
    if (GetEnvironmentVariable("PATH", OrgPathBuff, sizeof(OrgPathBuff))<sizeof(OrgPathBuff))
      {
      strcpy(NewPathBuff, XPath());
      strcat(NewPathBuff, OrgPathBuff);

      SetEnvironmentVariable("PATH", NewPathBuff);
      }

    }
  else
    Err2=true;

  SetDllFilesPath((LPSTR)ScdPFMachine.RdStr("General", "DllPath", ""));

  Strng LFiles;
  if (GetTempPath(sizeof(WDir), WDir)>0)
    LFiles = WDir; // use a sub-folder of the system temp folder
  else
    LFiles = (GetDriveType(StartupDrive())==DRIVE_FIXED) ? StartupDrive() : "C:\\";
  LFiles+="SysCAD\\";
  Strng LDir(LFiles());
  LDir.FnClearEndBSlash();
  if (!FileExists(LDir()) && !CreateDirectory(LFiles(), NULL))
    {
    DWORD e=GetLastError();
    Err3=true;
    }

  SetTemporaryFiles(LFiles());

  // Local Files Must first Exist

  CDbgMngr::StartUp();
  gs_MsgLog.LogFileClose();
  if (Err1)
    LogError("SysCAD", LF_Exclamation, "Cannot GetCurrentFolder");
  if (Err2)
    LogError("SysCAD", LF_Exclamation, "Cannot GetModuleFileName");
  if (Err3)
    LogError("SysCAD", LF_Exclamation, "Cannot Create %s", LFiles());

  Strng BCfg(ProgFiles());
  BCfg.FnClearEndBSlash();
  BCfg.FnDrivePath(BCfg());
  BCfg.FnClearEndBSlash();
  BCfg.FnMakeDataFolder(BCfgFolderName());
  SetBaseCfgFiles(BCfg());

  Strng BC(BaseCfgFiles());
  BC.FnClearEndBSlash();

  if (!FileExists(BC()))
    {//critical error
    LogError("SysCAD", LF_Exclamation, "Folder '%s' missing!", BaseCfgFiles());
    return FALSE;
    }

  SetCfgFiles();
  SetPrjFiles();

  BCfg+="TagDescData.mdb";
  gs_AccessWnds.InitHelp(BCfg());

  return TRUE;
  }

//---------------------------------------------------------------------------

int CSysCADApp::InitInstLicense1(bool Embedded)
  {
#if CK_LICENSINGON
  CDlgBusy::Open("\n\nChecking License");
  /*if (IsWinNT)
  {
  int LicErr = gs_License.CheckService();
  if (LicErr==-3 || LicErr==-4)
  AfxMessageBox("License Service probably not running (check control panel services)");
  else if (LicErr<0)
  AfxMessageBox("Error with License Service. Check the following:\n1) Licensing Service installed?\n2) User permissions\n3) Control panel services\n");
  }*/
  gs_License.SetEmbedded(Embedded);
  if (m_CLH.bAltLicenseLoc)
    gs_License.SetAppPath(m_CLH.sLicenseLoc());
  else
    {
    Strng S,S1,S2;
    char LongPath[_MAX_PATH];
    if (GetModuleFileName(NULL, LongPath, sizeof(LongPath))<1)
      S1 = "c:\\Program Files\\SysCAD91License\\";
    else
      {
#if (1)
      // where is license (SysCAD.exe)
      S1 = LongPath;
      S1 = S1.Left(S1.Len()-16); //strip bin\syscad91.exe from end of string
      S1 += "License\\";
      S2 = S1;
      S2 += "syscad.exe";
      if (!FileExists(S2()))
        {
        S1 = LongPath;
        S1 = S1.Left(S1.Len()-12); //strip syscad91.exe from end of string
        S1 += "";
        }
#else
      S1 = LongPath;
      S1 = S1.Left(S1.Len()-12); //strip syscad.exe from end of string
#endif
      }
    S = ScdPFMachine.RdStr(LicINISection, "LicenseLocation", S1());
    if (S.Len()<1)
      S = S1;
    ScdPFMachine.WrStr(LicINISection, "LicenseLocation", S());
    gs_License.SetAppPath(S());
    }

  gs_License.SetUseCOM(m_CLH.bDeveloper);

  int LicenseRet=gs_License.Init(NULL, ProgFiles());
  switch (LicenseRet)
    {
    case LicInit_OK:
      {
      CWaitCursor Wait1; //old hour glass may be gone
      CDlgBusy::Close();
      if (!gs_License.Check(true))
        {
#if CK_ALLOWDEMOMODE
        LicenseRet=LicInit_GoDemo;
        gs_License.SetDemoMode();
#else
        LicenseFailed = !gs_License.StartDialog();
        if (!LicenseFailed)
          LicenseFailed = !gs_License.Check(); //check again!
#endif
        }
      else if (gs_License.MultiUserFailure())
        {
        if (Embedded)
          gs_License.Error("Unable to obtain license, set SysCAD to demo mode?");
        else if (AfxMessageBox("Unable to obtain license, set SysCAD to demo mode?", MB_YESNO)!=IDYES)
          CDlgBusy::Close();
        }
      break;
      }
    case LicInit_ExitReqd:
      {
      if (AfxMessageBox("License failed to initialise, Set SysCAD to demo mode ?", MB_ICONQUESTION|MB_YESNO)==IDNO)
        break;
      LicenseRet=LicInit_GoDemo;
      }
    case LicInit_GoDemo:
      {
      CWaitCursor Wait1; //old hour glass may be gone
      CDlgBusy::Close();
#if CK_ALLOWDEMOMODE
      gs_License.Error(gs_License.DidInitCrypkey() ? "License failed to initialise, SysCAD set to demo mode" : "License service ERROR!\n\nSysCAD set to demo mode");
      gs_License.SetDemoMode();
#else
      CDlgBusy::Close();
      ////AfxMessageBox("Contact suppliers to License SysCAD");
      return false;
#endif
      break;//return false;
      }
    case LicInit_GoAcademic:
      {
      CWaitCursor Wait1; //old hour glass may be gone
      CDlgBusy::Close();
      break;//return false;
      }
    }
  return LicenseRet;
#else
  return LicInit_OK;
#endif
  };

//---------------------------------------------------------------------------

BOOL CSysCADApp::InitInstLicense2(int LicenseRet)
  {
#if CK_LICENSINGON
  //gs_License.Check();
  if (LicenseRet==LicInit_ExitReqd)
    {
    CDlgBusy::Close();
    return false;
    }
  if (gs_License.Licensed())
    {
    Strng LicDesc, LicUserVer;
    LicUserVer = (gs_License.MultiUsers()>0) ? "Multi-User Network License" : "Stand Alone License";
    //if (!gs_License.AllowVer90())
    //  {
    //  LogError("License", LF_Exclamation, "A SysCAD 9.0 License has NOT been issued!");
    //  LicDesc = "None";
    //  gs_License.SetDemoMode();
    //  }
    //else
      {//check that a license exists for at least one mode...
      if (gs_License.AllowProBal() && gs_License.AllowDynamicFull())
        LicDesc = "ProBal & Dynamic(Full)";
      else if (gs_License.AllowProBal() && gs_License.AllowDynamicFlow())
        LicDesc = "ProBal & Dynamic(Flow)";
      else if (gs_License.AllowProBal() && !gs_License.AllowDynamicFlow() && !gs_License.AllowDynamicFull())
        LicDesc = "ProBal";
      else if (!gs_License.AllowProBal() && gs_License.AllowDynamicFull())
        LicDesc = "Dynamic(Full)";
      else if (!gs_License.AllowProBal() && gs_License.AllowDynamicFlow())
        LicDesc = "Dynamic(Flow)";
      else
        {
        LogError("License", LF_Exclamation, "A license for ProBal and/or Dynamic has NOT been issued!");
        LicDesc = "None";
        gs_License.SetDemoMode();
        }
      }
    if (gs_License.HashDefineCurtin())
      LogNote("License", 0, "Using Curtin University Academic License.");
    else
      {
      if (gs_License.DaysLeft()<=CK_WarnDays)
        LogWarning("License", gs_License.DaysLeft()<=CK_UrgentWarnDays ? 0|LF_Exclamation : 0, "License (%s) expires in %d days.   [ %s ]", LicDesc(), gs_License.DaysLeft(), LicUserVer());
      else if (gs_License.DaysLeft()!=CK_InfiniteDays)
        LogNote("License", 0, "License (%s) expires in %d days.   [ %s ]", LicDesc(), gs_License.DaysLeft(), LicUserVer());
      else
        LogNote("License", 0, "License (%s) OK.  [ %s ]", LicDesc(), LicUserVer());
      }
    }
  if (m_CLH.bForceDemo)
    gs_License.SetDemoMode();
  if (m_CLH.bForceRuntime)
    gs_License.SetAsRunTime();
  if (gs_License.DemoMode())
    {
    if (gs_License.DidInitCrypkey())
      LogNote("License", 0, "Using SysCAD in Demo Mode");
    else
      LogError("License", 0, "Licensing Service Initialise Failed, SysCAD set to Demo Mode!");
    }
  else if (gs_License.HashDefineCurtin())
    {
    LogNote("License", 0, "Using Curtin University Academic License.");
    }
  if (gs_License.IsRunTime())
    ((CMainFrame*)gs_SysCADApp.m_pMainWnd)->UpdateMainToolBar();
  //((CMainFrame*)SysCAD.m_pMainWnd)->UpdateMainToolBar();
  ScdMainWnd()->PostMessage(WMU_UPDATEMAINWND, SUB_UPDMAIN_BACKGROUND, 0);
#endif

  return TRUE;
  };

//---------------------------------------------------------------------------

BOOL CSysCADApp::InitInstVersion()
  {
  Strng CurrentVersionStr;
  CurrentVersionStr.Set("%i.%i", SCD_VERINFO_V0, SCD_VERINFO_V1);
  /*Strng PreviousVersionStr;
  PreviousVersionStr = ScdPFUser.RdStr("General", "SysCADVersion", "XXX");
  if (CurrentVersionStr.XStrICmp(PreviousVersionStr)!=0)
  {
  Strng sTmp(ScdPFUser.RdStr("General", "TextEditor", ""));
  if (sTmp()) // SysCAD Has Run Before on this box
  {
  sTmp.Set("Upgrade from previous Version of SysCAD %s to %s?", PreviousVersionStr(), CurrentVersionStr());
  if (AfxMessageBox(sTmp(), MB_YESNO|MB_DEFBUTTON2|MB_ICONQUESTION)==IDNO)
  {
  return false;
  }
  }
  }*/
  ScdPFMachine.WrStr("General", "SysCADVersion", CurrentVersionStr());

  return TRUE;
  };

//---------------------------------------------------------------------------

BOOL CSysCADApp::InitInstBaseCfg(int & CnvsLoadedCnt, Strng &CnvsFilename)
  {
  // Check Base Cfg
  if (1)
    {
    Strng B(BaseCfgFiles());
    B.FnClearEndBSlash();
    if (!FileExists(B()))
      {
      // ? Old Data Lib;
      Strng O;
      O.FnDrivePath(B());
      O+="_CfgData";
      if (FileExists(O()))
        {
        if (!MoveFile(O(), B()))
          {
          Strng E;
          E.GetSystemErrorStr(GetLastError());
          LogError("SysCAD", LF_Exclamation,
            "BaseCfg folder not renamed :\n%s\n%s",O(),E());
          }
        }
      else
        {
        O+="Data_Lib";
        if (FileExists(O()))
          {
          if (!MoveFile(O(), B()))
            {
            Strng E;
            E.GetSystemErrorStr(GetLastError());
            LogError("SysCAD", LF_Exclamation,
              "BaseCfg folder not renamed :\n%s\n%s",O(),E());
            }
          }
        }
      }
    //    if (!FileExists(B()))
    //      {
    //      LogError("SysCAD", LF_Exclamation, "BaseCfg Not found");
    //      return false;
    //      }
    }
  switch (TestBaseCfgFiles())
    {
    case 0: break;
    case -1:
      LogError("Project", LF_Exclamation, "Folder '%s' missing", BaseCfgFiles());
      return false;
    }

  // Must Use 8.2 to Upgrade CfgFiles
  SetSCDFileFormat(1);
  CnvsLoadedCnt = 0;
  CnvsFilename=BaseCfgFiles();
  CnvsFilename+=BCfgDBFileName();
  CnvsLoadedCnt = gs_CnvsMngr.LoadCnvsADO(CnvsFilename(), false);
  SetPrjFileFormat(ScdPFUser.RdInt("General", "PrjFileFormat", 1));

  //TODO: check for flwsym and fonts directories...
  //if (DirectoryExists(RootDir, "DATA_LIB\\FONTS")
  //if (DirectoryExists(RootDir, "DATA_LIB\\FLWSYM")

  return TRUE;
  };

//---------------------------------------------------------------------------

BOOL CSysCADApp::InitInstWinNT()
  {
#if WINNT_ONLY
  //below is not supported on Win95/Win98...
  DWORD dwMinimumWorkingSetSize;
  DWORD dwMaximumWorkingSetSize;
  //+DWORD PID=GetCurrentProcessId();
  HANDLE PHND=OpenProcess(PROCESS_ALL_ACCESS, true, GetCurrentProcessId());

  if (m_CLH.dwAffinity)
    SetProcessAffinityMask(PHND, m_CLH.dwAffinity);

  if (GetProcessWorkingSetSize(PHND, &dwMinimumWorkingSetSize, &dwMaximumWorkingSetSize))
    {
    dbgpln("dwMinimumWorkingSetSize;     %8i", dwMinimumWorkingSetSize);
    dbgpln("dwMaximumWorkingSetSize;     %8i", dwMaximumWorkingSetSize);

    SetProcessWorkingSetSize(PHND, dwMinimumWorkingSetSize, dwMaximumWorkingSetSize*2);
    if (GetProcessWorkingSetSize( PHND, &dwMinimumWorkingSetSize, &dwMaximumWorkingSetSize))
      {
      dbgpln("dwMinimumWorkingSetSize;     %8i", dwMinimumWorkingSetSize);
      dbgpln("dwMaximumWorkingSetSize;     %8i", dwMaximumWorkingSetSize);
      }
    }
  CloseHandle(PHND);
#endif

  return TRUE;
  };

//---------------------------------------------------------------------------

#ifdef _DEBUG
/* REPORT HOOK FUNCTION
--------------------
Again, report hook functions can serve a very wide variety of purposes.
This one logs error and assertion failure debug reports in the
log file, along with 'Damage' reports about overwritten memory.

By setting the retVal parameter to zero, we are instructing _CrtDbgReport
to return zero, which causes execution to continue. If we want the function
to start the debugger, we should have _CrtDbgReport return one.
*/
int MyReportHook(int nRptType, char *szMsg, int  *retVal)
  {
  char *RptTypes[] = { "Warning", "Error", "Assert" };

  //if ( ( nRptType > 0 ) || ( strstr( szMsg, "DAMAGE" ) ) )
  for (int i=0; szMsg[i]>0; i++)
    //    if (szMsg[i]=='\n' || szMsg[i]=='\r')
    if (szMsg[i]=='\r')
      {
      szMsg[i]=' ';
      break;
      }

    if (dbgtestfileopen())
      {
      if ( nRptType > 0 )
        dbgp("%s: %s", RptTypes[nRptType], szMsg );
      else
        dbgp("%s", szMsg );

      if (nRptType >= 2)
        *retVal=1;
      else
        *retVal=0;
      }
    else
      *retVal=1;

    return( true );         // Allow the report to be made as usual
  }


#endif
/*
BOOL CSysCADApp::xInitInstance()
{
// Initialize OLE libraries
if (!AfxOleInit())
{
AfxMessageBox(IDP_OLE_INIT_FAILED);
return FALSE;
}

if (!InitATL())
return FALSE;

AfxEnableControlContainer();

// Standard initialization
// If you are not using these features and wish to reduce the size
//  of your final executable, you should remove from the following
//  the specific initialization routines you do not need.

#ifdef _AFXDLL
Enable3dControls();     // Call this when using MFC in a shared DLL
#else
Enable3dControlsStatic(); // Call this when linking to MFC statically
#endif

// Change the registry key under which our settings are stored.
// TODO: You should modify this string to be something appropriate
// such as the name of your company or organization.
SetRegistryKey(_T("Local AppWizard-Generated Applications"));

LoadStdProfileSettings();  // Load standard INI file options (including MRU)

// Register the application's document templates.  Document templates
//  serve as the connection between documents, frame windows and views.

CMultiDocTemplate* pDocTemplate;
pDocTemplate = new CMultiDocTemplate(
IDR_SYSCADTYPE,
RUNTIME_CLASS(CSysCADDoc),
RUNTIME_CLASS(CChildFrame), // custom MDI child frame
RUNTIME_CLASS(CSysCADView));
AddDocTemplate(pDocTemplate);

// Connect the COleTemplateServer to the document template.
//  The COleTemplateServer creates new documents on behalf
//  of requesting OLE containers by using information
//  specified in the document template.
m_server.ConnectTemplate(clsid, pDocTemplate, FALSE);

// Register all OLE server factories as running.  This enables the
//  OLE libraries to create objects from other applications.
COleTemplateServer::RegisterAll();
_Module.RegisterClassObjects(CLSCTX_LOCAL_SERVER,
REGCLS_MULTIPLEUSE);

// Note: MDI applications register all server objects without regard
//  to the /Embedding or /Automation on the command line.

// create main MDI Frame window
CMainFrame* pMainFrame = new CMainFrame;
if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
return FALSE;
m_pMainWnd = pMainFrame;

// Parse command line for standard shell commands, DDE, file open
CCommandLineInfo cmdInfo;
ParseCommandLine(cmdInfo);

_Module.UpdateRegistryFromResource(IDR_SYSCAD, TRUE);
_Module.RegisterServer(TRUE);


// Check to see if launched as OLE server
if (cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated)
{
// Application was run with /Embedding or /Automation.  Don't show the
//  main window in this case.
return TRUE;
}

// When a server application is launched stand-alone, it is a good idea
//  to update the system registry in case it has been damaged.
m_server.UpdateRegistry(OAT_DISPATCH_OBJECT);
COleObjectFactory::UpdateRegistryAll();

// Dispatch commands specified on the command line
if (!ProcessShellCommand(cmdInfo))
return FALSE;

// The main window has been initialized, so show and update it.
pMainFrame->ShowWindow(m_nCmdShow);
pMainFrame->UpdateWindow();

return TRUE;
}
*/

//---------------------------------------------------------------------------

//void myPurecallHandler(void)
//  {
//  _asm int 3;
//  exit(0);
//  }


BOOL CSysCADApp::DoInitInstance()
  {

  //========================================
  // Testing : put this in to break into a new debugger when running embedded
#if BREAKINTODEBUGGER
  INCOMPLETECODE(__FILE__, __LINE__);
#endif
  //========================================

  if (!InitInstStartup())
    return false;

#if (0 && SYSCAD10)
  INITCOMMONCONTROLSEX InitCtrls;
  InitCtrls.dwSize = sizeof(InitCtrls);
  // Set this to include all the common control classes you want to use
  // in your application.
  InitCtrls.dwICC = ICC_WIN95_CLASSES;
  InitCommonControlsEx(&InitCtrls);
#endif

#if !(0 && SYSCAD10)
  if (!AfxOleInit())
    {
    AfxMessageBox("Unable to initialise OLE libraries.");
    return false;
    }
#endif

  if (!InitATL())
    return FALSE;

  AfxEnableControlContainer();
  //_Module.RegisterClassObjects(CLSCTX_LOCAL_SERVER, REGCLS_MULTIPLEUSE);

#ifndef DotNet
#ifdef _AFXDLL
  Enable3dControls();
#else
  Enable3dControlsStatic()
#endif
#endif

#if (USEREGISTRY)
    SetRegistryKey(_T("Kenwalt"));
#endif

  if (!InitInstRegistry())
    return false;

  if (!InitInstFolder())
    return false;

  if (!InitIniFile())
    return false;

  // unmask the some floating point exceptions
  int CW=FPP_EnableExceptions();
  //int CW=FPP_DisableExceptions();

  // REMOVED for .Net
  //  if (m_hPrevInstance!=NULL)
  //    return false;

#ifdef _DEBUG
  lDoFreeLibraries = true;
  BOOL UseMyReportHook = 0; //Use Debugger to set this to 0 if you want output to goto the 'debug' window
  if (UseMyReportHook)
    _CrtSetReportHook(MyReportHook);

  long BreakAlloc = -1; // Use Debugger to set this
  _CrtSetBreakAlloc(BreakAlloc); //this sets _crtBreakAlloc
  //afxMemDF=allocMemDF|checkAlwaysMemDF;

  BOOL UseMyAllocHook = 1; //Use Debugger to set this to 0 if you want output to goto the 'debug' window
  if (UseMyAllocHook)
    _CrtSetAllocHook(dbgAllocHook);

#endif


#if HEAP_SPARES
  Strng::StartUp();
#endif
  //  Dbg_Entry();
  gs_MsgLog.Entry();
  //gs_MsgLog.SetOptions( LogOption_COMCallsEntry
  //                      |LogOption_COMCallsExit 
  //                      |LogOption_COMCalls
  //                      //|LogOption_COMCallsInternal
  //                      ); 

  //ScdPFUser.SetProfFilename((char*)m_pszProfileName);
  ////ensure these are present in INI file...
  //int i = ScdPFUser.RdInt("General", "AccessDriver", 1); //0==Access92;1==Access2000;2==etc
  //ScdPFUser.WrInt("General", "AccessDriver", i);
  //i = ScdPFUser.RdInt("General", "DefaultAccessFormat", 0);
  //ScdPFUser.WrInt("General", "DefaultAccessFormat", i);

  gs_ServerWait = ScdPFUser.RdInt("General", "ServerStartUpWait", 10000);

  InitVersionStuff();

  CWaitCursor Wait;
  if (!MakeVersionOK("SysCAD91.EXE", _MAKENAME, SCD_VERINFO_V0, SCD_VERINFO_V1, SCD_VERINFO_V2, SCD_VERINFO_V3))
    return false;

  OSVERSIONINFO VI;
  VI.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  if (GetVersionEx(&VI))
    {
    if (VI.dwPlatformId==VER_PLATFORM_WIN32s)
      return false;
    IsWinNT = (VI.dwPlatformId==VER_PLATFORM_WIN32_NT);
    }

  LoadStdProfileSettings(8);  // Load standard INI file options (including MRU)
  // Allow longer filenames in MRU
  m_pRecentFileList->m_nMaxDisplayLength=80;

  if (!InitInstDocTemplate())
    return false;

  if (!InitInstCmdLineOnly())
    return false;

  // Parse command line for standard shell commands, DDE, file open
  CMyCommandLineInfo cmdInfo;
  ParseCommandLine(cmdInfo);

  if (cmdInfo.m_bRegServer)
    {
    _Module.UpdateRegistryFromResource(IDR_SYSCAD, TRUE);
    _Module.RegisterServer(TRUE);

    if (1)// || gs_License.AllowOPCServer())
      COPCSrvrWrapper::RegisterOPCServer();
    return TRUE;
    }

  StartAppComThread(WMU_APPEVT);

  // create main MDI Frame window
  CMainFrame* pMainFrame = new CMainFrame;
  if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
    return false;

  SetScdMainWnd(pMainFrame);

  // Check to see if launched as OLE server
  if (cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated)
    {
    // Application was run with /Embedding or /Automation.  Don't show the
    //  main window in this case.
    //return TRUE;
    gs_MsgLog.SetMsgBoxEmbedded();
    }
  m_DoOPCServerStartup = cmdInfo.m_bRunEmbedded;

  // When a server application is launched stand-alone, it is a good idea
  //  to update the system registry in case it has been damaged.
#if WITHVISIODOC
  m_VisioServer.UpdateRegistry(OAT_DISPATCH_OBJECT);
#endif
#if WITHAUTOMATION
  m_AutoServer.UpdateRegistry(OAT_DISPATCH_OBJECT);
#endif

  COleObjectFactory::UpdateRegistryAll();

  //<VISIO>  // Dispatch commands specified on the command line
  //<VISIO>  if (!ProcessShellCommand(cmdInfo))
  //<VISIO>    return FALSE;

  m_pMainWnd = pMainFrame;
  CProject::RestoreOneWindow(CWindowLists::MainWndTitle, pMainFrame, true, false);
  m_pMainWnd->ShowWindow(m_nCmdShow);
  m_pMainWnd->RedrawWindow();
  CStatusBarMsg BarMsg("Starting SysCAD");

  CScdCOCmdBase::s_hWnd4Msgs=m_pMainWnd->m_hWnd;

  Strng LFn(TemporaryFiles());
  LFn.FnCheckEndBSlash();
  LFn+="Scd_Log.csv";
  gs_MsgLog.LogFileOpen(LFn());

  //parse command line...
  m_DoAutoLoadCLHPrj=false;
  m_DoAutoLoadCLHSnp=false;
  m_DoAutoRunCLHPrj=false;
  m_DoAutoRunCLHMinimise=false;
  m_DoLoadCLHPrj=false;
  m_DoExecCLHScript=false;
  m_DoAutoRunCLHWait=0;

  //NBNBNB
  //m_lpCmdLine=" /d";


  //if (m_lpCmdLine && m_lpCmdLine[0] != '\0')
  //  AfxMessageBox(m_lpCmdLine);
  if (!m_CLH.Parse(m_lpCmdLine))
    return false;

  if (0)
    {
    m_CLH.bDebugOn = true;
    m_CLH.sDebugFile.Set("%sScd_Dbg.%i.Txt", TemporaryFiles(), SCD_BUILD_NUM);
    }

  if (m_CLH.bDebugOn)
    {


    flag IsOpen = dbgfileopen(m_CLH.sDebugFile());
    //    dbgtime(3);
    ASSERT(IsOpen);
    dbgpln("SysCAD Init ===================================================");
    dbgpln("CmdLine:%s", m_CLH.sCmdLine());
    dbgpln("ThreadId %4x", GetCurrentThreadId());
    dbgpln("MAC Address %s", GetMacAddressStr(CString("*")));
    dbgpln("Disk Serial Number 0x%x", GetDiskSerialNumber(ProgFiles()));

    //DNanValueUnion DSignalNanValue1={0x7ffbffffffffffff};
    //DNanValueUnion DSignalNanValue2={0x7fffffffffffffff};
    //DNanValueUnion DSignalNanValue3={0x7ffbfff000000000};
    //DNanValueUnion DSignalNanValue4={0x7ffffff000000000};
    //DNanValueUnion DSignalNanValue5={0x7ffbf0ffffffffff};
    //DNanValueUnion DSignalNanValue6={0x7ffbff00ffffffff};

    //dbgpln("1, %s %g", Valid(DSignalNanValue1.D)?"   ":"NAN", DSignalNanValue1.D);
    //dbgpln("2, %s %g", Valid(DSignalNanValue2.D)?"   ":"NAN", DSignalNanValue2.D);
    //dbgpln("3, %s %g", Valid(DSignalNanValue3.D)?"   ":"NAN", DSignalNanValue3.D);
    //dbgpln("4, %s %g", Valid(DSignalNanValue4.D)?"   ":"NAN", DSignalNanValue4.D);
    //dbgpln("5, %s %g", Valid(DSignalNanValue5.D)?"   ":"NAN", DSignalNanValue5.D);
    //dbgpln("6, %s %g", Valid(DSignalNanValue6.D)?"   ":"NAN", DSignalNanValue6.D);
    //for (int i=0; i<40; i++)
    //  {
    //  double X=i*500.0;
    //  dbgpln("%i %10.4f   P:%10.4f %10.4f %10.4f   T:%10.4f %10.4f %10.4f   D:%10.4f %10.4f %10.4f", i, X,
    //    AtmosPress(X), XAtmosPress(X), AtmosPress(X)/XAtmosPress(X),
    //    AmbientTemp(X), XAtmosTemp(X), AmbientTemp(X)/XAtmosTemp(X),
    //    XAtmosDens(X), XAtmosDens(X), AtmosDensity(X)/XAtmosDens(X)
    //    );
    //  }

    dbgfileflush(0);
    }


  if (m_CLH.bDebugOn)
    m_WithLocalCopy=1;

  ScdCtrls_Entry();
  GPWFuncs_Entry();

  //  CProject::RestoreOneWindow(CWindowLists::MainWndTitle, m_pMainWnd, true, false);

  CMsgWindow::StartUp();
  CDlgBusy::Startup();
  CDlgBusy::Open("\n\nStarting");

  BuildInterfaceWindows();

  int LicenseRet=InitInstLicense1(cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated);
  //if (LicenseRet==LicInit_ExitReqd)
  //  return false;

  if (!InitInstVersion())
    return false;

  int CnvsLoadedCnt;
  Strng CnvsFilename;
  if (!InitInstBaseCfg(CnvsLoadedCnt, CnvsFilename))
    return false;

  Grf3Drw_Entry();
  GDIBlkCfg::RestoreAll();

  //int Pri=GetThreadPriority(GetCurrentThread());
  //::SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);

  if (gs_License.AllowOPCServer())
    OpenMainOPCSrvr(ExeFile(), 0, 0);

  gs_Exec.Initialise();// = new CExecutive(GetCurrentThreadId());
  gs_pXOle = new COleExec();
  gs_pXCmd = new CCmdExec();
  gs_pXRep = new CRepExec();
  //pTagDB = new TagDBase;
  gs_pIOMarshal = new CIOMarshal;
#if WITHDRVMAN
  gs_pDrvMan = new CDriverManager;
#endif
  gs_pArcMan = new CArchiveManager;
#if WITHNETSERVER
  pCS_Mngr = new CCS_Manager;
#endif
  gs_HstMngr.CreateTheHistEO();
  gs_pPrj  = new CProject;

  MainWnd()->EO_Register(pExecName_MainWnd, EOWrite_Msg, 0, 0);


  CPGMDbgMngr::Open();

  //CRptTagLists::StartUp();
  //CMdlValueShow::StartUp();
  //CMdlValueSet::StartUp();

  gs_pBaseCmds = new BaseCmdBlk();
  gs_pCmd->OpenCommandBlk(gs_pBaseCmds, true);
  gs_pCmd->SetFocus();

  dbgpln("Windows System Info : ");
  SYSTEM_INFO si;
  GetSystemInfo(&si);
  dbgpln("dwOemId                      %08x",si.dwOemId);
  dbgpln("dwPageSize                   %08x",si.dwPageSize);
  dbgpln("lpMinimumApplicationAddress  %08x",si.lpMinimumApplicationAddress);
  dbgpln("lpMaximumApplicationAddress  %08x",si.lpMaximumApplicationAddress);
  dbgpln("dwActiveProcessorMask        %08x",si.dwActiveProcessorMask);
  dbgpln("dwNumberOfProcessors         %8li",si.dwNumberOfProcessors);
  dbgpln("dwProcessorType              %08x",si.dwProcessorType);
  dbgpln("dwProcessorLevel             %08x",si.wProcessorLevel);
  dbgfileflush(1);

  if (!InitInstWinNT())
    return false;

  CExploreScd::SetUseScdExplorer(ScdPFUser.RdInt("General", "UseScdExplorer", 1));
  SetTxtEditor((LPSTR)ScdPFUser.RdStr("General", "TextEditor", "notepad.exe"));
  SetSymbolicPaths(ScdPFUser.RdInt("General", "SymbolicPaths", 0));

  FxdEdtView::m_WheelChangeVal = (ScdPFUser.RdInt("Mouse", "WheelChangeValuesOn", FxdEdtView::m_WheelChangeVal)!=0);
  GrfCmdBlk::m_Left      = (MouseBtnActs)ScdPFUser.RdInt("Mouse", "Left",      GrfCmdBlk::m_Left);
  GrfCmdBlk::m_LeftDbl   = (MouseBtnActs)ScdPFUser.RdInt("Mouse", "LeftDbl",   GrfCmdBlk::m_LeftDbl);
  GrfCmdBlk::m_LeftCtrl  = (MouseBtnActs)ScdPFUser.RdInt("Mouse", "LeftCtrl",  GrfCmdBlk::m_LeftCtrl);
  GrfCmdBlk::m_LeftShft  = (MouseBtnActs)ScdPFUser.RdInt("Mouse", "LeftShft",  GrfCmdBlk::m_LeftShft);
  GrfCmdBlk::m_LeftCtSh  = (MouseBtnActs)ScdPFUser.RdInt("Mouse", "LeftCtSh",  GrfCmdBlk::m_LeftCtSh);
  GrfCmdBlk::m_Right     = (MouseBtnActs)ScdPFUser.RdInt("Mouse", "Right",     GrfCmdBlk::m_Right);
  GrfCmdBlk::m_RightDbl  = (MouseBtnActs)ScdPFUser.RdInt("Mouse", "RightDbl",  GrfCmdBlk::m_RightDbl);
  GrfCmdBlk::m_RightCtrl = (MouseBtnActs)ScdPFUser.RdInt("Mouse", "RightCtrl", GrfCmdBlk::m_RightCtrl);
  GrfCmdBlk::m_RightShft = (MouseBtnActs)ScdPFUser.RdInt("Mouse", "RightShft", GrfCmdBlk::m_RightShft);
  GrfCmdBlk::m_RightCtSh = (MouseBtnActs)ScdPFUser.RdInt("Mouse", "RightCtSh", GrfCmdBlk::m_RightCtSh);

  HelpMngr.Init();

  LogNote("Version", 0, "%s (%s)", FullVersion(), BuildDate());

  if (!InitInstLicense2(LicenseRet))
    return false;

  if (m_CLH.sAutoLoadPrj.Length()>0)
    m_DoAutoLoadCLHPrj=true;
  else if (m_CLH.sAutoRunPrj.Length()>0)
    m_DoAutoRunCLHPrj=true;
  else if (m_CLH.bAllowLoadPrevPrj && (m_CLH.bLoadPrevPrj || ScdPFUser.RdInt("General", "PromptPrevPrj", 0)!=0))
    m_DoLoadCLHPrj=true;

  if (m_CLH.sAutoLoadSnp.Length()>0)
    m_DoAutoLoadCLHSnp=true;

#if WITHAUTOMATION
  if (m_CLH.bAutomation)
    {
    AutomationTemplate().CreateNewDocument();
    //CDocument* pDoc = SysCAD.OpenDocumentFile("xxx.sam");
    //if (pDoc)
    //  return true;
    }
#endif

  gs_pPrj->Initialise(NULL);

  if (m_CLH.ScriptFiles.GetSize()>0)
    m_DoExecCLHScript=true;
  CDlgBusy::Close();

  if (CnvsLoadedCnt>0)
    LogNote("SysCAD", 0, "%d conversions (in %d families) loaded from %s", CnvsLoadedCnt, (gs_CnvsMngr.CnvCnt()), CnvsFilename());

  gs_MsgLog.SetAppStarted();

  //_set_purecall_handler(myPurecallHandler);

  m_DoAutoRunCLHWait=m_CLH.nAutoRunWait;
  if (m_CLH.bMinimiseWnd)
    {
    if (m_DoAutoLoadCLHPrj || m_DoAutoRunCLHPrj || m_DoLoadCLHPrj)
      m_DoAutoRunCLHMinimise =true;
    else
      MainWnd()->ShowWindow(SW_MINIMIZE);
    }

  return true;
  }


/*
int CSysCADApp::ExitInstance()
{
if (m_bATLInited)
{
_Module.RevokeClassObjects();
_Module.Term();
}

return CWinApp::ExitInstance();
}
*/
//---------------------------------------------------------------------------

int CSysCADApp::DoExitInstance()
  {
  gs_AccessWnds.TermHelp();

#if WITHCOMCMD
  gs_ScdCmdIF.UnRegister();
#endif

  CExploreScd::CloseIt();
  CPGMDbgMngr::Close();
  //::OleUninitialize();

  if (gs_pCmd)
    gs_pCmd->CloseCommandBlk(gs_pBaseCmds);
  if (gs_pBaseCmds)
    {
    delete gs_pBaseCmds;
    gs_pBaseCmds=NULL;
    }
  if (gs_pPrj)
    {
    delete gs_pPrj;
    gs_pPrj = NULL;
    //delete pTagDB;
    gs_HstMngr.DestroyTheHistEO();
#if WITHNETSERVER
    delete pCS_Mngr;
#endif
    delete gs_pArcMan;
#if WITHDRVMAN
    delete gs_pDrvMan;
#endif
    delete gs_pIOMarshal;
    delete gs_pXRep;
    delete gs_pXCmd;
    delete gs_pXOle;
    gs_Exec.Terminate(); //delete pExec;

    gs_MsgLog.DeRegisterMsgWnd();
    //SFEClientDLL::ShutDown();
    TagObjClass::ClearAll();
    GDIBlkCfg::SaveAll();
    //Cnvs_Exit();

    dbgpln("SysCAD Exit ===================================================");
    //    Dbg_Exit();
    gs_MsgLog.Exit();

    // Allow dump of memory leaks
    // dbgfileclose();
    }

  gs_License.Exit();

  CloseMainOPCSrvr();

#if (HEAP_SPARES && HEAP_STATS)
  if (0)
    SparesList::DumpUsage("SysCAD Done",true);
#endif
  //#if (TESTMEMGOBBLE)
  //  DumpOurHeap();
  //#endif

  CDbgMngr::ShutDown();
#if HEAP_SPARES
  Strng::ShutDown();
#endif
  CDlgBusy::Shutdown();
  DataDefnBlk::ShutDown();

  //if (m_bATLInited)
  //  {
  //  _Module.RevokeClassObjects();
  //  _Module.Term();
  //  }

  if (m_bATLInited)
    {
    StopAppComThread();
    _Module.Term();
    }

  return CWinApp::ExitInstance();
  }

//---------------------------------------------------------------------------

BOOL CSysCADApp::InitInstance()
  {
  BOOL Ret=FALSE;
  _set_se_translator(XcpTranslateFunction);                        
  DWORD FPP_State=_controlfp(0,0);                                 
  try                                                              
    {                                                              
    /* clear any outstanding exceptions */                         
    _clearfp();                                                    
    /* Set New fpControl */                                        
    _controlfp(_EM_DENORMAL|_EM_UNDERFLOW|_EM_INEXACT, _MCW_EM);

    Ret=DoInitInstance();

    FPP_RestoreExceptions(FPP_State);                             
    }       
  catch (MSysException e)                                         
    {                                                             
    _clearfp();                                                   
                                                         
    //e->ReportError();
    Strng S;                                                      
    S.Set(" %s \n\n"                                              
          "Exception %s (%x) occurred\n\n"                        
         "Address: %08x\n\n"                                     
          "Thread terminating\n\n"                                
          "Push OK to exit SysCAD",                               
      "SysCAD Application Initialise", e.Name, e.Code, e.Address);                          
                                                                  
    AfxMessageBox(S(), MB_OK);                                       
    }

  catch (CMemoryException * e)         { e->ReportError(); }//gs_License.Exit(); }
  catch (CNotSupportedException * e)   { e->ReportError(); }//gs_License.Exit(); }
  catch (CArchiveException * e)        { e->ReportError(); }//gs_License.Exit(); }
  catch (CFileException * e)           { e->ReportError(); }//gs_License.Exit(); }
  catch (CResourceException * e)       { e->ReportError(); }//gs_License.Exit(); }
  catch (COleException * e)            { e->ReportError(); }//gs_License.Exit(); }
  catch (COleDispatchException * e)    { e->ReportError(); }//gs_License.Exit(); }
  catch (CUserException * e)           { e->ReportError(); }//gs_License.Exit(); }
  catch (CException * e)               { e->ReportError(); }//gs_License.Exit(); }
  
  catch (CScdException * e)            
    { 
    AfxMessageBox(e->ErrStr(), MB_OK);                                       
    }
  catch (...)
    {
    AfxMessageBox("Unknown Exception Occurred - During Initialise", MB_OK);                                       
    gs_License.Exit();
    }
  return Ret;
  };
int CSysCADApp::ExitInstance()
  {
  int Ret=0;
  _set_se_translator(XcpTranslateFunction);                        
  DWORD FPP_State=_controlfp(0,0);                                 
  try                                                              
    {                                                              
    /* clear any outstanding exceptions */                         
    _clearfp();                                                    
    /* Set New fpControl */                                        
    _controlfp(_EM_DENORMAL|_EM_UNDERFLOW|_EM_INEXACT, _MCW_EM);

    Ret=DoExitInstance();
  
    FPP_RestoreExceptions(FPP_State);                             
    }       
  catch (MSysException e)                                         
    {                                                             
    _clearfp();                                                   
                                                         
    //e->ReportError();
    Strng S;                                                      
    S.Set(" %s \n\n"                                              
          "Exception %s (%x) occurred\n\n"                        
         "Address: %08x\n\n"                                     
          "Thread terminating\n\n"                                
          "Push OK to exit SysCAD",                               
      "SysCAD Application Exit", e.Name, e.Code, e.Address);                          
                                                                  
    AfxMessageBox(S(), MB_OK);                                       
    }

  catch (CMemoryException * e)         { e->ReportError(); }//gs_License.Exit(); }
  catch (CNotSupportedException * e)   { e->ReportError(); }//gs_License.Exit(); }
  catch (CArchiveException * e)        { e->ReportError(); }//gs_License.Exit(); }
  catch (CFileException * e)           { e->ReportError(); }//gs_License.Exit(); }
  catch (CResourceException * e)       { e->ReportError(); }//gs_License.Exit(); }
  catch (COleException * e)            { e->ReportError(); }//gs_License.Exit(); }
  catch (COleDispatchException * e)    { e->ReportError(); }//gs_License.Exit(); }
  catch (CUserException * e)           { e->ReportError(); }//gs_License.Exit(); }
  catch (CException * e)               { e->ReportError(); }//gs_License.Exit(); }
  
  catch (CScdException * e)            
    { 
    AfxMessageBox(e->ErrStr(), MB_OK);                                       
    }
  catch (...)
    {
    AfxMessageBox("Unknown Exception Occurred - During Exit", MB_OK);                                       
    gs_License.Exit();
    }
  return Ret;
  };

//---------------------------------------------------------------------------

static CDocument* DocN(CWnd *pp, int &n)
  {
  CWnd *p=pp->GetWindow(GW_CHILD);
  CWnd *p1=p;
  while (p)
    {
    if (p->IsKindOf(RUNTIME_CLASS(CView)))
      {
      CDocument* pd=((CView*)p)->GetDocument();
      if (pd && pd->IsKindOf(RUNTIME_CLASS(DocRoot)))
        {
        //n--;
        if (n==0)
          return pd;
        n--;
        }
      else if (pd && pd->IsKindOf(RUNTIME_CLASS(OleDocRoot)))
        {
        //n--;
        if (n==0)
          return pd;
        n--;
        }
      return NULL;
      }
    CDocument* pd=DocN(p, n);
    if (pd)
      return pd;
    p=p->GetWindow(GW_HWNDNEXT);
    if (p==p1)
      break;
    }
  return NULL;
  }

CDocument* CSysCADApp::DocumentNo(int n)
  {
  return DocN(m_pMainWnd, n);
  }


//---------------------------------------------------------------------------

BOOL CSysCADApp::SaveAllModified() // save before exit
  {
  if (gs_pPrj)
    gs_pPrj->bDocChanged = true; //force project save
  return CWinApp::SaveAllModified();
  }

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

int CSysCADApp::ShutDown()
  {
  if (gs_pPrj->ShutDown())
    {
    CProject::SaveOneWindow(0, CWindowLists::MainWndTitle, m_pMainWnd, true);
    CTrndInfo::ShutDown();
    //CRptTagLists::ShutDown();
    CMsgWindow::ShutDown();
    //CMdlValueShow::ShutDown();
    //CMdlValueSet::ShutDown();
    MainWnd()->EO_DeRegister();
    HelpMngr.Term();
    return true;
    }
  return false;
  }

//---------------------------------------------------------------------------

BOOL CSysCADApp::CanClose()
  {
  // Must interogate all ExecObjects
  Strng_List Problems;
  switch (gs_Exec.CanClose(Problems))
    {
    case EO_CanClose_Yes:
      return true;
    case EO_CanClose_Wait:
      return false;
    case EO_CanClose_Save:
      return false;
    case EO_CanClose_No:
      return false;
    }
  return true;
  }

//---------------------------------------------------------------------------

void CSysCADApp::StdProfileChanged()
  {
  CWinApp::SaveStdProfileSettings();
  }

//---------------------------------------------------------------------------

void CSysCADApp::BuildInterfaceWindows()
  {
  CRect MainRect,Rect;
  MainWnd()->GetClientRect(&MainRect);
  int w = (MainRect.right - MainRect.left - 30) / 3;
  HICON hIcon;
  CString ClassName;
  CCreateContext CC;

  CMsgWindow::Register();
  if (ScdPFUser.RdInt("General", "ShowEvalOrderWindow", 0)!=0)
    COrdWindow::Register();
  if (ScdPFUser.RdInt("General", "ShowEvalStatsWindow", 0)!=0)
    CStatsWindow::Register();
  //if (ScdPFUser.RdInt("General", "ShowEvalOrderWindow", 0)!=0)
  //  CWirePanelWindow::Register();

  //access window...
  Rect.left = 2 * w;
  Rect.right = (3 * w) + 30;
  Rect.top = 0;
  Rect.bottom = MainRect.bottom - 50;
  gs_AccWndDoc.m_bAutoDelete = false;
  CC.m_pCurrentDoc = &gs_AccWndDoc;
  CC.m_pNewDocTemplate = NULL;
  CC.m_pLastView = NULL;
  CC.m_pCurrentFrame = NULL;
  CC.m_pNewViewClass = RUNTIME_CLASS(AccNodeWnd);


  if (1)
    {
    for (int i=0; i<NAccessWnds; i++)
      {
      CMDIChildWnd* pAccFrame=NULL;
      AccNodeWnd*   pAccWnd=NULL;

      pAccFrame = new CFrm;
      hIcon = ScdApp()->LoadIcon(IDI_ACCESS);
      ClassName = AfxRegisterWndClass(CS_DBLCLKS|CS_NOCLOSE, 0, 0, hIcon);
      if (pAccFrame->Create(ClassName.GetBuffer(0),
        "Access", WS_CHILD | WS_OVERLAPPED |
        WS_CAPTION | WS_SYSMENU | WS_THICKFRAME |
        WS_MINIMIZEBOX | WS_MAXIMIZEBOX |
        WS_MINIMIZE,
        Rect, NULL, &CC))
        {
        pAccWnd = (pAccNodeWnd)pAccFrame->GetTopWindow();
        pAccFrame->ShowWindow(SW_MINIMIZE);
        }
      gs_AccessWnds.Add(pAccWnd, pAccFrame);
      }
    }

  //command window...
  Rect.left = 0;
  Rect.right = w * 2;
  Rect.top = MainRect.bottom - 360;
  Rect.bottom = MainRect.bottom - 160;
  CC.m_pCurrentDoc = NULL;
  CC.m_pNewDocTemplate = NULL;
  CC.m_pLastView = NULL;
  CC.m_pCurrentFrame = NULL;
  CC.m_pNewViewClass = RUNTIME_CLASS(CommandWnd);
  gs_pCmdFrame = new CFrm;
  hIcon = ScdApp()->LoadIcon(IDI_COMMAND);
  ClassName = AfxRegisterWndClass(CS_DBLCLKS|CS_NOCLOSE, 0, 0, hIcon);
  if (gs_pCmdFrame->Create(ClassName.GetBuffer(0),
    "Command", WS_CHILD | WS_OVERLAPPED |
    WS_CAPTION | WS_SYSMENU | WS_THICKFRAME |
    WS_MINIMIZEBOX | WS_MAXIMIZEBOX /*| WS_MINIMIZE*/,
    Rect, NULL, &CC))
    {
    gs_pCmd = (CommandWnd*)gs_pCmdFrame->GetTopWindow();
    gs_pCmdFrame->ShowWindow(SW_MINIMIZE);
    }
  }

//---------------------------------------------------------------------------

BOOL CSysCADApp::OnIdle(LONG lCount)
  {
  if (m_CLH.lGobbleMemUntil>=0)
    {
    for (int i=0; ; i++)
      {
      MEMORYSTATUS MS;
      MS.dwLength=sizeof(MS);
      GlobalMemoryStatus(&MS);
      dbgpln(">>> %5i %8i %8i %8i %8i %8i %8i",
        i,
        MS.dwAvailVirtual/(1024*1024),
        MS.dwTotalPhys/(1024*1024),
        MS.dwAvailPhys/(1024*1024),
        MS.dwTotalPageFile/(1024*1024),
        MS.dwAvailPageFile/(1024*1024),
        (MS.dwTotalVirtual-MS.dwAvailVirtual)/(1024*1024)
        );
      if (MS.dwAvailPageFile<=(DWORD)m_CLH.lGobbleMemUntil*1024*1024)
        break;
      if (1)
        calloc(1000*1024, sizeof(char));
      else
        new char[1000*1024];
      }
    m_CLH.lGobbleMemUntil=-1;
    }

  //#if (TESTMEMGOBBLE)
  //for (int ig=0; ig<GobbleAmount; ig++)
  //  {
  //  //GobbleBuffer[ig] = new char[1000*1024]; //calloc(1000*1024, sizeof(char));
  //  GobbleBuffer[ig] = calloc(1000*1024, sizeof(char));
  //  dbgpln(" %08x", GobbleBuffer[ig]);
  //  memset(GobbleBuffer[ig], 0xa2, 1000*1024);
  //  }
  //#endif


  if (lCount==0 && (abs(int(GetTickCount()-m_TimeOfLastIdle)) < 60000))
    gs_MsgLog.FlushWndQ();

  m_TimeOfLastIdle=GetTickCount();

  if (!gs_Exec.Busy())
    gs_MsgLog.ClrMsgBoxSkip();

  EnterCriticalSection(&GDIBlk::cbDrawSect); // must lock MFC Graphics stuff
  BOOL bResult = CWinApp::OnIdle((false) ? Max(1L, lCount) : lCount);
  LeaveCriticalSection(&GDIBlk::cbDrawSect);

  switch (lCount)
    {
    case 0:
      bResult = true;
      break;

    case 1:
      CmdExec().SetIdle();
      gs_Exec.SetAppIdleDone();
      bResult = true;
      break;

    case 2:
      CExploreScd::ChkRefreshIt();
      bResult = true;
      break;

    case 3:
      {
      if (m_DoAutoLoadCLHPrj||m_DoAutoRunCLHPrj||m_DoLoadCLHPrj)
        {
        bResult=false; // Let This Complete

        if (m_DoAutoRunCLHWait)
          Sleep(Range(0l,m_DoAutoRunCLHWait,300l)*1000);

        Strng PrjFn("");

        if (m_DoAutoLoadCLHPrj)
          PrjFn=m_CLH.sAutoLoadPrj();
        else if (m_DoAutoRunCLHPrj)
          PrjFn=m_CLH.sAutoRunPrj();
        else if (m_DoLoadCLHPrj)
          PrjFn = ScdPFUser.RdStr("General", "RecentProject", "");

        if (PrjFn.Len()>0)
          {
          CWaitCursor Wait;
          flag OpenPrj = false;
          if (m_DoAutoLoadCLHPrj || m_DoAutoRunCLHPrj)
            OpenPrj = true;
          else
            {
            Strng T;
            T.Set("Open Project '%s' ?", PrjFn());
            if (AfxMessageBox(T(), MB_YESNO|MB_DEFBUTTON2|MB_ICONQUESTION)==IDYES)
              OpenPrj = true;
            }
          Wait.Restore();
          if (OpenPrj)
            {
            PrjFn.FnCheckExtension("spj");
            CDocument* pDoc = OpenDocumentFile(PrjFn());
            if (!pDoc)
              LogError("Project", LF_Exclamation, "Unable to open '%s'", PrjFn());
            if (m_DoAutoRunCLHPrj)
              MainWnd()->PostMessage(WM_COMMAND, ID_ACTIONS_RUN, 0);
            }

          if (m_DoAutoLoadCLHSnp)
            {
            Strng SnpFn(m_CLH.sAutoLoadSnp());
            SnpFn.FnDrivePath();
            if (SnpFn.Length()==0)
              {
              SnpFn.FnDrivePath(PrjFn());
              SnpFn+=m_CLH.sAutoLoadSnp();
              }
            gs_pPrj->DoLoadSnapShotScenario(true, SnpFn());
            }
          }

        if (m_DoAutoRunCLHMinimise)
          MainWnd()->ShowWindow(SW_MINIMIZE);

        m_DoAutoLoadCLHPrj=false;
        m_DoAutoLoadCLHSnp=false;
        m_DoAutoRunCLHPrj=false;
        m_DoLoadCLHPrj=false;
        m_DoAutoRunCLHMinimise=false;
        m_DoAutoRunCLHWait=0;

        }
      else if (m_DoExecCLHScript)
        {
        bResult=false;
        m_DoExecCLHScript=false;
        if (m_CLH.ScriptFiles.GetSize()>0)
          {
          for (int i=0; i<m_CLH.ScriptFiles.GetSize(); i++)
            gs_pXCmd->AddCmds((char*)(const char*)(m_CLH.ScriptFiles[i]));
          MainWnd()->PostMessage(WMU_CMDNEXT, ComCmd_ExecNextCmd, 0);
          }
        }
      else if (m_DoOPCServerStartup)
        {
        ScdMainWnd()->PostMessage(WMU_UPDATEMAINWND, SUB_UPDMAIN_STARTED, (LPARAM)0);
        m_DoOPCServerStartup = false;
        }
      else
        {
        bResult=false;
        CProject::sm_SysCADInited=true;
        }
      }
      break;
    }

  return bResult;
  }

//---------------------------------------------------------------------------

BOOL CSysCADApp::PreTranslateMessage(MSG* pMsg)
  {
  //return CWinApp::PreTranslateMessage(pMsg);
  BOOL bResult = CWinApp::PreTranslateMessage(pMsg);

  //#if USESCDEXPLORER
  //  CExploreScd::ChkRefreshIt();
  //#else
  //  CWndSlctWnd::ChkRefreshIt(); //is their a more effecient way to do this???
  //#endif

  switch (pMsg->message)
    {
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_NCLBUTTONDOWN:
    case WM_NCRBUTTONDOWN:
    case WM_NCMBUTTONDOWN:
    case WM_MOUSEMOVE:
    case WM_NCMOUSEMOVE:
    case WM_NCHITTEST:
    case WM_ACTIVATEAPP:
      {
#if WITHSCRCYCLES
      if (gs_pPrj && gs_pPrj->iCycleNo>=0)
        {
        //m_pMainWnd->PostMessage(WMU_ENDCYCLES);
        m_pMainWnd->SendMessage(WMU_ENDCYCLES);
        }
#endif
      //if (pExec)
      if (gs_Exec.Initialised())
        gs_Exec.LowerPriority();
      }
    }

  return bResult;
  }

//---------------------------------------------------------------------------

LRESULT CSysCADApp::DoFlushMsgQ(WPARAM wParam, LPARAM lParam)
  {
  MSG msg;

  while (::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE) && (msg.message!=WM_QUIT))
    PumpMessage();

  return 0;
  }

//---------------------------------------------------------------------------

//#pragma warning (disable : 4101) // unreferenced variable
int CSysCADApp::Run( )
  {
  int Ret=0;
  _set_se_translator(XcpTranslateFunction);                        
  DWORD FPP_State=_controlfp(0,0);                                 
  try                                                              
    {                                                              
    /* clear any outstanding exceptions */                         
    _clearfp();                                                    
    /* Set New fpControl */                                        
    _controlfp(_EM_DENORMAL|_EM_UNDERFLOW|_EM_INEXACT, _MCW_EM);

    Ret=CWinApp::Run();

    FPP_RestoreExceptions(FPP_State);                             
    }       
  catch (MSysException e)                                         
    {                                                             
    _clearfp();                                                   
                                                         
    //e->ReportError();
    Strng S;                                                      
    S.Set(" %s \n\n"                                              
          "Exception %s (%x) occurred\n\n"                        
         "Address: %08x\n\n"                                     
          "Thread terminating\n\n"                                
          "Push OK to exit SysCAD",                               
          "SysCAD Application", e.Name, e.Code, e.Address);                          
                                                                  
    AfxMessageBox(S(), MB_OK);                                       
    }

  catch (CMemoryException * e)         { e->ReportError(); }//gs_License.Exit(); }
  catch (CNotSupportedException * e)   { e->ReportError(); }//gs_License.Exit(); }
  catch (CArchiveException * e)        { e->ReportError(); }//gs_License.Exit(); }
  catch (CFileException * e)           { e->ReportError(); }//gs_License.Exit(); }
  catch (CResourceException * e)       { e->ReportError(); }//gs_License.Exit(); }
  catch (COleException * e)            { e->ReportError(); }//gs_License.Exit(); }
  //catch (CDBException * e)             { e->ReportError(); }//gs_License.Exit(); }
  catch (COleDispatchException * e)    { e->ReportError(); }//gs_License.Exit(); }
  catch (CUserException * e)           { e->ReportError(); }//gs_License.Exit(); }
  //catch (CDaoException * e)            { e->ReportError(); }//gs_License.Exit(); }
  //catch (CInternetException * e)       { e->ReportError(); }//gs_License.Exit(); }
  catch (CException * e)               { e->ReportError(); }//gs_License.Exit(); }
  
  catch (CScdException * e)            
    { 
    AfxMessageBox(e->ErrStr(), MB_OK);                                       
    }
  //catch (_com_error &e)
  //  {
  //  e.ReportError();
  //  int xxx=0;
  //  }
  catch (...)
    {
    //EXCEPTION_POINTERS * p = GetExceptionInformation();

    //Strng S;                                                      
    //S.Set(" %s \n\n"                                              
    //      "Exception (%x) occurred\n\n"                        
    //     "Address: %08x\n\n"                                     
    //      "Thread terminating\n\n"                                
    //      "Push OK to exit SysCAD",                               
    //      "SysCAD Application", p->ExceptionRecord->ExceptionCode, p->ExceptionRecord->ExceptionAddress);                          
                                                                  
    AfxMessageBox("Unknown Exception Occurred", MB_OK);                                       

    gs_License.Exit();
    }

  return Ret;
  }

//---------------------------------------------------------------------------

BOOL CSysCADApp::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
  {
  // Discard Command Messages if in Message Wnd
  //if (MsgWndBusy() && nCode != CN_UPDATE_COMMAND_UI)
  //  return 1;

  //return CScdWinApp::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
  if (CWinApp::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
    return 1;

  if (nID==0 || gs_pCmd==NULL)
    return 0;

  BOOL Found = gs_pCmd->FindCmdID(nID); // Command Exists

  if (Found)
    {
    if ((nCode == CN_UPDATE_COMMAND_UI) && (pExtra != NULL))  //see MFC::CmdTarg Line 80
      {
      CCmdUI* pCmdUI = (CCmdUI*)pExtra;
      ASSERT(pCmdUI->m_nID == nID);           // sanity assert
      //ASSERT(!pCmdUI->m_bContinueRouting);    // idle - not set
      pCmdUI->m_bContinueRouting = !gs_pCmd->UpdateCmdID(nID, pCmdUI);
      //(pTarget->*mmf.pfn_UPDATE_COMMAND_UI)(pCmdUI);
      //bOK = !pCmdUI->m_bContinueRouting;
      }
    else if (pExtra==NULL)
      {
      if ((UINT)nCode < 0xC000 || (UINT)nCode > 0xFF00) // see MFC::CmdTarg Line 122
        { // Must Execute This Command
        //TRACE("Exec CmdMsg %5i %5i  %8x %8x",nID, nCode, pExtra, pHandlerInfo);
        gs_pCmd->ExecCmdID(nID);

        //return DispatchCmdMsg(this, nID, nCode,
        //  lpEntry->pfn, pExtra, lpEntry->nSig, pHandlerInfo);
        }
      }
    }

  return Found;
  }

//---------------------------------------------------------------------------

//void CSysCADApp::OnAppAbout()
//  {
//  CAboutBox Dlg;
//  //char* p = NULL;
//  //int i = strlen(p);//force crash for testing...
//  Dlg.DoModal();
//  }

//---------------------------------------------------------------------------

void CSysCADApp::OnHelpSyscad()
  {
  if (1)
    {
    Strng s(BaseCfgFiles());
    s += "User Manual 9.1\\User Manual.htm";
    // Use shell to invoke Web browser on the HTML help file.
    int iRes = (int) ShellExecute(NULL, "open", s(), NULL, NULL, SW_SHOWNORMAL);

    //if(iRes <= 32)
    //  AfxMessageBox(_T("An error occured. Please browse ") + str1 + _T(" manually from your Microsoft Internet Explorer !"), MB_ICONEXCLAMATION);
    }
  else
    {
    Strng s(ProgFiles());
    s += "UserMan.hlp";
    //BOOL b = ::WinHelp(m_pMainWnd->m_hWnd, "syscad.hlp", HELP_CONTENTS, 0);
    BOOL b = ::WinHelp(m_pMainWnd->m_hWnd, s(), HELP_CONTENTS, 0);
    }
  }

//---------------------------------------------------------------------------

void CSysCADApp::OnHelpPgm()
  {
  if (1)
    {
    Strng s(BaseCfgFiles());
    s += "PGM Help 9.1\\PGMHelp.htm";
    // Use shell to invoke Web browser on the HTML help file.
    int iRes = (int) ShellExecute(NULL, "open", s(), NULL, NULL, SW_SHOWNORMAL);

    //if(iRes <= 32)
    //  AfxMessageBox(_T("An error occured. Please browse ") + str1 + _T(" manually from your Microsoft Internet Explorer !"), MB_ICONEXCLAMATION);
    }
  else
    {
    Strng s(ProgFiles());
    s += "pgm.hlp";
    BOOL b = ::WinHelp(m_pMainWnd->m_hWnd, s(), HELP_CONTENTS, 0);
    }
  }

//---------------------------------------------------------------------------

void CSysCADApp::OnHelpModels()
  {
  /*HWND HtmlHelp(HWND    hwndCaller,LPCSTR  pszFile,
  UINT    uCommand, DWORD   dwData) ;*/
  //DWORD dw = 0;
  //::HtmlHelp(m_pMainWnd->m_hWnd, "c:\\syscad90xxx\\Help Files\\User's Manual\\User Manual Revision 13 TOC.htm", HH_DISPLAY_TOC, dw);

  //HtmlHelp(NULL);

  /*STARTUPINFO si;
  memset(&si, 0, sizeof(si));
  si.cb = sizeof(si);
  si.wShowWindow = SW_SHOWDEFAULT;
  PROCESS_INFORMATION pi;
  if (CreateProcess(NULL, "iexplore c:\\syscad90xxx\\Help Files\\User's Manual\\User Manual Revision 13 TOC.htm", NULL, NULL, FALSE, 0, NULL, "c:\\syscad90xxx\\Help Files\\User's Manual\\", &si, &pi))
  {
  //hProcess = pi.hProcess;
  //dwProcessId = pi.dwProcessId;

  //SYSTEMTIME ST;
  //GetSystemTime(&ST);
  //SystemTimeToFileTime(&ST, &EditTime);
  }*/

  if (1)
    {
    //Strng s = "http:\\\\www.syscad.net";
    Strng s(BaseCfgFiles());
    s += "Models Help 9.1\\Models Help.htm";
    // Use shell to invoke Web browser on the HTML help file.
    int iRes = (int)ShellExecute(NULL, "open", s(), NULL, NULL, SW_SHOWNORMAL);

    //if(iRes <= 32)
    //  AfxMessageBox(_T("An error occured. Please browse ") + str1 + _T(" manually from your Microsoft Internet Explorer !"), MB_ICONEXCLAMATION);
    }
  else
    {
    Strng s(ProgFiles());
    s += "models.hlp";
    BOOL b = ::WinHelp(m_pMainWnd->m_hWnd, s(), HELP_CONTENTS, 0);
    }
  }

//---------------------------------------------------------------------------

void CSysCADApp::OnFileOpen()
  {
  CAutoIncDec AID(gs_FileOpenFlag);

  if (!gs_License.Blocked())
    OnFileOpenX();
  //CWinApp::OnFileOpen();
  }

void CSysCADApp::OnUpdateFileOpen(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable((EnableNoPrj() || (EnableNotBusy() && EnableNotAnalysing())) && EnableNotFiling() && EnableNotStopped() && !gs_License.Blocked());
  }

//---------------------------------------------------------------------------

#ifdef SYSCAD10
void CSysCADApp::DummyInsert()
  {
    PKRectangleF boundingRect1 = { 0.0,  0.0, 20.0, 20.0};
    PKRectangleF boundingRect2 = {30.0,  0.0, 20.0, 20.0};
    PKRectangleF boundingRect3 = { 0.0, 30.0, 20.0, 20.0};
    PKRectangleF boundingRect4 = {30.0, 30.0, 20.0, 20.0};
    gs_pPrj->m_pCLRSrvr->CreateItem(1, "5CC47868-AF4E-11DB-9259-AFCD56D89593", "One", "Dummy/", "Tank-1", "Tank-1", boundingRect1, 0.0, 0, 0, false, false);
    gs_pPrj->m_pCLRSrvr->CreateItem(2, "08C8768E-AF5C-11DB-A0C1-F05655D89593", "Two", "Dummy/", "Tank-1", "Tank-1", boundingRect2, 0.0, 0, 0, false, false);
    gs_pPrj->m_pCLRSrvr->CreateItem(3, "0D2CA98E-AF5C-11DB-B82F-F15655D89593", "Three", "Dummy/", "Tank-1", "Tank-1", boundingRect3, 0.0, 0, 0, false, false);
    gs_pPrj->m_pCLRSrvr->CreateItem(4, "1109E6DE-AF5C-11DB-8B86-F35655D89593", "Four", "Dummy/", "Tank-1", "Tank-1", boundingRect4, 0.0, 0, 0, false, false);

    PKPointF controlPoints12[10] = {{20.0,  5.0}, {25.0,  5.0}, {25.0, 15.0}, {30.0, 15.0}};
    PKPointF controlPoints34[10] = {{20.0, 35.0}, {25.0, 35.0}, {25.0, 45.0}, {30.0, 45.0}};
    gs_pPrj->m_pCLRSrvr->CreateLink(5, "ce429210-aff1-11db-abbd-0800200c9a66", "One-Two", "Pipe-1", "5CC47868-AF4E-11DB-9259-AFCD56D89593", "08C8768E-AF5C-11DB-A0C1-F05655D89593", "Side", "Feed", controlPoints12, 4);
    gs_pPrj->m_pCLRSrvr->CreateLink(6, "ce429211-aff1-11db-abbd-0800200c9a66", "Three-Four", "Pipe-1", "0D2CA98E-AF5C-11DB-B82F-F15655D89593", "1109E6DE-AF5C-11DB-8B86-F35655D89593", "Side", "Feed", controlPoints34, 4);
  }

void CSysCADApp::DummyDelete()
  {
    gs_pPrj->m_pCLRSrvr->DeleteItem(1, "5CC47868-AF4E-11DB-9259-AFCD56D89593");
    gs_pPrj->m_pCLRSrvr->DeleteItem(2, "08C8768E-AF5C-11DB-A0C1-F05655D89593");
    gs_pPrj->m_pCLRSrvr->DeleteItem(3, "0D2CA98E-AF5C-11DB-B82F-F15655D89593");
    gs_pPrj->m_pCLRSrvr->DeleteItem(4, "1109E6DE-AF5C-11DB-8B86-F35655D89593");

    gs_pPrj->m_pCLRSrvr->DeleteItem(5, "ce429210-aff1-11db-abbd-0800200c9a66");
    gs_pPrj->m_pCLRSrvr->DeleteItem(6, "ce429211-aff1-11db-abbd-0800200c9a66");
  }
#endif

void CSysCADApp::OnFileNew()
  {
  CAutoIncDec AID(gs_FileNewFlag);

  if (!gs_License.Blocked())
    {
    //SetUsingPrjLclFiles(false);
    if (EnableNoPrj())
      ProjectTemplate().OpenDocumentFile(NULL);
    else
      {
      //CWinApp::OnFileNew();

      CSCDFileNewDlg dlg(AfxGetMainWnd());

      CDocTemplate* pTemplate_ = NULL;
      int nID = dlg.DoModal();
      if (nID == IDOK)
        {
        pTemplate_ = dlg.m_pReqdTemplate;
        }
      else
        return;     // none - cancel operation

      ASSERT(pTemplate_ != NULL);

      pTemplate_->OpenDocumentFile(NULL);
      // if returns NULL, the user has already been alerted
      }
    }
  }

void CSysCADApp::OnUpdateFileNew(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable((EnableNoPrj() || (EnableNotBusy() && EnableNotAnalysing())) && EnableNotFiling() && EnableNotStopped() && !gs_License.Blocked());
  }

#ifdef SYSCAD10
void CSysCADApp::OnUpdateDummyInsert(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(EnableNotBusy() && EnableNotAnalysing() && EnableNotFiling() && EnableNotStopped() && (CWindowLists::GetCurrentTopWindowDoc()!=NULL));
  }
void CSysCADApp::OnUpdateDummyDelete(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(EnableNotBusy() && EnableNotAnalysing() && EnableNotFiling() && EnableNotStopped() && (CWindowLists::GetCurrentTopWindowDoc()!=NULL));
  }
#endif

//---------------------------------------------------------------------------

void CSysCADApp::OnUpdateFileSave(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(EnableNotBusy() && EnableNotAnalysing() && EnableNotFiling() && EnableNotStopped() && (CWindowLists::GetCurrentTopWindowDoc()!=NULL));
  }

void CSysCADApp::OnUpdateFileSaveAs(CCmdUI* pCmdUI)
  {
  BOOL Enable = (EnableNotBusy() && EnableNotAnalysing() && EnableNotFiling() && EnableNotStopped());
  if (Enable)
    {
    // Prevent Saveas of project document AND windows without documents
    CDocument* pDoc = CWindowLists::GetCurrentTopWindowDoc();
    Enable = (pDoc && !pDoc->IsKindOf(RUNTIME_CLASS(CPrjDoc)));
    }
  pCmdUI->Enable(Enable);
  }

void CSysCADApp::OnUpdateFileClose(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(EnableNotBusy() && EnableNotAnalysing() && EnableNotFiling() && EnableNotStopped() && (CWindowLists::GetCurrentTopWindowDoc()!=NULL));
  }

//---------------------------------------------------------------------------

void CSysCADApp::OnProjectSaveall()
  {
  if (EnableNotBusy() && EnableNotAnalysing())
    {
    if (gs_pPrj->m_bReadOnlyPrj)
      LogWarning("SysCAD", LF_Exclamation, "Project marked as read-only, use Project Save As option.");
    else
      gs_pPrj->pPrjDoc->OnSaveDocument(PrjFile());
    }
  }

void CSysCADApp::OnUpdateProjectSaveall(CCmdUI* pCmdUI)
  {
  //flag f1=EnableNoPrj();
  //flag f2=EnableNotBusy();
  //flag f3=EnableNotFiling();
  //CExecutive *p=pExec;
  pCmdUI->Enable(EnableNotBusy() && EnableNotAnalysing() && EnableNotFiling() && EnableNotStopped());
  }

//---------------------------------------------------------------------------

void CSysCADApp::OnProjectClose()
  {
  if (EnableNotBusy() && EnableNotAnalysing() && EnableNotFiling())
    {
    gs_pPrj->bDocChanged = true; //force project save
    gs_pPrj->pPrjDoc->OnCloseDocument();//gs_pPrj->sPrjFile());
    }

  //CWndSlctWnd::CloseIt();
  }

void CSysCADApp::OnUpdateProjectClose(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(EnableNotBusy() && EnableNotAnalysing() && EnableNotFiling() && EnableNotStopped());
  }

//---------------------------------------------------------------------------

void CSysCADApp::OnProjectCloseNoSave()
  {
  if (EnableNotBusy() && EnableNotAnalysing() && EnableNotFiling())
    {
    gs_pPrj->bDocChanged = false; //skip project save
    gs_pPrj->pPrjDoc->OnCloseDocument();//gs_pPrj->sPrjFile());
    }

  //CWndSlctWnd::CloseIt();
  }

void CSysCADApp::OnUpdateProjectCloseNoSave(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(EnableNotBusy() && EnableNotAnalysing() && EnableNotFiling() && EnableNotStopped());
  }

//---------------------------------------------------------------------------

void CSysCADApp::OnProjectOpen()
  {
  int UPB=SetUsingPrjLclFiles(false);
  CString newName;
  if (!DoPromptFileName(newName, AFX_IDS_OPENFILE,
    OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, true, &ProjectTemplate(), false))
    {
    SetUsingPrjLclFiles(UPB);
    return; // open cancelled
    }

  CProject::sm_LoadTypeRqst=PLT_Normal;
  AfxGetApp()->OpenDocumentFile(newName);
  CProject::sm_LoadTypeRqst=PLT_Null;
  }

void CSysCADApp::OnUpdateProjectOpen(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable((EnableNoPrj() || (EnableNotBusy() && EnableNotAnalysing())) && EnableNotFiling() && EnableNotStopped());
  //pCmdUI->Enable(EnableNotBusy() && EnableNotAnalysing() && EnableNotFiling() && EnableNotStopped());
  }

//---------------------------------------------------------------------------

void CSysCADApp::OnProjectOpenLocal()
  {
  if (m_CLH.bAllowLocalCopy)
    {
    //#if (WITHLOCALCOPY)
    if (m_WithLocalCopy)
      {
      int UPB=SetUsingPrjLclFiles(false);
      CString newName;
      if (!DoPromptFileName(newName, IDS_OPENFILELOCAL,
        OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, true, &ProjectTemplate(), false))
        {
        SetUsingPrjLclFiles(UPB);
        return; // open cancelled
        }

      CProject::sm_LoadTypeRqst=PLT_Local;
      AfxGetApp()->OpenDocumentFile(newName);
      CProject::sm_LoadTypeRqst=PLT_Null;
      }
    //#endif
    }
  }

void CSysCADApp::OnUpdateProjectOpenLocal(CCmdUI* pCmdUI)
  {
  if (m_CLH.bAllowLocalCopy)
    {
    //#if (WITHLOCALCOPY)
    if (m_WithLocalCopy)
      pCmdUI->Enable((EnableNoPrj() || (EnableNotBusy() && EnableNotAnalysing())) && EnableNotFiling() && EnableNotStopped());
    else
      pCmdUI->Enable(0);
    }
  else//  #else
    {
    pCmdUI->Enable(0);
    //#endif
    }
  }

//---------------------------------------------------------------------------

void CSysCADApp::OnProjectNew()
  {
  if ((EnableNoPrj() || (EnableNotBusy() && EnableNotAnalysing())) && EnableNotFiling() && !gs_License.Blocked())
    //gs_pPrj->OnNew(); DOES NOT WORK!!!
    ProjectTemplate().OpenDocumentFile(NULL);
  }

void CSysCADApp::OnUpdateProjectNew(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable((EnableNoPrj() || (EnableNotBusy() && EnableNotAnalysing() && EnableNotStopped())) && EnableNotFiling() && !gs_License.Blocked());
  }

//---------------------------------------------------------------------------

void CSysCADApp::OnProjectEditSettings()
  {
  if (!EnablePrjOK() && gs_License.AllowFullLic())
    {
    CWaitCursor Wait;
    // prompt the user (with all document templates)
    CString PrjFile;
    if (!DoPromptFileName(PrjFile, AFX_IDS_OPENFILE,
      OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, true, NULL, (gs_pPrj && gs_pPrj->pPrjDoc)))
      return; // Edit cancelled

    CHistSettings Hst;
    CProjectSettings Sets(Hst);
    CProfINIFile PF(PrjFile);
    Sets.ReadSettings(PF, 10000);
    if (Sets.EditSettings("Edit Project Settings", NULL))
      Sets.WriteSettings(PF, false);


    //CPrjSettingsSheet Dlg(PrjFile, "Project Settings", MainWnd(), CPrjSettingsSheet::iCurrentPage);
    //if ((Dlg.DoModal()==IDOK))
    //  {
    //  }
    }
  }

void CSysCADApp::OnUpdateProjectEditSettings(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(!EnablePrjOK() && gs_License.AllowFullLic());
  }

//---------------------------------------------------------------------------

void CSysCADApp::OnUpdateAppExit(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable((EnableNoPrj() || (EnableNotBusy() && EnableNotAnalysing() && EnableNotStopped())) && EnableNotFiling());
  }

//---------------------------------------------------------------------------

void CSysCADApp::OnLicense(UINT nID)
  {
#if CK_LICENSINGON
  switch (nID)
    {
    case ID_CK_LIC_RESETSERVICE:
      if (IsWinNT)// && AfxMessageBox("Are you sure?")==ID?;
        {
        CDlgBusy::Open("\n\nStop and Start Licensing Service");
        if (gs_License.ResetService(true))
          LogNote("SysCAD", 0, "Licensing Service Driver Reset.");
        else
          LogWarning("SysCAD", 0, "License Service Driver Reset FAILED!");
        CDlgBusy::SetLine(2, "Checking License");
        if (gs_License.Init())
          {
          if (!gs_License.Check())
            {
#if CK_ALLOWDEMOMODE
            gs_License.SetDemoMode();
#endif
            }
          }
        gs_License.QuickCheck(1);
        CDlgBusy::Close();
        }
      break;
    case ID_CK_LIC_TRIAL:
      if (gs_License.IssueTrial(CK_NoOfTrialDays, true))
        LogNote("SysCAD", 0, "Trial License has been issued.");
      break;
    case ID_CK_LIC_LOCATION:
      {
      if (m_CLH.bAltLicenseLoc)
        {
        LogWarning("License", LF_Exclamation, "Location cannot be changed, as it was set by command line '/l:%s'", m_CLH.sLicenseLoc());
        }
      else
        {
        int err = gs_License.SetLocation();
        if (err==3)
          LogWarning("SysCAD", 0, "License location unchanged.");
        else if (err==1 || err==5)
          LogNote("SysCAD", 0, "License location has been set to %s.", gs_License.GetAppPath());
        else if (err==2)
          LogWarning("SysCAD", 0, "License location change failed, set back to %s.", gs_License.GetAppPath());
        else if (err==4)
          LogWarning("SysCAD", 0, "License location change failed!  Location has been set to %s.", gs_License.GetAppPath());
        else if (err==6)
          LogWarning("SysCAD", 0, "License location change failed!  Now in Demo Mode. Location has been set to %s.", gs_License.GetAppPath());
        }
      break;
      }
    case ID_CK_LIC_AUTH:
      if (gs_License.IssueLicense())
        LogNote("SysCAD", 0, "License has been issued.");
      break;
    case ID_CK_LIC_KILL:
      {
      CString ConfirmCode;
      if (gs_License.Kill(ConfirmCode))
        LogNote("SysCAD", LF_Exclamation, "License has been removed.\nConfirmation code '%s'", (const char*)ConfirmCode);
      break;
      }
    case ID_CK_LIC_REGTRANSFER:
      if (gs_License.DoRegisterTransfer())
        LogNote("SysCAD", 0, "License transfer has been registered.");
      break;
    case ID_CK_LIC_TRANSOUT:
      if (gs_License.DoTransferOut())
        LogNote("SysCAD", LF_Exclamation, "License has been transferred out.");
      break;
    case ID_CK_LIC_TRANSIN:
      if (gs_License.DoTransferIn())
        LogNote("SysCAD", 0, "License has been transferred in.");
      break;
    case ID_CK_LIC_DIRECTTRANS:
      if (gs_License.DoDirectTransfer())
        LogNote("SysCAD", LF_Exclamation, "License has been transferred.\nSysCAD will need to be restarted!");
      break;
    case ID_CK_LIC_INFO:
      gs_License.Info();
      break;
    case ID_CK_LIC_NETUSERS:
      gs_License.NetworkUsersInfo();
      break;
    }
  if (gs_pPrj)
    {
    gs_pPrj->CheckLicenseConditions();
    if (gs_pPrj->FlwLib())
      InitClassLicenses(gs_License.LicCatagories());
    }
  gs_License.QuickCheck(0);
  ScdMainWnd()->PostMessage(WMU_UPDATEMAINWND, SUB_UPDMAIN_BACKGROUND, 0);
#endif
  }

//---------------------------------------------------------------------------

void CSysCADApp::OnUpdateLicense(CCmdUI* pCmdUI)
  {
#if CK_LICENSINGON
  flag NotActive = (!EnableActive() && !gs_License.Blocked() && !gs_License.AcademicMode());
  switch (pCmdUI->m_nID)
    {
    case ID_CK_LIC_RESETSERVICE:
      pCmdUI->Enable(IsWinNT && !gs_License.AcademicMode());
      break;
    case ID_CK_LIC_TRIAL:
      pCmdUI->Enable(!gs_License.Licensed() && NotActive && gs_License.DidInitCrypkey() && !gs_License.TrialFailed());
      break;
    case ID_CK_LIC_LOCATION:
      pCmdUI->Enable(NotActive);
      break;
    case ID_CK_LIC_AUTH:
      pCmdUI->Enable(NotActive && gs_License.DidInitCrypkey());
      break;
    case ID_CK_LIC_INFO:
      pCmdUI->Enable(true);
      break;
    case ID_CK_LIC_NETUSERS:
      pCmdUI->Enable(NotActive && gs_License.MultiUsers()>0);
      break;
    case ID_CK_LIC_KILL:
    case ID_CK_LIC_TRANSOUT:
    case ID_CK_LIC_DIRECTTRANS:
      pCmdUI->Enable(gs_License.Licensed() && NotActive);
      break;
    case ID_CK_LIC_REGTRANSFER:
    case ID_CK_LIC_TRANSIN:
      pCmdUI->Enable(!gs_License.Licensed() && NotActive && gs_License.DidInitCrypkey());
      break;
    }
#else
  pCmdUI->Enable(false);
#endif
  }

//---------------------------------------------------------------------------

static void AppendFilterSuffix(CString& filter, OPENFILENAME& ofn, CDocTemplate* pTemplate_, CString* pstrDefaultExt)
  {
  ASSERT_VALID(pTemplate_);
  ASSERT_KINDOF(CDocTemplate, pTemplate_);

  CString strFilterExt, strFilterName;
  if (pTemplate_->GetDocString(strFilterExt, CDocTemplate::filterExt) &&
    !strFilterExt.IsEmpty() &&
    pTemplate_->GetDocString(strFilterName, CDocTemplate::filterName) &&
    !strFilterName.IsEmpty())
    {
    // a file based document template - add to filter list
    ASSERT(strFilterExt[0] == '.');
    if (pstrDefaultExt != NULL)
      {
      // set the default extension
      *pstrDefaultExt = ((LPCTSTR)strFilterExt) + 1;  // skip the '.'
      ofn.lpstrDefExt = (LPTSTR)(LPCTSTR)(*pstrDefaultExt);
      ofn.nFilterIndex = ofn.nMaxCustFilter + 1;  // 1 based number
      }

    // add to filter
    filter += strFilterName;
    ASSERT(!filter.IsEmpty());  // must have a file type name
    filter += (TCHAR)'\0';  // next string please
    filter += (TCHAR)'*';
    filter += strFilterExt;
    filter += (TCHAR)'\0';  // next string please
    ofn.nMaxCustFilter++;
    }
  }

BOOL CSysCADApp::DoPromptFileName(CString& fileName, UINT nIDSTitle, DWORD lFlags, BOOL bOpenFileDialog, CDocTemplate* pTemplate_, BOOL AddAllFilter)
  {
  CDocManager & DM=*m_pDocManager;

  //  CSimpleFileName DlgS(bOpenFileDialog);
  CSCDFileDialog     Dlg(bOpenFileDialog);
  Dlg.m_DoingPrjOpen=true;

  OPENFILENAME & m_ofn = Dlg.m_ofn;

  m_ofn.Flags |= lFlags;

  CString strTitle;
  VERIFY(strTitle.LoadString(nIDSTitle));
  CString strFilter;
  CString strDefault;
  CString strInitialDir;

  if (gs_pPrj==NULL || gs_pPrj->pPrjDoc==NULL)
    {
    pTemplate_ = pTemplate[iProjectTemplate];
    AddAllFilter = false;
    }

  if (pTemplate_==pTemplate[iProjectTemplate])
    {
    Strng s = ScdPFUser.RdStr("ProjectSets", "MostRecent", "?");
    s.FnClearEndBSlash();
    if (s!="?" && FileExists(s()))
      strInitialDir = s();
    else
      {
      strInitialDir = ProgFiles();
      if (strInitialDir[1]==':')
        {
        strInitialDir=strInitialDir.Left(2);
        strInitialDir+="\\SysCAD Projects\\";
        }
      //if (_stricmp(strInitialDir.Right(4), "bin\\")==0)
      //  strInitialDir = strInitialDir.Left(strInitialDir.GetLength()-4);
      }
    strTitle += " Project";
    }
  else
    {
    strInitialDir = PrjFiles();
    Dlg.m_DoingPrjOpen=false;  // these tests should only apply for the project files
    //strTitle
    }

  if (pTemplate_ != NULL)
    {
    ASSERT_VALID(pTemplate_);
    AppendFilterSuffix(strFilter, m_ofn, pTemplate_, &strDefault);
    }
  else
    {
    BOOL bFirst = true;
    if (AddAllFilter)
      {
      CString strExtList;
      CString strFilterExt;
      for (int i=0; i<getTemplateCount(); i++)
        //if (pTemplate[i])
        if (i!=iProjectTemplate && pTemplate[i])
          {
          if (pTemplate[i]->GetDocString(strFilterExt, CDocTemplate::filterExt) && !strFilterExt.IsEmpty())
            {
            strExtList += (strExtList.GetLength()==0 ? _T("*") : _T(";*"));
            strExtList += strFilterExt;
            }
          }

        strFilter += _T("All SysCAD Files(");
        strFilter += strExtList;
        strFilter += _T(")");
        strFilter += (TCHAR)'\0';   // next string please
        strFilter += strExtList;
        strFilter += (TCHAR)'\0';   // last string
        m_ofn.nMaxCustFilter++;
        bFirst = false;
      }

    // do for all doc template
    POSITION pos = DM.GetFirstDocTemplatePosition();
    while (pos != NULL)
      {
      CDocTemplate* pTmplt = DM.GetNextDocTemplate(pos);
      AppendFilterSuffix(strFilter, m_ofn, pTmplt, bFirst ? &strDefault : NULL);
      bFirst = false;
      }
    }

  // append the "*.*" all files filter
  if (AddAllFilter)
    {
    CString allFilter;
    VERIFY(allFilter.LoadString(AFX_IDS_ALLFILTER));
    strFilter += allFilter;
    strFilter += (TCHAR)'\0';   // next string please
    strFilter += _T("*.*");
    strFilter += (TCHAR)'\0';   // last string
    m_ofn.nMaxCustFilter++;
    }

  m_ofn.lpstrFilter = strFilter;
  m_ofn.lpstrTitle = strTitle;
  m_ofn.lpstrFile = fileName.GetBuffer(_MAX_PATH);
  m_ofn.lpstrInitialDir = strInitialDir;
  //  BOOL bResult = dlgFile.DoModal() == IDOK ? true : false;
  BOOL bResult= Dlg.DoModal() == IDOK ? true : false;
  fileName.ReleaseBuffer();
  return bResult;
  }

//---------------------------------------------------------------------------

void CSysCADApp::OnFileNewX()
  {
  }

//---------------------------------------------------------------------------

void CSysCADApp::OnFileOpenX()
  {
  // prompt the user (with all document templates)
  CString newName;
  if (!DoPromptFileName(newName, AFX_IDS_OPENFILE,
    OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, true, NULL, (gs_pPrj && gs_pPrj->pPrjDoc)))
    return; // open cancelled

  AfxGetApp()->OpenDocumentFile(newName);
  // if returns NULL, the user has already been alerted
  }

//---------------------------------------------------------------------------

void CSysCADApp::AddToRecentFileList(LPCTSTR lpszPathName)
  {
  if (m_HoldAddToRecentFileList)
    return;

  ASSERT_VALID(this);
  ASSERT(lpszPathName != NULL);
  ASSERT(AfxIsValidString(lpszPathName));
  Strng F;
  F.FnExt((char*)lpszPathName);
  if (F.XStrICmp(".spj")==0)
    {
    F.FnDrivePath((char*)lpszPathName);
    F.FnClearEndBSlash();
    }
  else
    F=lpszPathName;

  CWinApp::AddToRecentFileList(F());
  }

//---------------------------------------------------------------------------

BOOL CSysCADApp::OnOpenRecentFile(UINT nID)
  {
  ASSERT_VALID(this);
  ASSERT(m_pRecentFileList != NULL);

  ASSERT(nID >= ID_FILE_MRU_FILE1);
  ASSERT(nID < ID_FILE_MRU_FILE1 + (UINT)m_pRecentFileList->GetSize());
  int nIndex = nID - ID_FILE_MRU_FILE1;
  ASSERT((*m_pRecentFileList)[nIndex].GetLength() != 0);

  TRACE2("MRU: open file (%d) '%s'.\n", (nIndex) + 1,
    (LPCTSTR)(*m_pRecentFileList)[nIndex]);

  Strng Fn=(*m_pRecentFileList)[nIndex];


  WIN32_FIND_DATA fd;
  HANDLE H = FindFirstFile(Fn(), &fd);
  if (H!=INVALID_HANDLE_VALUE)
    if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
      //  Strng T;
      //  T.FnExt(Fn());
      //  if (T.XStrICmp(".spf")==0)
      {
      Fn.FnCheckEndBSlash();
      Fn+="Project.spj";
      }
    FindClose(H);

    if (OpenDocumentFile(Fn()) == NULL)
      m_pRecentFileList->Remove(nIndex);

    return true;

  }

//===========================================================================

bool CCmdLineHelper::Parse(char* pCmdLine)
  {
  //parse command line...
  bAllowLoadPrevPrj = 1;
  bLoadPrevPrj = 0;
  bDebugOn = 0;
  bDeveloper = 0;
  bAutomation = 0;
  bAltLicenseLoc = 0;
  bForceNewCopy = 0;
  bUse2000 = 0;
  bUse97 = 0;
  bForceRuntime = 0;
  bForceDemo = 0;
  dwAffinity = 0;
  bForegroundWnd = true;
  bMinimiseWnd = false;
  nAutoRunWait =  0;
#if (WITHLOCALCOPY)
  bAllowLocalCopy = true;
#else
  bAllowLocalCopy = false;
#endif
#if (WITHXMENUS)
  bAllowXMenus = true;
#else
  bAllowXMenus = false;
#endif
  lGobbleMemUntil = -1;
  sAutoRunPrj = "";
  sAutoLoadPrj = "";
  sAutoLoadSnp = "";
  sDebugFile = "";
  sCmdLine = "";
  sLicenseLoc = "";
  sScriptCmd = "";
  ScriptFiles.SetSize(0);

  char CmdLineCopy[2048]; //work on copy!!!
  strcpy(CmdLineCopy, pCmdLine ? pCmdLine : "");
  if (ScdPFUser.RdInt("Debug", "On", 0)) // force debug
    strcat(CmdLineCopy, " /d");

  if (strlen(CmdLineCopy)>0)
    {
    sCmdLine = CmdLineCopy; //keep copy of command line

    pchar p = strtok(CmdLineCopy, " \n");
    BOOL IsOneCmd=false;
    while (p && !IsOneCmd)
      {
      CString OptRgt, OptLft2 = p, Option = p;
      int l = OptLft2.GetLength(), OptWide=2;
      if ((l>OptWide) && (OptLft2[2]==':'))
        OptWide = 3;
      OptRgt = OptLft2.Right(l-OptWide);
      OptLft2 = OptLft2.Left(2);
      if (OptRgt.GetLength()>0 && OptRgt[0]=='"' && (OptLft2=="/d" || OptLft2=="/l" || OptLft2=="/r"))
        {//find closing "
        OptRgt = OptRgt.Mid(1, 1024);
        if (OptRgt.GetLength()>0 && OptRgt[OptRgt.GetLength()-1]=='"')
          OptRgt = OptRgt.Left(OptRgt.GetLength()-1);
        else
          {
          OptRgt += " ";
          p = strtok(NULL, "\"\n");
          if (p)
            OptRgt += p;
          }
        }
      Option=Option.MakeLower();

      if (Option=="-embedding")
        {
        bAutomation = true;
        }
      else if (Option=="/2000")
        {
        bUse2000 = 1;
        }
      else if (Option=="/97")
        {
        bUse97 = 1;
        }
      else if (Option=="/1997")
        {
        bUse97 = 1;
        }
      else if (Option=="/demo")
        {
        bForceDemo = 1;
        }
      else if (Option=="/dev")
        {
        if (!gs_License.HashDefineCurtin())
          bDeveloper = 1; //only allow developer mode if NOT Academic license
        }
      else if (OptLft2=="/d")
        {
        bDebugOn = true;
        if (OptRgt.GetLength()==0)
          {
#if BLDDEPENDENTFILES
          OptRgt.Format("%sScd_Dbg.%i.Txt", TemporaryFiles(), SCD_BUILD_NUM);
#else
          OptRgt = TemporaryFiles();
          OptRgt += "Scd_Dbg.Txt";
#endif
          }
        else if (strpbrk(OptRgt.GetBuffer(0), ":\\")==NULL)
          {
          CString T(OptRgt);
          OptRgt = TemporaryFiles();
          OptRgt += T;
          }
        sDebugFile = (const char*)OptRgt;
        }
      else if (OptLft2=="/f")
        {
        bForegroundWnd = (OptRgt.GetLength() && (OptRgt[0]=='Y'||OptRgt[0]=='y'));
        }
      else if (OptLft2=="/g")
        {
        if (OptRgt.GetLength())
          lGobbleMemUntil=atol((const char*)OptRgt);
        }
      else if (Option=="/lc")
        {
        bAllowLocalCopy = 1;
        }
      else if (OptLft2=="/l")
        {
        sLicenseLoc = OptRgt;
        if (sLicenseLoc.Len()>0)
          bAltLicenseLoc = true;
        }
      else if (Option=="/min")
        {
        bMinimiseWnd = 1;
        }
      else if (Option=="/n")
        {
        bForceNewCopy = true;
        }
      else if (OptLft2=="/a")
        {
        if (OptRgt.GetLength())
          dwAffinity = SafeAtoI((const char *)OptRgt, 1);
        else
          dwAffinity = 0x00000001;
        }
      else if (Option=="/rt")
        {
        bForceRuntime = 1;
        }
      else if (Option=="/r")
        {
        if (OptRgt.GetLength()>0)
          {
          sAutoRunPrj = (const char*)OptRgt;
          bAllowLoadPrevPrj = false;
          }
        else
          if (AfxGetMainWnd())
            LogWarning("SysCAD", 0, "No Autorun Project specified");
        }
      else if (Option=="/p")
        {
        bLoadPrevPrj = true;
        }
      else if (OptLft2=="/c")
        {
        IsOneCmd = true;
        sScriptCmd=OptRgt;
        p=strtok(NULL, "\n");// Try to append any more
        if (p)
          sScriptCmd+=p;
        }
      else if (Option=="/?")
        {
        printf("HELP\n");
        return false;
        }
      else if (OptLft2=="/w")
        {
        nAutoRunWait=atol(OptRgt);
        }
      else if (Option=="/xmenu")
        {
        bAllowXMenus = 1;
        }
      else if (OptLft2[0]!='/')
        {//assume filename...
        bAllowLoadPrevPrj = false;
        Strng s(p);
        if (s.Len()>0 && (s[0]=='"' || s[0]=='\''))
          {//find closing " or '
          s = s.Mid(1, 1024);
          if (s.CmpLastChar('"') || s.CmpLastChar('\''))
            s = s.Left(s.Len()-1);
          else
            {
            s += " ";
            p = strtok(NULL, "\"\'\n");
            if (p)
              s += p;
            }
          }
        Strng FnXO(s);
        Strng FnNX(s);
        FnNX.FnNameExt();
        FnXO.FnExt();
        if (FnXO.XStrICmp(".ssc")==0)
          {//command script
          ScriptFiles.Add(s());
          }
        else if (FnXO.XStrICmp(".snp")==0)
          {//command script
          sAutoLoadSnp = s();
          }
        else
          {//assume spj file
          if (sAutoLoadPrj.Len()==0)
            {
            sAutoLoadPrj = s();
            if (FnNX.XStrICmp("Project.spj")!=0)
              {
              if (!sAutoLoadPrj.CmpLastChar('\\'))
                sAutoLoadPrj += '\\';
              sAutoLoadPrj += "Project.spj";
              }
            }
          else
            if (AfxGetMainWnd())
              LogWarning("SysCAD", 0, "Additional project file '%s' ignored", p);
          }
        }
      else
        {
        LogWarning("SysCAD", 0, "Unknown Option '%s' ignored", p);
        }
      p = strtok(NULL, " \n");
      }
    }
  return true;
  }

/////////////////////////////////////////////////////////////////////////////
// CSysCADApp message handlers

CSysCADModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
  OBJECT_ENTRY(CLSID_ScdApplication , CScdApplication)
  OBJECT_ENTRY(CLSID_ScdAppTag      , CScdAppTag     )
  OBJECT_ENTRY(CLSID_ScdAppTags     , CScdAppTags    )
  OBJECT_ENTRY(CLSID_ScdGraphic     , CScdGraphic    )
  OBJECT_ENTRY(CLSID_ScdGraphics    , CScdGraphics   )
  OBJECT_ENTRY(CLSID_ScdTrend       , CScdTrend      )
  OBJECT_ENTRY(CLSID_ScdTrends      , CScdTrends     )
  OBJECT_ENTRY(CLSID_ScdHistorian   , CScdHistorian  )
  OBJECT_ENTRY(CLSID_ScdLicenseApp  , CScdLicenseApp )
  OBJECT_ENTRY(CLSID_ScdOptions     , CScdOptions    )
  OBJECT_ENTRY(CLSID_ScdProject     , CScdProject    )
  OBJECT_ENTRY(CLSID_ScdReplay      , CScdReplay     )
  OBJECT_ENTRY(CLSID_ScdReport      , CScdReport     )
  OBJECT_ENTRY(CLSID_ScdReports     , CScdReports    )
  OBJECT_ENTRY(CLSID_ScdSnapshot    , CScdSnapshot   )
  OBJECT_ENTRY(CLSID_ScdOPCServer   , CScdOPCServer  )
  OBJECT_ENTRY(CLSID_ScdIOMarshal   , CScdIOMarshal  )
  OBJECT_ENTRY(CLSID_ScdDDEServer   , CScdDDEServer  )
END_OBJECT_MAP()

const int NoAggObjectsInApp = 0;
CEvent gs_evtAppComThreadDone;

LONG CSysCADModule::Unlock()
  {
  //AfxOleUnlockApp();
  AFX_MODULE_STATE* pModuleState = AfxGetModuleState();
  ASSERT(pModuleState->m_nObjectCount != 0);

  TRACE2(" UnLock ? %3i %3i\n", pModuleState->m_nObjectCount, NoAggObjectsInApp);

  if (InterlockedDecrement(&pModuleState->m_nObjectCount) == NoAggObjectsInApp)
    {
    //::AfxOleOnReleaseAllObjects();

    // allow application to shut down when all the objects have
    //  been released
    // don't shut down the application if user is in control of the app
    if (1 && AfxOleGetUserCtrl())
      return 0;

    AfxOleSetUserCtrl(TRUE);    // avoid re-entry

    // shut down the application .. do not save
    gs_pXCmd->bNoSaveOnExit=true;
    gs_pXCmd->bSaveOnExit=false;
    //_asm int 3;
    /*AfxGet*/MainWnd()->PostMessage(WM_COMMAND, ID_APP_EXIT, 0);
    AfxGetApp()->PostThreadMessage(WM_QUIT, 0, 0);
    //_asm int 3;

    HANDLE WtHnds[2] = {gs_evtAppComThreadDone};
    WaitForMultipleObjects(1, WtHnds, true, 10000);
    //Sleep(1000); // let other threads shutdown
    }
  return 0;
  }

LONG CSysCADModule::Lock()
  {
  AfxOleLockApp();
  return 1;
  }

BOOL CSysCADApp::InitATL()
  {
  m_bATLInited = TRUE;

  _Module.Init(ObjectMap, AfxGetInstanceHandle());
  _Module.dwThreadID = GetCurrentThreadId();

  return TRUE;
  }


//==========================================================================

class CScdAppComThread //: public CScdAppIF
  {
  public:
    //static DWORD WINAPI StaticThreadProc( LPVOID lpv );
    void RegisterAndStart(DWORD ComEvtMsg);
    CScdAppComThread();
    virtual ~CScdAppComThread();

    void StopAndUnRegister();

    void DoThreadStartupStuff();
    void DoThreadShutdownStuff();
    DWORD WINAPI ThreadProc();

    DWORD                     m_dwRegClsObj;
    IClassFactory            *pICFUnk;
    DWORD                     m_dwRegActObj;

    //HANDLE       m_hThreadProc;
    CWinThread * m_pThread;
    //DWORD        m_dwThreadId;
    DWORD        m_dwEvtMsgId;
    CScdCOEventSender m_EventSender;
  };

//--------------------------------------------------------------------------

CScdAppComThread::CScdAppComThread()
  {
  //m_hThreadProc=0;
  //m_dwThreadId=0;
  m_dwEvtMsgId=0;
  };

//--------------------------------------------------------------------------

CScdAppComThread::~CScdAppComThread()
  {
  };

//--------------------------------------------------------------------------

UINT StaticThreadProc(LPVOID lpv)
  {
  UINT Ret=0;
  SCDTRY_THREAD
    Ret=static_cast<CScdAppComThread*>(lpv)->ThreadProc();
  SCDCATCH_THREAD("CScdAppComThread::StaticThreadProc")
  return Ret;
  }

//--------------------------------------------------------------------------

void CScdAppComThread::RegisterAndStart(DWORD EvtMsgId)
  {
  m_dwEvtMsgId = EvtMsgId;
  m_pThread= ::AfxBeginThread((AFX_THREADPROC)StaticThreadProc, (LPVOID)this, THREAD_PRIORITY_NORMAL, 100000, CREATE_SUSPENDED, NULL);//&m_dwThreadId);
  SetThreadName(m_pThread->m_nThreadID, "Application COM");
  // if ((pObjThread = AfxBeginThread(EO_ExecThread, (LPVOID)this, THREAD_PRIORITY_NORMAL, StackSize, CREATE_SUSPENDED))==NULL)


  m_EventSender.Init(m_pThread->m_nThreadID, m_dwEvtMsgId);
  gs_Events.Add(&m_EventSender);

  m_pThread->ResumeThread();
  }

//--------------------------------------------------------------------------

void CScdAppComThread::StopAndUnRegister()
  {
  gs_Events.Remove(&m_EventSender);

  if (m_pThread)
    {
    PostThreadMessage(m_pThread->m_nThreadID, WM_QUIT, 0, 0);
    if ( WAIT_TIMEOUT == ::WaitForSingleObject(m_pThread->m_hThread, 10000 ))
      {
      ::TerminateThread( m_pThread->m_hThread , 0 );
      }
    }
  }

//--------------------------------------------------------------------------

void CScdAppComThread::DoThreadStartupStuff()
  {
  TRACE0("CScdAppComThread : DoThreadStartupStuff\n");

  HRESULT hr=S_OK;

  //???????/

  _Module.RegisterClassObjects(CLSCTX_LOCAL_SERVER, REGCLS_MULTIPLEUSE|REGCLS_SUSPENDED);

  hr=::CoResumeClassObjects();
  SCD_TOF(hr, "ALBDoStartupStuff:CoResumeClassObjects");

  }

//--------------------------------------------------------------------------

void CScdAppComThread::DoThreadShutdownStuff()
  {
  TRACE0("CScdAppComThread : DoThreadShutdownStuff\n");

  //TODO COMPLETE --------------

  _Module.RevokeClassObjects();
  }

//--------------------------------------------------------------------------

DWORD WINAPI CScdAppComThread::ThreadProc()
  {
  try
    {
    ::CoInitializeEx( 0 , COINIT_MULTITHREADED );

    DoThreadStartupStuff();

    if (1)
      {
      MSG msg;
      while (GetMessage(&msg,NULL,0,0)>0)
        {
        if (msg.message==m_dwEvtMsgId)
          {
          AtlTrace(">>CScdAppComThread::DoEventMsg\n");

          CScdCOCmdBase::FireEventMsgs(msg.wParam, msg.lParam);

          //        m_pCallback->DoEventMsg(msg.wParam, msg.lParam);
          AtlTrace("<<CScdAppComThread::DoEventMsg\n");
          }
        else
          Sleep(1);
        }
      }

    DoThreadShutdownStuff();

    gs_evtAppComThreadDone.SetEvent();

    Sleep(1000);

    ::CoUninitialize( );
    return 0;
    }
  catch(...)
    {
    //  if (TestLogOption(LogOption_COMCallsInternal))
    LogError("Com", 0, "CScdAppComThread Exception Occurred");
    //  throw CScdComException (LclHR, Desc);
    return -1;
    }
  }

//==========================================================================

static CScdAppComThread *gs_ptheThread=NULL;

//--------------------------------------------------------------------------

void StartAppComThread(DWORD EvtMsgId)
  {
  //  _Module.Init(ObjectMap, AfxGetInstanceHandle());
  //  _Module.dwThreadID = GetCurrentThreadId();

  gs_ptheThread = new CScdAppComThread;
  gs_ptheThread ->RegisterAndStart(EvtMsgId);
  //gs_pScdAppIF = gs_ptheThread;
  }

//--------------------------------------------------------------------------

void StopAppComThread()
  {
  //gs_pScdAppIF = NULL;
  if (gs_ptheThread)
    gs_ptheThread->StopAndUnRegister();
  delete gs_ptheThread;
  }

