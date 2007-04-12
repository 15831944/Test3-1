// SysCADMarshal.cpp : Defines the class behaviors for the application.
//
#include "stdafx.h"
#include "SysCADMarshal.h"
  
#include "MainFrm.h"
#include "SysCADMarshalDoc.h"
#include "SysCADMarshalView.h"
#include "slotmngr.h"
#include <initguid.h>
#include "SysCADMarshal_i.c"
#include "ScdMarshal.h"
#include ".\syscadmarshal.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#import "c:\program files\common files\system\ado\msado15.dll" rename("EOF", "adEOF"), implementation_only
//#import "c:\Program Files\Common Files\system\ado\msadox.dll" implementation_only
//#import "c:\Program Files\Common Files\system\ado\msjro.dll"  implementation_only
#import "c:\program files\common files\system\ado\msado15.dll" rename("EOF", "adEOF") implementation_only
#import "c:\Program Files\Common Files\system\ado\msadox.dll" implementation_only


#define ON_APP_MESSAGE(message, memberFxn)    \
    { message, 0, 0, 0, AfxSig_lwl, \
        (AFX_PMSG) (AFX_PMSGW) (static_cast< LRESULT (AFX_MSG_CALL CWinApp::*)(WPARAM, LPARAM) > (&memberFxn)) },
  
/////////////////////////////////////////////////////////////////////////////
// CSysCADMarshalApp
BEGIN_MESSAGE_MAP(CSysCADMarshalApp, CWinApp)
//{{AFX_MSG_MAP(CSysCADMarshalApp)
  ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
// NOTE - the ClassWizard will add and remove mapping macros here.
//    DO NOT EDIT what you see in these blocks of generated code!
//}}AFX_MSG_MAP
// Standard file based document commands
  ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
  ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
// Standard print setup command
  ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
//  ON_APP_MESSAGE(WMU_UPDATEALL, OnUpdateAll)
//  ON_APP_MESSAGE(WMU_UPDATETAGVALUES, OnUpdateTagValues)
  ON_APP_MESSAGE(WMU_ADDERROR, OnAddError)
  ON_APP_MESSAGE(WMU_OPENDOC,  OnOpenDoc)
  ON_APP_MESSAGE(WMU_CLOSEDOC, OnCloseDoc)
  ON_APP_MESSAGE(WMU_SETSLOTCFG, OnSetSlotCfg)
  ON_APP_MESSAGE(WMU_SETLINKCFG, OnSetLinkCfg)
  ON_APP_MESSAGE(WMU_SETDEVICECFG, OnSetDeviceCfg)
  ON_APP_MESSAGE(WMU_SETCDBLKCFG, OnSetCdBlkCfg)
  ON_APP_MESSAGE(WMU_UPDATEONESLOT, OnUpdateOneSlot)
  ON_APP_MESSAGE(WMU_UPDATEONELINK, OnUpdateOneLink)
  ON_APP_MESSAGE(WMU_UPDATEONESLOTSTATE, OnUpdateOneSlotState)
  ON_APP_MESSAGE(WMU_UPDATEONELINKSTATE, OnUpdateOneLinkState)
  ON_APP_MESSAGE(WMU_UPDATESTATUS, OnUpdateStatus)
  ON_APP_MESSAGE(WMU_XFERCHANGEBLOCK, OnXferChangeBlock)
  //ON_APP_MESSAGE(WMU_SETSELECTION, OnSetSelection)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////

// CSysCADMarshalApp construction
CSysCADMarshalApp::CSysCADMarshalApp(void) :
  m_OpenCloseSema(0,1)
  {
  // TODO: add construction code here,
  // Place all significant initialization in InitInstance
  }

/////////////////////////////////////////////////////////////////////////////
// The one and only CSysCADMarshalApp object
CSysCADMarshalApp     theApp;

// This identifier was generated to be statistically unique for your app.
// You may change it if you prefer to choose a specific identifier.
// {01E21D76-DF1D-43DD-A2A9-6A40C14765B1}
static const CLSID  clsid =
  {
  0x1e21d76,
  0xdf1d,
  0x43dd,
  { 0xa2, 0xa9, 0x6a, 0x40, 0xc1, 0x47, 0x65, 0xb1 }
  };

/////////////////////////////////////////////////////////////////////////////

