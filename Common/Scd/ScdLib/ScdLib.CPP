//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SCDLib.cpp : Defines the initialization routines for the DLL.
//
 
#include "stdafx.h"
#include "sc_defs.h"
#include "scdver.h"
#include <afxdllx.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
         
#pragma LIBCOMMENT("..\\..\\com\\ScdIF\\", "\\ScdIF" )
        
/*static*/ AFX_EXTENSION_MODULE SCDLibDLL = { NULL, NULL };
                           
extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE1("SCDLIB.DLL Initializing %08x!\n", hInstance);
		
		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(SCDLibDLL, hInstance))
      return 0;

		// Insert this DLL into the resource chain
		new CDynLinkLibrary(SCDLibDLL);

		if (!MakeVersionOK("SCDLIB.DLL", _MAKENAME, SCD_VERINFO_V0, SCD_VERINFO_V1, SCD_VERINFO_V2, SCD_VERINFO_V3))
      return 0;
		TRACE0("SCDLIB.DLL Initializing Done!\n");

	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("SCDLIB.DLL Terminating!\n");
		AfxTermExtensionModule(SCDLibDLL);
	}
	return 1;   // ok
}
        