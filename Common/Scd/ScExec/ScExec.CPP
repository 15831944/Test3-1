//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// scexec.cpp : Defines the initialization routines for the DLL.
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
#pragma LIBCOMMENT("..\\..\\scd\\ScdLib\\", "\\ScdLib" )
#pragma LIBCOMMENT("..\\..\\scd\\kwdb\\", "\\kwdb" )

#import "c:\program files\common files\system\ado\msado15.dll" rename("EOF", "adEOF"), implementation_only
#import "c:\Program Files\Common Files\system\ado\msadox.dll" implementation_only

AFX_EXTENSION_MODULE SCExecDLL = { NULL, NULL };

// --------------------------------------------------------------------------

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
  {
	if (dwReason == DLL_PROCESS_ATTACH)
    {
		TRACE1("SCEXEC.DLL Initializing %08x!\n", hInstance);
		
		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(SCExecDLL, hInstance))
      return 0;

		// Insert this DLL into the resource chain
		new CDynLinkLibrary(SCExecDLL);

		if (!MakeVersionOK("SCEXEC.DLL", _MAKENAME, SCD_VERINFO_V0, SCD_VERINFO_V1, SCD_VERINFO_V2, SCD_VERINFO_V3))
      return 0;

    }
	else if (dwReason == DLL_PROCESS_DETACH)
    {
		TRACE0("SCEXEC.DLL Terminating!\n");
		AfxTermExtensionModule(SCExecDLL);
    }
	return 1;   // ok
  }

// --------------------------------------------------------------------------