// CSysCADMarshalApp initialization
BOOL CSysCADMarshalApp::InitInstance(void)
  {
  // Initialize OLE libraries
  if(!AfxOleInit())
    {
    AfxMessageBox(IDP_OLE_INIT_FAILED);
    return FALSE;
    }

  if(!InitATL()) return FALSE;

  if(1)
    {
    // Everyone can connect back to IAdviseSink etc
    HRESULT hr = CoInitializeSecurity(NULL,-1,NULL,NULL,RPC_C_AUTHN_LEVEL_NONE,
                                      RPC_C_IMP_LEVEL_IDENTIFY,NULL,EOAC_NONE,NULL);
    if(FAILED(hr))
      {
      CString S;
      S.Format("CoInitializeSecurity failed, %lx", hr);
      AfxMessageBox(S);
      }

    ASSERT(SUCCEEDED(hr));
    }

  AfxEnableControlContainer();

  // Standard initialization
  // If you are not using these features and wish to reduce the size
  //  of your final executable, you should remove from the following
  //  the specific initialization routines you do not need.
//#ifdef _AFXDLL
//  Enable3dControls();             // Call this when using MFC in a shared DLL
//#else
//  Enable3dControlsStatic();       // Call this when linking to MFC statically
//#endif

  // Change the registry key under which our settings are stored.
  // TODO: You should modify this string to be something appropriate
  // such as the name of your company or organization.
  SetRegistryKey(_T("Kenwalt"));
  ScdPFUser.SetUseRegistry(true, HKEY_CURRENT_USER, "Kenwalt", m_pszProfileName);
  ScdPFMachine.SetUseRegistry(true, HKEY_LOCAL_MACHINE, "Kenwalt", m_pszProfileName);

  LoadStdProfileSettings();       // Load standard INI file options (including MRU)

  // Register the application's document templates.  Document templates
  //  serve as the connection between documents, frame windows and views.
  CSingleDocTemplate  *pDocTemplate;
  pDocTemplate = new CSingleDocTemplate
    (
      IDR_MAINFRAME,
      RUNTIME_CLASS(CSysCADMarshalDoc),
      RUNTIME_CLASS(CMainFrame),  // main SDI frame window
      RUNTIME_CLASS(CSysCADMarshalView)
    );
  AddDocTemplate(pDocTemplate);

  // Connect the COleTemplateServer to the document template.
  //  The COleTemplateServer creates new documents on behalf
  //  of requesting OLE containers by using information
  //  specified in the document template.
  m_server.ConnectTemplate(clsid, pDocTemplate, TRUE);

  // Note: SDI applications register server objects only if /Embedding
  //   or /Automation is present on the command line.
  // Enable DDE Execute open
  EnableShellOpen();
  RegisterShellFileTypes(TRUE);

  // Parse command line for standard shell commands, DDE, file open
  CCommandLineInfo  cmdInfo;
  ParseCommandLine(cmdInfo);

  _Module.UpdateRegistryFromResource(IDR_SYSCADMARSHAL, TRUE);
  _Module.RegisterServer(TRUE);

  InitVersionStuff();

  // Check to see if launched as OLE server
  // Always register class objects so that we connect to them
  if(1)//cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated)
    {
    // Register all OLE server (factories) as running.  This enables the
    //  OLE libraries to create objects from other applications.
    COleTemplateServer::RegisterAll();
    _Module.RegisterClassObjects(CLSCTX_LOCAL_SERVER, REGCLS_MULTIPLEUSE);

    // Application was run with /Embedding or /Automation.  Don't show the
    //  main window in this case.
    // CNM We do actually wnat to see the main window
    // return TRUE;
    }                   

  // When a server application is launched stand-alone, it is a good idea
  //  to update the system registry in case it has been damaged.
  m_server.UpdateRegistry(OAT_DISPATCH_OBJECT);
  COleObjectFactory::UpdateRegistryAll();

#if (0|| !defined(_RELEASE))      
  if(1)
    {
    char WDir[1024];
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
      }

    Strng DbgFile(LFiles);
    LFiles+="SysCADMarshal.dbg.txt";

    dbgfileopen(LFiles());//"c:\\SysCADMarshal.dbg.txt");
    dbgfileflush(1);
    }
#endif

  //// Force use of DAO3.6 if present
  //if (1)
  //  {
  //  AFX_MODULE_STATE * p=AfxGetModuleState();
  //  p->m_dwVersion = 0x0601; //this MUST be set before any DAO DLLs are loaded
  //  }

  // Dispatch commands specified on the command line
  if(!ProcessShellCommand(cmdInfo)) return FALSE;

  // The one and only window has been initialized, so show and update it.
  m_pMainWnd->ShowWindow(SW_SHOW);
  m_pMainWnd->UpdateWindow();

  // Enable drag/drop open
  m_pMainWnd->DragAcceptFiles();

