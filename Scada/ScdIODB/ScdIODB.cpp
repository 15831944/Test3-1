//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  ScdIODB.cpp
//  $Author    :  Jim Hansen
//
//  Description:  MFC application object, global definitions, OPC thread
//
//
//**************************************************************************

#include "stdafx.h"
#include "ScdIODB.h"
#include "MainFrm.h"
#include "TreeView.h"
#include "Splash.h"
//#include <atlimpl.cpp>
//#include <statreg.cpp>

// Added Ver. 2.0
#include "cathelp.h"
#define __GUID_DEFINED__
#include "opcda_cats.c"

#include "initguid.h"

// define IIDs
#define IID_DEFINED
#include "OPCDa_i.c"
#include "OPCComn_i.c"

// Define OPC globals for this application
// \/ \/ \/ Toolkit Modify Here \/ \/ \/
// {60401B2F-56C6-409f-9E86-9DAFEAE97251} // this application's CLSID
CLSID CLSID_OPCServer =
{ 0x60401b2f, 0x56c6, 0x409f, { 0x9e, 0x86, 0x9d, 0xaf, 0xea, 0xe9, 0x72, 0x51 } };
// /\ /\ /\ Toolkit Modify Here /\ /\ /

// define OPC data stream formats
UINT OPCSTMFORMATDATA = RegisterClipboardFormat(_T("OPCSTMFORMATDATA"));
UINT OPCSTMFORMATDATATIME = RegisterClipboardFormat(_T("OPCSTMFORMATDATATIME"));
UINT OPCSTMFORMATWRITECOMPLETE = RegisterClipboardFormat(_T("OPCSTMFORMATWRITECOMPLETE"));

// ATL global class factory, etc
CAtlGlobalModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
   OBJECT_ENTRY(CLSID_OPCServer, OPCServer)
END_OBJECT_MAP()
// End of OPC globals

extern CRITICAL_SECTION g_cs;  // Added 2.02
extern ServerList servers;
extern CDWordArray LCIDs;      // Added 2.02
FILETIME     serverStartTime;
CRITICAL_SECTION uiCS;  // protect between UI and other threads changing
                        // device parameters, current device, etc.
CRITICAL_SECTION tagCS; // protect access to tag values (data) with this

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************
DWORD OPCMainThread(DWORD hInstance)
{
   HRESULT hRes = CoInitializeEx(NULL, COINIT_MULTITHREADED);
   if( FAILED( hRes ) )
      return hRes;

   _Module.RegisterClassObjects(CLSCTX_LOCAL_SERVER|CLSCTX_REMOTE_SERVER, REGCLS_MULTIPLEUSE);

   // wait until we get a WM_QUIT
   MSG msg;
   while (GetMessage(&msg, 0, 0, 0))
      DispatchMessage(&msg);
   _Module.RevokeClassObjects(); // Revoke class factories for ATL

   // Notify clients that the server is exiting
   {  // protect this loop
      CSLock wait( &g_cs );
      POSITION pos = servers.GetHeadPosition();
      while( pos )
      {
         OPCServerBase* pServer = servers.GetNext( pos );
         pServer->ServerShutdown( _T("Exiting Server"));
      }
   }

   // Let clients disconnect
   Sleep( 1000 );

   // disconnect remaining clients
   {  // protect this loop
      CSLock wait( &g_cs );
      POSITION pos = servers.GetHeadPosition();
      while( pos )
      {
         OPCServerBase* pServer = servers.GetNext( pos );
         IUnknown* pUnk=0;
         HRESULT hr = ((CComObject<OPCServer>*)pServer)->QueryInterface( IID_IUnknown, (LPVOID*)&pUnk );
         if( SUCCEEDED(hr) )
            CoDisconnectObject( pUnk, 0 );  // disconnect from any remaining clients
      }
   }

   CoUninitialize();
   Sleep( 500 );

   // Delete any remaining servers
   {  // protect this loop
      CSLock wait( &g_cs );
      POSITION pos = servers.GetHeadPosition();
      while( pos )
      {
         OPCServerBase* pServer = servers.GetNext( pos );
         delete pServer;
      }
   }

   return S_OK;
}

