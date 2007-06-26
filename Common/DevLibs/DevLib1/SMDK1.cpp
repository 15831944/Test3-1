//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define __SMDK1_CPP
#include "smdk1.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

AFX_EXTENSION_MODULE SMDK1DLL = { NULL, NULL };

//===========================================================================

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
  {
	if (dwReason == DLL_PROCESS_ATTACH)
    {
		TRACE0("SMDK1.DLL Initializing!\n");
		
		// Extension DLL one-time initialization
		AfxInitExtensionModule(SMDK1DLL, hInstance);

		// Insert this DLL into the resource chain
		new CDynLinkLibrary(SMDK1DLL);

		//if (!MakeVersionOK("SMDK1.DLL", _MAKENAME, SCD_VERINFO_V0, SCD_VERINFO_V1, SCD_VERINFO_V2, SCD_VERINFO_V3))
    //  return 0;

    }
	else if (dwReason == DLL_PROCESS_DETACH)
    {
		TRACE0("SMDK1.DLL Terminating!\n");
		AfxTermExtensionModule(SMDK1DLL);
    }
	return 1;   // ok
  }

//===========================================================================

