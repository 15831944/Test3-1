//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#pragma warning(disable:4786)

#include "stdafx.h"
#define __TS_SMDK_CPP
#include "TS_SMDK.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#include "scdmacros.h" 
#include "md_headers.h" 

#include "oreprops.h"


#pragma LIBCOMMENT("..\\..\\Bin\\", "\\scdlib" )
#pragma LIBCOMMENT("..\\..\\Bin\\", "\\DevLib" )
#pragma LIBCOMMENT("..\\..\\Bin\\", "\\ScdIF" )

//===========================================================================

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{

	// Establish a connection to the ore properties database
	//void* ptr = GetGlobalOther();
	//pRT_OreProp = NULL;
	//if (ptr)
	//{
	//	pRT_OreProp = (RT_OreProp*)ptr;
	//}

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

void ForceLoad_UserMethods()
  {
  // Dummy Function to allow other libraries to force load this one
  }

//===========================================================================

extern "C" __declspec(dllexport) BOOL IsSMDKLibDLL()
  {
  return TRUE;
  }

//-- End TS_ModelLib1.cpp ---------------------------------------------------

