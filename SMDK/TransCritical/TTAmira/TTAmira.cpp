//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define __TTAMIRA_CPP
#include "TTAmira.h"

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

extern "C" __declspec(dllexport) BOOL IsSMDKLibDLL()
  {
  return TRUE;
  }
//===========================================================================

