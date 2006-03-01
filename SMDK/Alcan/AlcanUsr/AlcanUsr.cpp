//===========================================================================
//=== SysCAD SMDK - Hyprod Precipitator Model 2003 : Alcan, Kenwalt       ===
//===========================================================================

#include "stdafx.h"
#define __ALCANUSR_CPP
#include "AlcanUsr.h"
#include "AlcanLicense.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#include "scdmacros.h" 
#include "md_headers.h" 
#pragma LIBCOMMENT("..\\..\\Bin\\", "\\scdlib" )
#pragma LIBCOMMENT("..\\..\\Bin\\", "\\SMDK1" )
#pragma LIBCOMMENT("..\\..\\Bin\\", "\\ScdIF" )

//===========================================================================
//===========================================================================

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
      {
      //if (!MakeVersionOK("AlcanUsr.DLL", _MAKENAME, SCD_VERINFO_V0, SCD_VERINFO_V1, SCD_VERINFO_V2, SCD_VERINFO_V3))
      //  return FALSE;

      break;
      }
		case DLL_THREAD_ATTACH:
      break;
		case DLL_THREAD_DETACH:
      break;
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}

//===========================================================================

extern "C" __declspec(dllexport) BOOL IsSMDKLibDLL()
  {
  return TRUE;
  }

//===========================================================================

extern "C" __declspec(dllexport) bool LoadInitialise()
  {
  #if UseLicensing
  if (!CheckForUnitsLicense())
    {
    return false;
    }
  #endif
  return true;
  }

//===========================================================================
