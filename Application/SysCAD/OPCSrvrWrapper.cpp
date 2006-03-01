#include "stdafx.h"
#include ".\opcsrvrwrapper.h"
//
//#include "stdafx.h"
//#define __PROJECT_CPP
#include "sc_defs.h"
//#include "resource.h"
//#include "syscad.h"
//#include "project.h"
//#include "mainfrm.h"
//#include "tagdb.h"
#include "drv_mngr.h"
//#include "ArchMngr.h"
//#if WITHNETSERVER
//#include "cs_mngr.h"
//#endif
//#include "..\schist\hstmain.h"
//#include "accnode.h"
//#include "msgwnd.h"
//#include "ordwnd.h"
//#include "statswnd.h"
//#include "grfdoc.h"
//#include "tagvdoc.h"
//#include "prjdoc.h"
//#include "scdver.h"
//#include "dlgbusy.h"
//#include "prjdlgs.h"
//#include "prjview.h"
//#include "chngtag.h"
//#include "tknparse.h"
//#include "toolbars.h"
//#include "tagvtext.h"
//#include "tagvtrnd.h"
//#include "tagvsplt.h"
//#include "cmd_mngr.h"
//#include "licbase.h"
//#include "scd_wm.h"
//#include "slvtool.h"
//#include "io.h"
//#include "mdlvalue.h"
//#include "zipstuff.h"
//#include "dbhelper.h"
////#include "visgrfdoc.h"
////#include "visgrffrm.h"
////#include "autodoc.h"
////#include "autofrm.h"
//#include "liccount.h"
//#include "copyblk.h"
//#include "wndslct.h"
//#include "revhist.h"
//#include "scdcmdif.h"
//#include "marshal.h"
//#include "FileImportSelect.h"
//#include "ElectricalImport.h"
//#include "flwsolve.h"
//#include "srvmngr.h"

//===========================================================================
//
//
//
//===========================================================================

COPCSrvrWrapper* gs_pMainOPCSrvr = NULL;

bool OpenMainOPCSrvr(LPCTSTR EXEName, bool bResetReg, int iServer)
  {
  if (gs_pMainOPCSrvr)
    CloseMainOPCSrvr();
  gs_pMainOPCSrvr = new COPCSrvrWrapper;
  return gs_pMainOPCSrvr->OpenSrvr(EXEName, bResetReg, iServer);//bOPCResetReg, iOPCServerNo))
  };

//---------------------------------------------------------------------------

void CloseMainOPCSrvr()
  {
  if (gs_pMainOPCSrvr)
    {
    gs_pMainOPCSrvr->CloseSrvr();
    delete gs_pMainOPCSrvr;
    gs_pMainOPCSrvr = NULL;
    };
  };

bool GetMainOPCSrvrEnable()
  {
  if (gs_pMainOPCSrvr)
    return gs_pMainOPCSrvr->OPCSrvrMngr()->Enabled();
  return false;
  };

bool EnableMainOPCSrvr(bool On)
  {
  if (gs_pMainOPCSrvr)
    gs_pMainOPCSrvr->OPCSrvrMngr()->SetEnable(On);
  return false;
  }

void DisableMainOPCSrvr()
  {
  if (gs_pMainOPCSrvr)
    gs_pMainOPCSrvr->OPCSrvrMngr()->SetEnable(false);
  }

void LoadMainOPCOptions(CProfINIFile & PF)
  {
  if (gs_pMainOPCSrvr && gs_pMainOPCSrvr->OPCSrvrMngr())
    gs_pMainOPCSrvr->OPCSrvrMngr()->LoadOrSaveOptions(PF, true);
  }

void SaveMainOPCOptions(CProfINIFile & PF)
  {
  if (gs_pMainOPCSrvr && gs_pMainOPCSrvr->OPCSrvrMngr())
    gs_pMainOPCSrvr->OPCSrvrMngr()->LoadOrSaveOptions(PF, false);
  }



//===========================================================================
//
//
//
//===========================================================================

COPCSrvrWrapper::COPCSrvrWrapper()
  {
  hOPCLib = NULL;
  iOPCUseCnt = 0;
  pMngr = NULL;
  }

//---------------------------------------------------------------------------

