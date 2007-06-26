//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define __DEVLIBEX_CPP
#include "DevLibEx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

AFX_EXTENSION_MODULE DevLibExDLL = { NULL, NULL };

//===========================================================================

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
  {
	if (dwReason == DLL_PROCESS_ATTACH)
    {
		TRACE0("DevLibEx.DLL Initializing!\n");
		
		// Extension DLL one-time initialization
		AfxInitExtensionModule(DevLibExDLL, hInstance);

		// Insert this DLL into the resource chain
		new CDynLinkLibrary(DevLibExDLL);

		//if (!MakeVersionOK("DevLibEx.DLL", _MAKENAME, SCD_VERINFO_V0, SCD_VERINFO_V1, SCD_VERINFO_V2, SCD_VERINFO_V3))
    //  return 0;

    }
	else if (dwReason == DLL_PROCESS_DETACH)
    {
		TRACE0("DevLibEx.DLL Terminating!\n");
		AfxTermExtensionModule(DevLibExDLL);
    }
	return 1;   // ok
  }

//===========================================================================

