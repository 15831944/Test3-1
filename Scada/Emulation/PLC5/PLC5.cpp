// PLC5.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "resource.h"
#include <initguid.h>
#include "PLC5.h"
#include "PLC5Dlg.h"
#include "Dlgs.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//*********************************************************
// FSServer stuff
//#include ".\OpcSrvr\Callback.h"   // FSServer callback class


/////////////////////////////////////////////////////////////////////////////

CPLC5Dlg theDlg;//(&m_Plc, &m_Xfer, pOPCCallback);//pOPCCallBack);

/////////////////////////////////////////////////////////////////////////////
// CPLC5App

BEGIN_MESSAGE_MAP(CPLC5App, CWinApp)
	//{{AFX_MSG_MAP(CPLC5App)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
//  ON_MESSAGE( WM_OPCCMD, OnOpcCmd)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPLC5App construction

CPLC5App::CPLC5App()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
  m_UpNAbout=FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CPLC5App object



CPLC5App theApp;
//CPLC5Module _Module;
//
//BEGIN_OBJECT_MAP(ObjectMap)
//OBJECT_ENTRY(CLSID_SysCADCmd, CSysCADCmd)
//END_OBJECT_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPLC5App initialization

BOOL CPLC5App::InitInstance()
{

  OleInitialize(NULL);

  //if( !StartFSServer(m_hInstance, &CLSID_OPCServer) )
  //  return FALSE;

  // COM Command Register
	m_ScdCmdIF.Register( WM_COMEVT);

//#if _WIN32_WINNT >= 0x0400 & defined(_ATL_FREE_THREADED)
//    HRESULT hRes = CoInitializeEx(NULL, COINIT_MULTITHREADED);
//#else
//    HRESULT hRes = CoInitialize(NULL);
//#endif
//    _ASSERTE(SUCCEEDED(hRes));
//  _Module.Init(ObjectMap, m_hInstance, &LIBID_PLC5Lib);
  //_Module.dwThreadID = GetCurrentThreadId();
//    TCHAR szTokens[] = _T("-/");


  bool Embedding=false;
  bool RegoDone=false;
  TCHAR szTokens[] = _T("-/ ");
  // copy m_lpCmdLine because _tcstok modifies the string

  CString tempCmdLine(m_lpCmdLine);
  LPTSTR lpszToken = _tcstok(tempCmdLine.GetBuffer(1), szTokens);
  while (lpszToken != NULL)
    {
    if (_tcsicmp(lpszToken, _T("UnregServer"))==0)
      {
      // SysCADCmd
      //_Module.UpdateRegistryFromResource(IDR_PLC5, FALSE);
      //if ( FAILED(_Module.UnregisterServer(TRUE)) )
      //  AfxMessageBox(_T("SysCADCmd UnregisterServer Failed"));

      // OPC
      if (theDlg.OPCStart())
        if (!theDlg.OPCUnregister())
          AfxMessageBox(_T("OPC UnregisterServer Failed"));
      theDlg.OPCStop();
      RegoDone=true;
      }
    else if (_tcsicmp(lpszToken, _T("RegServer"))==0)
      {
      // SysCADCmd
      HRESULT hr=m_ScdCmdIF.UpdateRegistry(m_hInstance, TRUE);//, OLESTR("AAA"), OLESTR("TTT"));
      if( FAILED(hr) )
         AfxMessageBox(_T("SysCADCmd RegisterServer Failed"));
      // OPC
      if (theDlg.OPCStart())
        if (!theDlg.OPCRegister())
          AfxMessageBox(_T("OPC RegisterServer Failed"));
      theDlg.OPCStop();
      RegoDone=true;
      }
    else if (_tcsicmp(lpszToken, _T("Embedding"))==0)
      {
      Embedding=true;
      }
    lpszToken = _tcstok(NULL, szTokens);
    }

//  StopFSServer();
  
  if (RegoDone)
    {
    Sleep(100);//_Module.Term();
    m_ScdCmdIF.UnRegister();

    OleUninitialize();
    return FALSE;
    }

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
//	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

#ifdef _DEBUG

  long BreakAlloc = -1;           // Use Debugger to set this
  _CrtSetBreakAlloc(BreakAlloc);  //this sets _crtBreakAlloc

#endif
  char ModuleFN[_MAX_PATH];
  GetModuleFileName(NULL, ModuleFN, sizeof(ModuleFN));
  char cDrv[_MAX_DRIVE];
  char cPath[_MAX_PATH];
  char cFile[_MAX_FNAME];
  char cExt[_MAX_EXT];
  _splitpath(ModuleFN, cDrv, cPath, cFile, cExt);
  CString sFNRoot=cDrv;
  CString sFNPrj;
  _strlwr(cPath);
  char*pBin=strstr(cPath, "\\bin\\");
  if (pBin)
    {
    *pBin=0; // remove \Bin\ etc
    sFNRoot+=cPath;
    sFNRoot+="\\MosRef\\";
    }
  else
    sFNRoot+="\\SysCAD82\\MosRef\\";

  sFNRoot+="EsdPlc\\";
  sFNRoot = GetProfileString("General", "ProjectFolder", (const char*)sFNRoot);
  sFNPrj = GetProfileString("General", "ProjectFile", (const char*)sFNPrj);

  bool DbgOn = (GetProfileInt("Debug", "On", 1)!=0);
  bool GotPrj=0;

  if (Embedding)
    {
    //_splitpath(sFNRoot, cDrv, cPath, NULL, NULL);
    //sFNRoot=cDrv;
    //sFNRoot+=cPath;
    //_splitpath(sFNPrj, NULL, NULL, cFile, cExt);
    //sFNPrj=cFile;
    //sFNPrj+=cExt;
    }
  else
    {
    //not started remotely, ask user to select project folder...
    COpenDlg Dlg;
    Dlg.m_PrjPath = sFNRoot;
    Dlg.m_PrjPath += sFNPrj;
    Dlg.m_DbgOn = DbgOn;
    if (Dlg.DoModal()==IDOK)
      {
      _splitpath(Dlg.m_PrjPath, cDrv, cPath, cFile, cExt);
      sFNRoot=cDrv;
      sFNRoot+=cPath;
      sFNPrj=cFile;
      sFNPrj+=cExt;
      DbgOn = (Dlg.m_DbgOn!=0);
      HANDLE H;
      WIN32_FIND_DATA fd;
      CString Fn;
      Fn=sFNRoot+sFNPrj;
      if ((H=FindFirstFile(Fn, &fd))!=INVALID_HANDLE_VALUE)
        {
        GotPrj=true;
        FindClose(H);
        }
      else if (Fn.GetLength()>0)
        {
        CString Msg;
        Msg.Format("File not found: %s", (LPCTSTR)Fn);
        MessageBox(NULL, Msg, "PLC5", MB_OK);
        }
      }
    }

  //assume folder exists!
  //if (sFNRoot[sFNRoot.GetLength()-1]!='\\')
  //  sFNRoot += '\\';
  WriteProfileString("General", "ProjectFolder", (const char*)sFNRoot);
  WriteProfileString("General", "ProjectFile", (const char*)sFNPrj);
  WriteProfileInt("Debug", "On", DbgOn);

  if (DbgOn)
    {
    char Temp[_MAX_PATH];
    if (GetEnvironmentVariable("TEMP", Temp, sizeof(Temp)))
      {
      char t=Temp[strlen(Temp)-1];
      if (t!='/' && t!='\\')
        strcat(Temp, "\\");
      strcat(Temp, "SysCAD");
      ::CreateDirectory(Temp, NULL);
      strcat(Temp, "\\PLC5_dbg.txt");
      dbgf=fopen(Temp, "wt");
      }                                           
    }

  if (GotPrj)
    {
    theDlg.m_sInitFile=sFNRoot;
    theDlg.m_sInitFile+=sFNPrj;

    //theDlg.Open(sFNRoot, sFNPrj);
    }


//  m_Plc.InitTree(m_Tree);

  m_UpNAbout=true;
  
	//CPLC5Dlg theDlg;//(&m_Plc, &m_Xfer, pOPCCallback);//pOPCCallBack);
	m_pMainWnd = &theDlg;
	int nResponse = theDlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

  theDlg.OPCStop();
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

//=========================================================================

int CPLC5App::ExitInstance()
  {
	// TODO: Add your specialized code here and/or call the base class

  m_ScdCmdIF.UnRegister();
  //StopFSServer();
  OleUninitialize();

	return CWinApp::ExitInstance();
  }

//=========================================================================

void CTransferBlk::DoTransfer()
  {
  if (m_bOK)
    {
    if (m_pSrc->IsFlt())
      {
      for (int i=0; i<m_nLen; i++)
        m_pDst->SetFValue(i, m_pSrc->FValue(i));
      }
    else if (!m_pSrc->IsBit())
      {
      for (int i=0; i<m_nLen; i++)
        m_pDst->SetIValue(i, m_pSrc->IValue(i));
      }
    else
      {
      INCOMPLETECODE();
      }
    }
  };

//=========================================================================

void CForceBlk::DoTransfer()
  {
  if (m_bOK)
    {
    if (m_pDst->IsFlt())
      {
      for (int i=0; i<m_nLen; i++)
        m_pDst->SetFValue(i, m_Values[i]);
      }
    else if (!m_pDst->IsBit())
      {
      for (int i=0; i<m_nLen; i++)
        m_pDst->SetIValue(i, short(m_Values[i]));
      }
    else
      {
      INCOMPLETECODE();
      }
    }
  };

//=========================================================================
//
//
//
//=========================================================================