//  SetTimer(NULL, 1, gc_DisplayUpdateTimer, NULL);
  return TRUE;
  }

//---------------------------------------------------------------------------

int CSysCADMarshalApp::ExitInstance(void)
  {
//  KillTimer(NULL, 1);
  gs_SlotMngr.StopThread();

  if(m_bATLInited)
    {
    _Module.RevokeClassObjects();
    _Module.Term();
    }

  return CWinApp::ExitInstance();
  }

/////////////////////////////////////////////////////////////////////////////

// CAboutDlg dialog used for App About
class CAboutDlg :
  public CDialog
  {
public:
  CAboutDlg(void);

  // Dialog Data
  //{{AFX_DATA(CAboutDlg)
  enum { IDD = IDD_ABOUTBOX };
  //}}AFX_DATA
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CAboutDlg)
protected:
  virtual void DoDataExchange(CDataExchange *pDX);  // DDX/DDV support
  virtual BOOL OnInitDialog();
  //}}AFX_VIRTUAL
// Implementation
protected:
  //{{AFX_MSG(CAboutDlg)
  // No message handlers
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
  };

CAboutDlg::CAboutDlg(void) :
  CDialog(CAboutDlg::IDD)
  {
  //{{AFX_DATA_INIT(CAboutDlg)
  //}}AFX_DATA_INIT
  }

void CAboutDlg::DoDataExchange(CDataExchange *pDX)
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

  // fill SysCAD version number etc
  Strng s;
  SetDlgItemText(IDC_SYSCAD_VERSION, FullVersion());
  SetDlgItemText(IDC_ACOPYRIGHT, CopyrightNotice());
  SetDlgItemText(IDC_ACOMPANY, FullCompany());
  s.Set("Build date: %s", BuildDate());
  SetDlgItemText(IDC_BUILDDATE, s());
  s.Set("Distributed by %s", FullCompany2());
  SetDlgItemText(IDC_ACOMPANY2, s());
  SetDlgItemText(IDC_WEBLOCATION, "www.SysCAD.net");
  /*if (strlen(SCD_PATCHNOTE)>0)
    {
    if (strlen(SCD_PATCHDATE)>0)
      s.Set("%s  (%s)", SCD_PATCHNOTE, SCD_PATCHDATE);
    else
      s.Set("%s", SCD_PATCHNOTE);
    }
  else*/
    s = "";
  SetDlgItemText(IDC_PATCH, s());
  return TRUE;  // return TRUE  unless you set the focus to a control
  }
// App command to run the dialog
void CSysCADMarshalApp::OnAppAbout(void)
  {
  CAboutDlg aboutDlg;
  aboutDlg.DoModal();
  }

//---------------------------------------------------------------------------

CSysCADMarshalDoc * TheDocument(CSysCADMarshalApp * This)
  {
  //  gs_SlotMngr.AddSlot((CSlotCfgStrings*)lParam);
  POSITION  Pos = This->GetFirstDocTemplatePosition();
  if(Pos)
    {
    CDocTemplate  *pDocTemp = This->GetNextDocTemplate(Pos);
    POSITION      DocPos = pDocTemp->GetFirstDocPosition();
    if(DocPos)
      {
      CSysCADMarshalDoc *pDoc = dynamic_cast<CSysCADMarshalDoc *>(pDocTemp->GetNextDoc(DocPos));
      ASSERT(pDoc);
      return pDoc;
      }
    }
  return NULL;
  }

//---------------------------------------------------------------------------

LRESULT CSysCADMarshalApp::OnAddError(WPARAM wParam, LPARAM lParam)
  {
  CSysCADMarshalDoc *pDoc = TheDocument(this);
  CErrorItem      *pError = (CErrorItem *) lParam;
  if (pDoc)
    {
    pDoc->m_ErrorList.AddTail(pError);
    pDoc->UpdateAllViews(NULL, MakeHint(HINT_ERROR_ADDED, pDoc->m_ErrorList.Count()-1));
    return true;
    }
  return false;
  }

//---------------------------------------------------------------------------

