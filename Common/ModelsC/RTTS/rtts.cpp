//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include <afxdllx.h>

#define __RTTS_CPP

#define SIZEDST1
#define COMMN1

#include "sc_defs.h"
#include "scdver.h"
#include "rtts.h"
#include "Models.h"
#include "sfe_srvr.h"
#include "op_db.h"
#include "RTDlgs.h"

#pragma LIBCOMMENT("..\\..\\..\\smdk\\Bin\\", "\\SMDK1" )
//#pragma LIBCOMMENT("..\\..\\smdk\\SMDKBase\\", "\\SMDKBase" )

//===========================================================================
//
//                    Visual C++ Stuff
//
//===========================================================================

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

static AFX_EXTENSION_MODULE RTTSDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE1("RTTS.DLL Initializing %08x!\n", hInstance);

		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(RTTSDLL, hInstance))
      return 0;

		// Insert this DLL into the resource chain
		new CDynLinkLibrary(RTTSDLL);

		if (!MakeVersionOK("RTTS.DLL", _MAKENAME, SCD_VERINFO_V0, SCD_VERINFO_V1, SCD_VERINFO_V2, SCD_VERINFO_V3))
      return 0;

    //ForceLoadModelLibrary();

	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("RTTS.DLL Terminating!\n");
		AfxTermExtensionModule(RTTSDLL);
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
  pDLLInfo->sName = "RTTS";
  //pDLLInfo->AddDependentDLL("FlwLib");
  pDLLInfo->AddDependentDLL("MdlBase");
  pDLLInfo->AddDependentDLL("SizeDst1");
  pDLLInfo->AddDependentDLL("Commn1");
  return TRUE;
  }

//---------------------------------------------------------------------------

extern "C" DLLEXPORT CMdlCfgBase* GetCfgPropPg(int iPage, CMdlCfgSheet * pSheet)
  {
  switch (iPage)
    {
    case 0: return new CRTTSCfgPage(pSheet);
    default : return NULL;
    }

  return NULL;
  };

//---------------------------------------------------------------------------

extern "C" DLLEXPORT void InitCfg()
  {
  ODB.InitAndLoad();
  };

//===========================================================================
//
//
//
//===========================================================================

void ForceLoad_RTTS()
  {
  // Dummy Function to allow other libraries to force load this one
  }

//===========================================================================
//
//
//
//===========================================================================
