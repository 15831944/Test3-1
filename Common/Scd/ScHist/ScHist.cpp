//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#if ForSysCAD
#include "sc_defs.h"
#include "scdver.h"
#include <afxdllx.h>
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#if ForSysCAD
#pragma LIBCOMMENT("..\\..\\com\\ScdIF\\", "\\ScdIF" )
#pragma LIBCOMMENT("..\\..\\scd\\ScdLib\\", "\\ScdLib" )
#pragma LIBCOMMENT("..\\..\\scd\\ScExec\\", "\\ScExec" )
#endif

AFX_EXTENSION_MODULE SCHistDLL = { NULL, NULL };

// --------------------------------------------------------------------------

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
  {
	if (dwReason == DLL_PROCESS_ATTACH)
    {
		TRACE1("SCHIST.DLL Initializing %08x!\n", hInstance);
		
		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(SCHistDLL, hInstance))
      return 0;

		// Insert this DLL into the resource chain
		new CDynLinkLibrary(SCHistDLL);

    #if ForSysCAD
		if (!MakeVersionOK("SCHIST.DLL", _MAKENAME, SCD_VERINFO_V0, SCD_VERINFO_V1, SCD_VERINFO_V2, SCD_VERINFO_V3))
      return 0;
    #endif
    }
	else if (dwReason == DLL_PROCESS_DETACH)
    {
		TRACE0("SCHIST.DLL Terminating!\n");
		AfxTermExtensionModule(SCHistDLL);
    }
	return 1;   // ok
  }

// --------------------------------------------------------------------------