LRESULT CSysCADMarshalApp::OnOpenDoc(WPARAM wParam, LPARAM lParam)
  {
  CScdMarshal * pScdMarshal = (CScdMarshal*)wParam;

//  gs_SlotMngr.ConnectSimulator(pScdMarshal);

  CString *pS=(CString *)lParam;
  CString S=*pS;
  delete pS;
  if (!OpenDocumentFile(S))
    {
    m_OpenCloseSema.Unlock();
    return false;
    };    
  m_OpenCloseSema.Unlock();
  return true;
  }

//---------------------------------------------------------------------------

LRESULT CSysCADMarshalApp::OnCloseDoc(WPARAM wParam, LPARAM lParam)
  {
//  gs_SlotMngr.ConnectSimulator(NULL);
  OnFileNew();
  m_OpenCloseSema.Unlock();
  return true;
  }

//---------------------------------------------------------------------------

LRESULT CSysCADMarshalApp::OnSetSlotCfg(WPARAM wParam, LPARAM lParam)
  {
  CSysCADMarshalDoc *pDoc = TheDocument(this);
  if (pDoc)
    pDoc->SetSlotCfg((long)wParam, (CSlotCfgArray*)lParam);
  return true;
  }

//---------------------------------------------------------------------------

LRESULT CSysCADMarshalApp::OnSetLinkCfg(WPARAM wParam, LPARAM lParam)
  {
  CSysCADMarshalDoc *pDoc = TheDocument(this);
  if (pDoc)
    pDoc->SetLinkCfg((long)wParam, (CLinkCfgArray*)lParam);
  return true;
  }

//---------------------------------------------------------------------------

LRESULT CSysCADMarshalApp::OnSetCdBlkCfg(WPARAM wParam, LPARAM lParam)
  {
  CSysCADMarshalDoc *pDoc = TheDocument(this);
  if (pDoc)
    pDoc->SetCdBlkCfg((long)wParam, (CCdBlkCfgArray*)lParam);
  return true;
  }

//---------------------------------------------------------------------------

LRESULT CSysCADMarshalApp::OnSetDeviceCfg(WPARAM wParam, LPARAM lParam)
  {
  CSysCADMarshalDoc *pDoc = TheDocument(this);
  if (pDoc)
    pDoc->SetDeviceCfg((long)wParam, (CDeviceCfgArray*)lParam);
  return true;
  }

//---------------------------------------------------------------------------

LRESULT CSysCADMarshalApp::OnUpdateOneSlot(WPARAM wParam, LPARAM lParam)
  {
  CSysCADMarshalDoc *pDoc = TheDocument(this);
  if (pDoc)
    pDoc->UpdateOneSlot((long)wParam, (CSlotValue*)lParam);
  return true;
  }

//---------------------------------------------------------------------------

LRESULT CSysCADMarshalApp::OnUpdateOneLink(WPARAM wParam, LPARAM lParam)
  {
  CSysCADMarshalDoc *pDoc = TheDocument(this);
  if (pDoc)
    pDoc->UpdateOneLink((long)wParam, (CLinkValue*)lParam);
  return true;
  }

//---------------------------------------------------------------------------

LRESULT CSysCADMarshalApp::OnUpdateOneSlotState(WPARAM wParam, LPARAM lParam)
  {
  CSysCADMarshalDoc *pDoc = TheDocument(this);
  if (pDoc)
    pDoc->UpdateOneSlotState((long)wParam, lParam);
  return true;
  }

//---------------------------------------------------------------------------

LRESULT CSysCADMarshalApp::OnUpdateOneLinkState(WPARAM wParam, LPARAM lParam)
  {
  CSysCADMarshalDoc *pDoc = TheDocument(this);
  if (pDoc)
    pDoc->UpdateOneLinkState((long)wParam, lParam);
  return true;
  }

//---------------------------------------------------------------------------

