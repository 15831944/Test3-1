//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#include "stdafx.h"
#include "sc_defs.h"

#define MDLBASE
//#define BASIC1
#define SIZEDST1
#include "models.h"

#include "flwnode.h"
#include <afxdllx.h>

#define __QALPRCP1_CPP

#include "scdver.h"
#include "QALPRCP1.h"
#include "sfe_srvr.h"

//===========================================================================
//
//                    Visual C++ Stuff
//
//===========================================================================

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

static AFX_EXTENSION_MODULE QALPRCP1DLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
  if (dwReason == DLL_PROCESS_ATTACH)
  {
    TRACE1("QALPRCP1.DLL Initializing %08x!\n", hInstance);

    // Extension DLL one-time initialization
    if (!AfxInitExtensionModule(QALPRCP1DLL, hInstance))
      return 0;

    // Insert this DLL into the resource chain
    new CDynLinkLibrary(QALPRCP1DLL);

    if (!MakeVersionOK("QALPRCP1.DLL", _MAKENAME, SCD_VERINFO_V0, SCD_VERINFO_V1, SCD_VERINFO_V2, SCD_VERINFO_V3))
      return 0;

  //ForceLoadModelLibrary();
  //  ForceLoad_QALSPMDL();

  }
  else if (dwReason == DLL_PROCESS_DETACH)
  {
    TRACE0("QALPRCP1.DLL Terminating!\n");
    AfxTermExtensionModule(QALPRCP1DLL);
  }
  return 1;   // ok
}

//---------------------------------------------------------------------------

#define DLLEXPORT __declspec( dllexport )

//---------------------------------------------------------------------------

extern "C" DLLEXPORT BOOL FilterDllMsg(LPMSG lpMsg)
  {
  return AfxGetApp()->PreTranslateMessage(lpMsg);
  }

//---------------------------------------------------------------------------

extern "C" DLLEXPORT void ProcessDllIdle()
  {
  // flush it all at once
  long lCount = 0;
  while (AfxGetApp()->OnIdle(lCount))
    lCount++;
  }

//---------------------------------------------------------------------------

extern "C" DLLEXPORT BOOL IsFlowLibDLL()
  {
  return TRUE;
  }

//---------------------------------------------------------------------------

extern "C" DLLEXPORT BOOL GetDLLInfo(MDLLInfo* pDLLInfo)
  {
  pDLLInfo->iPriority = 6;
  pDLLInfo->sName = "QALPrcp1";
  //pDLLInfo->AddDependentDLL("FlwLib");
  pDLLInfo->AddDependentDLL("MdlBase");
  pDLLInfo->AddDependentDLL("QALSpMdl");
  pDLLInfo->AddDependentDLL("SizeDst1");
  return TRUE;
  }

//---------------------------------------------------------------------------

extern "C" DLLEXPORT CMdlCfgBase* GetCfgPropPg(int iPage, CMdlCfgSheet * pSheet)
  {
  return NULL;
  };

//===========================================================================
//
//
//
//===========================================================================

void ForceLoad_QALPRCP1()
  {
  // Dummy Function to allow other libraries to force load this one
  }

//===========================================================================
//
//
//
//===========================================================================
