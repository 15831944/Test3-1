//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/***************************************************************************
****************************************************************************/

#include "stdafx.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <float.h>

#include "sc_defs.h"
#include "scdver.h"

#include "xylib.h"

#pragma LIBCOMMENT("..\\..\\com\\ScdIF\\", "\\ScdIF" )
#pragma LIBCOMMENT("..\\..\\scd\\ScdLib\\", "\\ScdLib" )
#pragma LIBCOMMENT("..\\..\\scd\\ScExec\\", "\\ScExec" )

//===========================================================================
// Standard Requirement

#include <afxdllx.h>

static AFX_EXTENSION_MODULE extensionDLL;

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID)
  {
  if (dwReason == DLL_PROCESS_ATTACH)
    {
		TRACE1("XYLIB.DLL Initializing %08x!\n", hInstance);
    // Extension DLL one-time initialization
    if (!AfxInitExtensionModule(extensionDLL, hInstance))
      return 0;
    
    new CDynLinkLibrary(extensionDLL);

		if (!MakeVersionOK("XYLIB.DLL", _MAKENAME, SCD_VERINFO_V0, SCD_VERINFO_V1, SCD_VERINFO_V2, SCD_VERINFO_V3))
      return 0;

    }
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("XYLIB.DLL Terminating!\n");
		AfxTermExtensionModule(extensionDLL);
	}
  return 1;
  }

//===========================================================================


