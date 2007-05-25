//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//   Time-stamp: <2007-05-24 07:16:27 Rod Stephenson Transcritical Pty Ltd>
//===========================================================================

#include "stdafx.h"
#define __TTDX_CPP
#include "TTDX.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#include "scdmacros.h" 
#include "md_headers.h"

#pragma LIBCOMMENT("..\\..\\Bin\\", "\\SMDK1" )
#pragma LIBCOMMENT("..\\..\\Bin\\", "\\scdlib" )

//===========================================================================

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}

//===========================================================================

void ForceLoad_TTDX()
  {
  // Dummy Function to allow other libraries to force load this one
  }

//===========================================================================

extern "C" __declspec(dllexport) BOOL IsSMDKLibDLL()
  {
  return TRUE;
  }


//===========================================================================