COPCSrvrWrapper::~COPCSrvrWrapper()
  {
  ASSERT(iOPCUseCnt==0 && hOPCLib==NULL && pMngr==NULL); //this should have been properly closed!
  }

//---------------------------------------------------------------------------

void COPCSrvrWrapper::RegisterOPCServer()
  {
  if (gs_pMainOPCSrvr)
    CloseMainOPCSrvr();

  OpenMainOPCSrvr(ExeFile(), 1, 0);//bOPCResetReg, iOPCServerNo))
  CloseMainOPCSrvr();
  }

//---------------------------------------------------------------------------
typedef CCommsServerManagerBase* (*fn_OPCSvrDrvOpen)(LPCTSTR EXEName, bool ResetReg, int Server);
typedef bool (*fn_OPCSvrDrvClose)(void);

bool COPCSrvrWrapper::OpenSrvr(LPCTSTR EXEName, bool bResetReg, int iServer)
  {
  ASSERT(pMngr==NULL);
  CWaitCursor Wait;

  bool DoLoad = (hOPCLib==NULL);

  CString Path=ProgFiles();
  Path+=OPC_DLL_Name;
  HINSTANCE hLib = hOPCLib;
  if (hLib==NULL)
    {
    hLib = AfxLoadLibrary(Path);
    }
  if (hLib)
    {
    fn_OPCSvrDrvOpen f1 = (fn_OPCSvrDrvOpen)GetProcAddress(hLib, "OpenOPC");
    fn_OPCSvrDrvClose f2 = (fn_OPCSvrDrvClose)GetProcAddress(hLib, "CloseOPC");
    if (f1==NULL || f2==NULL)
      {
      LogError("OPC Server", LF_Exclamation, "Incorrect %s", OPC_DLL_Name);
      if (DoLoad)
        AfxFreeLibrary(hLib);
      return false; //DLL existed, but was not the expected DLL
      }
    pMngr = f1(EXEName, bResetReg, iServer);
    if (bResetReg)
      {
      hOPCLib = hLib;
      iOPCUseCnt++;
      //ASSERT(pMngr==NULL);
      //if (DoLoad)
      //  AfxFreeLibrary(hLib);
      return true;
      }
    if (pMngr==NULL)
      {
      LogError("OPC Server", LF_Exclamation, "Error Opening SysCAD as OPC Server");
      if (DoLoad)
        AfxFreeLibrary(hLib);
      return false;
      }
    //all is well in the land of SysCAD OPC Server...
    hOPCLib = hLib;
    iOPCUseCnt++;
    }
  else
    {

    LPVOID lpMsgBuf;
    FormatMessage(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
      NULL,
      GetLastError(),
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
      (LPTSTR) &lpMsgBuf,    0,    NULL );// Display the string.
    CString S=(char*)lpMsgBuf;
    // Free the buffer.
    LocalFree( lpMsgBuf );

    LogError("OPC Server", LF_Exclamation, "Unable to load %s or related DLL (ScdOPCSrv.dll) : %s", OPC_DLL_Name, (LPCTSTR)S);
    return false;
    }
  return (pMngr!=NULL);
  }

//---------------------------------------------------------------------------

void COPCSrvrWrapper::CloseSrvr()
  {
  if (hOPCLib)
    {
    CWaitCursor Wait;
    //CDlgBusy::Open("\n\nClosing OPC Server");
    fn_OPCSvrDrvClose f = (fn_OPCSvrDrvClose)GetProcAddress(hOPCLib, "CloseOPC");
    bool b = f();
    if (b)
      pMngr = NULL;
    else
      LogError("OPC Server", LF_Exclamation, "Error Closing SysCAD OPC Server");
    iOPCUseCnt--;
    if (iOPCUseCnt==0)
      {
      BOOL b = AfxFreeLibrary(hOPCLib);
      hOPCLib = NULL;
      }
    }
  }

//---------------------------------------------------------------------------

void COPCSrvrWrapper::Options()
  {
  if (pMngr)
    pMngr->OpenStatusWnd();
  }

//---------------------------------------------------------------------------

void COPCSrvrWrapper::CloseOptions()
  {
  if (pMngr)
    pMngr->CloseStatusWnd();
  }

