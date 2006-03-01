//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// Defines the initialization routines for the DLL.
#include "stdafx.h"
#include <afxdllx.h>

#define __QALMDLS_CPP

#include "sc_defs.h"
#include "scdver.h"
#define MDLBASE
#include "Models.h"
#include "QALMdls.h"
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

static AFX_EXTENSION_MODULE QALMDLSDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
  {
  if (dwReason == DLL_PROCESS_ATTACH)
    {
    TRACE1("QALMDLS.DLL Initializing %08x!\n", hInstance);

    // Extension DLL one-time initialization
    if (!AfxInitExtensionModule(QALMDLSDLL, hInstance))
      return 0;

    // Insert this DLL into the resource chain
    new CDynLinkLibrary(QALMDLSDLL);

    if (!MakeVersionOK("QALMDLS.DLL", _MAKENAME, SCD_VERINFO_V0, SCD_VERINFO_V1, SCD_VERINFO_V2, SCD_VERINFO_V3))
      return 0;

    //ForceLoadModelLibrary();

    }
  else if (dwReason == DLL_PROCESS_DETACH)
    {
    TRACE0("QALMDLS.DLL Terminating!\n");
    AfxTermExtensionModule(QALMDLSDLL);
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
  pDLLInfo->iPriority = 7;
  pDLLInfo->sName = "QALMdls";
  //pDLLInfo->AddDependentDLL("FlwLib");
  pDLLInfo->AddDependentDLL("MdlBase");
  pDLLInfo->AddDependentDLL("QALSpMdl");
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

void ForceLoad_QALMDLS()
  {
  // Dummy Function to allow other libraries to force load this one
  }

//===========================================================================
//
//
//
//===========================================================================
