//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#include "stdafx.h"
#include <afxdllx.h>
#include "sc_defs.h"
#include "execlib.h"
#include "drv_base.h"
#define __OPC_CODE
#include "scOPCsrv.h"
#include "errorlog.h"
#include "dbgmngr.h"
#include "resource.h"
#include "opcdlgs.h"
#include "executiv.h"

#include ".\\OpcSrvr\\srvCallback.h"   // FSServer callback class
#include ".\\OpcSrvr\\SrvMngr.h"

#pragma LIBCOMMENT("..\\..\\com\\ScdIF\\", "\\ScdIF" )
#pragma LIBCOMMENT("..\\..\\scd\\scdlib\\", "\\scdlib")
#pragma LIBCOMMENT("..\\..\\scd\\scexec\\", "\\scexec")
#pragma LIBCOMMENT("..\\scdopc\\serverdll\\", "\\scdopcsrv")

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//#include "optoff.h"

//**************************************************************************

const CLSID CLSID_OPCServerList = {0x13486D51,0x4821,0x11D2,{0xA4,0x94,0x3C,0xB3,0x06,0xC1,0x00,0x00}};
#define __GUID_DEFINED__
#include "OPCDa_Cats.c"

// --------------------------------------------------------------------------
//
//
//
// --------------------------------------------------------------------------

int ChkErrorOK(int err, flag WithMsg=true)
  {
//  if (err && err!=NADS_PARTIAL_FUNC_FAIL)
//    {
//    if (WithMsg)
//      {
//      char texterr[256];
//      myhsc_napierrstr(err, texterr);
//      LogError("OPC", 0, "%s", texterr);
//      }
//    return 0;
//    }
  return 1;
  }

//===========================================================================

static AFX_EXTENSION_MODULE OPC_DLL = { NULL, NULL };

//------------------------------------------------------------------------

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
  {
  if (dwReason == DLL_PROCESS_ATTACH)
    {
    TRACE0("scOPCsvr.DLL Initializing!\n");
    // Extension DLL one-time initialization
    AfxInitExtensionModule(OPC_DLL, hInstance);
    // Insert this DLL into the resource chain
    new CDynLinkLibrary(OPC_DLL);
    }
  else if (dwReason == DLL_PROCESS_DETACH)
    {
    TRACE0("scOPCsvr.DLL Terminating!\n");
    }
  return 1;   // ok
  }

//===========================================================================

static double TimeFromStart()
  {
  static DWORD S=0;
  if (S==0) 
    S=GetTickCount();
  return 0.001*(GetTickCount()-S);
  }

//===========================================================================

#define DLLEXPORT __declspec( dllexport )

// --------------------------------------------------------------------------

extern "C" DLLEXPORT CCommsServerManagerBase* /*FAR PASCAL*/ OpenOPC(LPCTSTR EXEName, bool ResetReg,int Server)
  {//OPC Server
  if (CScdOPCManager::sm_bInitOK)
    CScdOPCManager::Stop(); //for safety
  if (!CScdOPCManager::Init(EXEName, ResetReg,Server))
    return NULL;
  return CScdOPCManager::sm_pTheMngr;
  }

// --------------------------------------------------------------------------

extern "C" DLLEXPORT bool /*FAR PASCAL*/ CloseOPC(void)
  {//OPC Server
  if (CScdOPCManager::sm_bInitOK)
    {
    if (!CScdOPCManager::Stop())
      return false;
    }
  return true;
  }

//===========================================================================

