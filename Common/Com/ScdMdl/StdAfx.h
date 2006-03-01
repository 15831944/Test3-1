// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__08140E7A_A54E_4B33_8DED_7279F176758C__INCLUDED_)
#define AFX_STDAFX_H__08140E7A_A54E_4B33_8DED_7279F176758C__INCLUDED_

#include "Scd_SHLB.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define STRICT
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif
#define _ATL_APARTMENT_THREADED

#ifdef _DEBUG
//#define _ATL_DEBUG_INTERFACES
//#define _ATL_DEBUG_REFCOUNT
//#define _ATL_DEBUG_QI
#endif

#include <afxwin.h>
#include <afxdisp.h>

// ******* from ...
#include <afxext.h>         // MFC extensions
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>         // MFC support for Windows 95 Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
// *******
#include <afxtempl.h>			


#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>
#include <comdef.h>


#include "sc_defs.h"
#include "gpfuncs.h"

#pragma LIBCOMMENT("..\\..\\com\\ScdIF\\", "\\ScdIF")
//#pragma LIBCOMMENT("..\\..\\com\\scdcom\\", "\\scdcom")
#pragma LIBCOMMENT("..\\..\\scd\\scdlib\\", "\\scdlib")
#pragma LIBCOMMENT("..\\..\\scd\\scexec\\", "\\scexec")
#pragma LIBCOMMENT("..\\..\\scd\\flwlib\\", "\\flwlib")

#include "..\ScdIF\ScdIF.h"
#include "ScdCOMTmpl.h"
#include "ScdCATIDS.h"

#include "sfe_srvr.h"



#define MDLLIBNAME "ScdMdl"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__08140E7A_A54E_4B33_8DED_7279F176758C__INCLUDED)
