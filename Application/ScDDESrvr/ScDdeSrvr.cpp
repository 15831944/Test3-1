//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#include "scdver.h"
//#include <afxdllx.h>
#include "CommsSrvrBase.h"
#include "dde_mngr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#pragma LIBCOMMENT("..\\..\\Common\\com\\ScdIF\\", "\\ScdIF" )
#pragma LIBCOMMENT("..\\..\\Common\\scd\\ScdLib\\", "\\ScdLib" )
#pragma LIBCOMMENT("..\\..\\Common\\scd\\ScExec\\", "\\ScExec" )

AFX_EXTENSION_MODULE ScDdeSrvrDLL = { NULL, NULL };

// --------------------------------------------------------------------------

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
  {
	if (dwReason == DLL_PROCESS_ATTACH)
    {
		TRACE1("SCDDESRVR.DLL Initializing %08x!\n", hInstance);
		
		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(ScDdeSrvrDLL, hInstance))
      return 0;

		// Insert this DLL into the resource chain
		new CDynLinkLibrary(ScDdeSrvrDLL);

		if (!MakeVersionOK("SCDDESRVR.DLL", _MAKENAME, SCD_VERINFO_V0, SCD_VERINFO_V1, SCD_VERINFO_V2, SCD_VERINFO_V3))
      return 0;
    }
	else if (dwReason == DLL_PROCESS_DETACH)
    {
		TRACE0("SCDDESRVR.DLL Terminating!\n");
		AfxTermExtensionModule(ScDdeSrvrDLL);
    }
	return 1;   // ok
  }

// --------------------------------------------------------------------------

#define DLLEXPORT __declspec( dllexport )

extern "C" DLLEXPORT CCommsServerManagerBase* /*FAR PASCAL*/ OpenDDESrvr(void)
  {//DDE Server
  //if (CScdDDEManager::bInitOK)
  //  CScdDDEManager::Stop(); //for safety
  return CScdDDEManager::Init();
  }

// --------------------------------------------------------------------------

extern "C" DLLEXPORT bool /*FAR PASCAL*/ CloseDDESrvr(void)
  {//DDE Server
  //if (CScdDDEManager::bInitOK)
    {
    if (!CScdDDEManager::Stop())
      return false;
    }
  return true;
  }

//===========================================================================
