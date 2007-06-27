//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define __DEVLIB_CPP
#include "DevLib.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

AFX_EXTENSION_MODULE DevLibDLL = { NULL, NULL };

//===========================================================================

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
  {
	if (dwReason == DLL_PROCESS_ATTACH)
    {
		TRACE0("DevLib.DLL Initializing!\n");
		
		// Extension DLL one-time initialization
		AfxInitExtensionModule(DevLibDLL, hInstance);

		// Insert this DLL into the resource chain
		new CDynLinkLibrary(DevLibDLL);

		//if (!MakeVersionOK("DevLib.DLL", _MAKENAME, SCD_VERINFO_V0, SCD_VERINFO_V1, SCD_VERINFO_V2, SCD_VERINFO_V3))
    //  return 0;

    }
	else if (dwReason == DLL_PROCESS_DETACH)
    {
		TRACE0("DevLib.DLL Terminating!\n");
		AfxTermExtensionModule(DevLibDLL);
    }
	return 1;   // ok
  }

//===========================================================================

