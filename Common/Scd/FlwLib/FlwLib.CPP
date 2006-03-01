//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// FlwLib.cpp : Defines the initialization routines for the DLL.
//
 
#include "stdafx.h"
#include "sc_defs.h"
#include "scdver.h"
#include "fe_const.h"
#include "sfe_srvr.h"
#include "flweqn.h"
#include "mdlcfg.h"
#include <afxdllx.h>

#ifdef _DEBUG          
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#pragma LIBCOMMENT("..\\..\\com\\ScdIF\\", "\\ScdIF" )
#pragma LIBCOMMENT("..\\..\\scd\\ScdLib\\", "\\ScdLib" )
#pragma LIBCOMMENT("..\\..\\scd\\ScExec\\", "\\ScExec" )
#pragma LIBCOMMENT("..\\..\\scd\\XyLib\\", "\\XyLib" )

static AFX_EXTENSION_MODULE FlwLibDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE1("FLWLIB.DLL Initializing %08x!\n", hInstance);
		
		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(FlwLibDLL, hInstance))
      return 0;

		// Insert this DLL into the resource chain
		new CDynLinkLibrary(FlwLibDLL);

		if (!MakeVersionOK("FLWLIB.DLL", _MAKENAME, SCD_VERINFO_V0, SCD_VERINFO_V1, SCD_VERINFO_V2, SCD_VERINFO_V3))
      return 0;

#if WithOEP
    SetOEPOptions(False); // OEP should load after this and reset if neccessary
#endif
#if WithQAL
    SetQALOptions(False); // QAL should load after this and reset if neccessary
#endif
#if WithMG
    SetMGOptions(False); // MG should load after this and reset if neccessary
#endif

	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("FLWLIB.DLL Terminating!\n");
		AfxTermExtensionModule(FlwLibDLL);
	}
	return 1;   // ok
}

//===========================================================================

#define DLLEXPORT __declspec( dllexport ) 

extern "C" DLLEXPORT CSfeSrvr* AddSfeSrvr()
  {
  return new CSfeSrvr;
  }

extern "C" DLLEXPORT CSfeSrvrBase* AddSfeSrvrBase()
  {
  return new CSfeSrvrBase;
  }

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
  pDLLInfo->iPriority = 0;
  pDLLInfo->sName = "FlwLib";
  return TRUE;
  }

//---------------------------------------------------------------------------

extern "C" DLLEXPORT CMdlCfgBase* GetCfgPropPg(int iPage, CMdlCfgSheet * pSheet)
  {
  return NULL;
  };

//---------------------------------------------------------------------------
