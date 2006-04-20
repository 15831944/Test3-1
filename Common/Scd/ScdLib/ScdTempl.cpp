//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#include "stdafx.h"
#include "sc_defs.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

template<> DllExport UINT AFXAPI HashKey<LPTSTR> (LPTSTR Key)
  { 
  UINT nHash = 0;
	while (*Key)
		nHash = (nHash<<5) + nHash + toupper(*Key++);
	return nHash;
  };

//template<> DllExport UINT AFXAPI HashKey<LPCTSTR> (LPCTSTR Key)
//  { 
//  UINT nHash = 0;
//	while (*Key)
//		nHash = (nHash<<5) + nHash + toupper(*Key++);
//	return nHash;
//  };
//
//
template<> DllExport BOOL AFXAPI CompareElements<LPTSTR, LPTSTR> (const LPTSTR* E1, const LPTSTR* E2)
  { 
  return (_stricmp(*E1, *E2)==0);
  };

template<> DllExport BOOL AFXAPI CompareElements<LPCTSTR, LPCTSTR> (const LPCTSTR* E1, const LPCTSTR* E2)
  { 
  return (_stricmp(*E1, *E2)==0);
  };