//**************************************************************************
// find all languages in the resources
BOOL CALLBACK EnumResLangProc(
                    HMODULE hModule, // resource-module handle 
                    LPCTSTR lpszType, // address of resource type 
                    LPCTSTR lpszName, // address of resource name 
                    LANGID wIDLanguage, // resource language identifier
                    LPARAM lParam) // application-defined 
{
    LCIDs.Add(wIDLanguage);
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CScdTgSrcApp

BEGIN_MESSAGE_MAP(CScdTgSrcApp, CWinApp)
   //{{AFX_MSG_MAP(CScdTgSrcApp)
   ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
      // NOTE - the ClassWizard will add and remove mapping macros here.
      //    DO NOT EDIT what you see in these blocks of generated code!
   //}}AFX_MSG_MAP
   // Standard file based document commands
   ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
   ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScdTgSrcApp construction

CScdTgSrcApp::CScdTgSrcApp()
{
   m_bAutoClose = FALSE;
   hOPCThread = 0;
   OPCThreadId = 0;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CScdTgSrcApp object

CScdTgSrcApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CScdTgSrcApp initialization

BOOL CScdTgSrcApp::InitInstance()
{
   if(!AfxOleInit())
      {
      AfxMessageBox(_T("Ole Initialization failed"));
      return FALSE;
      }
   InitializeCriticalSection( &uiCS );
   InitializeCriticalSection( &tagCS );
   _Module.Init(ObjectMap, m_hInstance);

   // Modify registry based on the command line
   // If this happens, ExitInstance is not called,
   // so don't initialize much before this
   TCHAR szTokens[] = _T("-/ ");
   // copy m_lpCmdLine because _tcstok modifies the string
   CString tempCmdLine(m_lpCmdLine);
   LPTSTR lpszToken = _tcstok(tempCmdLine.GetBuffer(1), szTokens);
   while (lpszToken != NULL)
   {
      if (_tcsicmp(lpszToken, _T("UnregServer"))==0)
      {
         _Module.UnregisterServer();
         UnRegisterCLSIDInCategory( CLSID_OPCServer, CATID_OPCDAServer10 );
         UnRegisterCLSIDInCategory( CLSID_OPCServer, CATID_OPCDAServer20 );
         return FALSE;
      }
      else if (_tcsicmp(lpszToken, _T("RegServer"))==0)
      {
         HRESULT hr = _Module.RegisterServer(FALSE);
         if( SUCCEEDED(hr) )
         {  // Register this server in both OPC DA 1.0 and 2.0 categories
            hr = CreateComponentCategory( CATID_OPCDAServer10, L"OPC Data Access Servers Version 1.0");
            if( SUCCEEDED(hr) )
               hr = RegisterCLSIDInCategory( CLSID_OPCServer, CATID_OPCDAServer10 );
            hr = CreateComponentCategory( CATID_OPCDAServer20, L"OPC Data Access Servers Version 2.0");
            if( SUCCEEDED(hr) )
               hr = RegisterCLSIDInCategory( CLSID_OPCServer, CATID_OPCDAServer20 );
         }

         return FALSE;
      }
      lpszToken = _tcstok(NULL, szTokens);
   }

   CoFileTimeNow( &serverStartTime);

    UINT idRsrcBlk = IDS_OPC_E_INVALIDHANDLE / 16 + 1;
    
    // Fill the list of supported languages for an error message
    EnumResourceLanguages(m_hInstance,  // Look in this module.
                          RT_STRING, // given resource type
                          MAKEINTRESOURCE(idRsrcBlk), // given resource name
                          (ENUMRESLANGPROC)EnumResLangProc, // callback function
                          0); // application-defined 


   {
      CCommandLineInfo cmdInfo;
      ParseCommandLine(cmdInfo);

      //CSplashWnd::EnableSplashScreen(cmdInfo.m_bShowSplash);
   }

   // \/ \/ \/ Toolkit Modify Here \/ \/ \/
   SetRegistryKey( _T("ScdIODB.OPC") );
   // /\ /\ /\ Toolkit Modify Here /\ /\ /

   LoadStdProfileSettings();  // Load standard INI file options (including MRU)

   // Register the application's document templates.  Document templates
   //  serve as the connection between documents, frame windows and views.

   CSingleDocTemplate* pDocTemplate;
   pDocTemplate = new CSingleDocTemplate(
      IDR_MAINFRAME,
      RUNTIME_CLASS(CDoc),
      RUNTIME_CLASS(CMainFrame),       // main SDI frame window
      RUNTIME_CLASS(CDeviceView));
   AddDocTemplate(pDocTemplate);

   // Parse command line for standard shell commands, DDE, file open
   CCommandLineInfo cmdInfo;
   ParseCommandLine(cmdInfo);

   if (cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated)
   {
      m_bAutoClose = TRUE;
   }

   if( cmdInfo.m_strFileName.IsEmpty() )
   {
      // Automatically recall the last configuration
      CString file( GetProfileString( _T("Recent File List"), _T("File1") ) );
      if( !file.IsEmpty() )
         {
         CFileStatus status;
         if( CFile::GetStatus( file, status ) )
            {
            cmdInfo.m_nShellCommand = CCommandLineInfo::FileOpen;
            cmdInfo.m_strFileName = file;
            }
         }
   }

   // Dispatch commands specified on the command line
   if (!ProcessShellCommand(cmdInfo))
      return FALSE;

   SECURITY_ATTRIBUTES sa;
   sa.nLength = sizeof(sa);
   sa.lpSecurityDescriptor = NULL;
   hOPCThread = ::CreateThread( &sa, 0,
      (LPTHREAD_START_ROUTINE)&OPCMainThread,
                              this,
                              0,
                              &OPCThreadId);

   // Check to see if launched as OLE server
   {
      // Register all OLE server (factories) as running.  This enables the
      //  OLE libraries to create objects from other applications.
//    COleTemplateServer::RegisterAll();

      // Application was run with /Embedding or /Automation.  Don't show the
      //  main window in this case.
//    return TRUE;
   }

   if (cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated)
   {
      m_pMainWnd->ShowWindow( SW_SHOWMINIMIZED );
   }

   return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void  CScdTgSrcApp::ForceExit()
{
   CWaitCursor wait;
   // Quit the OPC thread
   if( hOPCThread )
   {
      ::PostThreadMessage(OPCThreadId, WM_QUIT, 0, 0);
      if( WAIT_FAILED == WaitForSingleObject( hOPCThread, 5000) )
      {
         TRACE_MSG;
      }
      CloseHandle( hOPCThread );
      hOPCThread = NULL;
   }
}

/////////////////////////////////////////////////////////////////////////////
int CScdTgSrcApp::ExitInstance()
{
   if( hOPCThread )
   {
      ::PostThreadMessage(OPCThreadId, WM_QUIT, 0, 0);
      if( WAIT_FAILED == WaitForSingleObject( hOPCThread, 5000) )
      {
         TRACE_MSG;
      }
      CloseHandle( hOPCThread );
      hOPCThread = NULL;
   }

   _Module.Term();
   DeleteCriticalSection( &tagCS );
   DeleteCriticalSection( &uiCS );
   return CWinApp::ExitInstance();
}

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
      // No message handlers
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

// App command to run the dialog
void CScdTgSrcApp::OnAppAbout()
{
   CAboutDlg aboutDlg;
   aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CScdTgSrcApp commands

BOOL CScdTgSrcApp::PreTranslateMessage(MSG* pMsg)
{
   //if (CSplashWnd::PreTranslateAppMessage(pMsg))
   //   return TRUE;

   return CWinApp::PreTranslateMessage(pMsg);
}