LRESULT CSysCADMarshalApp::OnUpdateStatus(WPARAM wParam, LPARAM lParam)
  {
  CStatusInfoBlock *pSB=(CStatusInfoBlock *)lParam;

  CMainFrame* pMain=(CMainFrame*)AfxGetMainWnd();
  if (pMain)
    {
    CString S,SW;
    SW.Format(pSB->m_nIntWritesBusy||pSB->m_nFltWritesBusy  ? " -> i:%i,f:%i":"", pSB->m_nIntWritesBusy, pSB->m_nFltWritesBusy);
    S.Format("Slt:%i",    pSB->m_dwSlots);                      pMain->m_wndStatusBar.SetPaneText(1, S);  
    S.Format("Lnk:%i",    pSB->m_dwLinks);                      pMain->m_wndStatusBar.SetPaneText(2, S);  
    S.Format("Hld:%i",    pSB->m_dwHolds);                      pMain->m_wndStatusBar.SetPaneText(3, S);  
    S.Format("Que:%i/%i", pSB->m_dwDelays, pSB->m_dwChanges);   pMain->m_wndStatusBar.SetPaneText(4, S);  
    //S.Format("Tot:%i Slt:%i,%i Lnk:%i,%i OPC:%i,(i:%i,f:%i,x:%i)%s",    
    //  pSB->m_dwTotChgs,
    //S.Format("Slt:%i,%i Lnk:%i,%i OPC:%i,(i:%i,f:%i,x:%i)%s",    
    //  pSB->m_nScdSlotChgsIn,   pSB->m_nScdSlotChgsOut,
    //  pSB->m_nScdLinkChgsIn,   pSB->m_nScdLinkChgsOut,
    //  pSB->m_nDeviceChgsIn,    pSB->m_nDeviceChgsOutInt, pSB->m_nDeviceChgsOutFlt, pSB->m_nFltWritesSkip,
    //  SW
    //  );                            

    S.Format("OPC Rd:%i, Wr(i:%i, f:%i, s:%i)%s",    
      pSB->m_nDeviceChgsIn,   
      pSB->m_nDeviceChgsOutInt, pSB->m_nDeviceChgsOutFlt, pSB->m_nFltWritesSkip,
      SW
      );
    
    pMain->m_wndStatusBar.SetPaneText(5, S);  
    }

  delete pSB;
  return true;
  }

//---------------------------------------------------------------------------

LRESULT CSysCADMarshalApp::OnXferChangeBlock(WPARAM wParam, LPARAM lParam)
  {
  CSysCADMarshalDoc *pDoc = TheDocument(this);
  if (pDoc)
    pDoc->XferChangeBlock((long)wParam, (CChangeBlock*)lParam);
  return true;
  }

//---------------------------------------------------------------------------

//LRESULT CSysCADMarshalApp::OnSetSelection(WPARAM wParam, LPARAM lParam)
//  {
//  CSysCADMarshalDoc *pDoc = TheDocument(this);
//  CString*pStr=(CString*)lParam;
//  if (pDoc)
//    pDoc->FindString((long)wParam, *pStr, true);
//  delete pStr;
//  return true;
//  }

//---------------------------------------------------------------------------

CSysCADMarshalModule  _Module;

//---------------------------------------------------------------------------

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_ScdMarshal, CScdMarshal)
END_OBJECT_MAP()
LONG CSysCADMarshalModule::Unlock(void)
  {
  //AfxOleUnlockApp();
	AFX_MODULE_STATE* pModuleState = AfxGetModuleState();
	ASSERT(pModuleState->m_nObjectCount != 0);

  // allow application to shut down when all the objects have
  // been released
  const int NoSingletonObjectsInApp = 1; 

	if (InterlockedDecrement(&pModuleState->m_nObjectCount) == NoSingletonObjectsInApp)
	  {
    //::AfxOleOnReleaseAllObjects();
		
    // don't shut down the application if user is in control of the app
    if (AfxOleGetUserCtrl())
		  goto Done;

	  AfxOleSetUserCtrl(TRUE);    // avoid re-entry

	  // shut down the application
    AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_APP_EXIT, 0);
    AfxGetApp()->PostThreadMessage(WM_QUIT, 0, 0);
    }
Done:
	if (1)
    {
    AFX_MODULE_STATE* pModuleState = AfxGetModuleState();
	  TRACE1("CSysCADMarshalModule::Unlock  %i =================================================\n", pModuleState->m_nObjectCount);
    }
  return 0;
  }

LONG CSysCADMarshalModule::Lock(void)
  {
	if (1)
    {
    AFX_MODULE_STATE* pModuleState = AfxGetModuleState();
  	TRACE1("CSysCADMarshalModule::Lock    %i =================================================\n", pModuleState->m_nObjectCount);
    }
  AfxOleLockApp();
  return 1;
  }

BOOL CSysCADMarshalApp::InitATL(void)
  {
  m_bATLInited = TRUE;

  _Module.Init(ObjectMap, AfxGetInstanceHandle());
  _Module.dwThreadID = GetCurrentThreadId();

  return TRUE;
  }

int CSysCADMarshalApp::Run(void)
  {
  // TODO: Add your specialized code here and/or call the base class
  return CWinApp::Run();
  }

