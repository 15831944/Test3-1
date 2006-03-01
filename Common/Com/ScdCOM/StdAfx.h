// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__2D321788_1C93_473B_8339_AE2E18366C11__INCLUDED_)
#define AFX_STDAFX_H__2D321788_1C93_473B_8339_AE2E18366C11__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define STRICT
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif
#define _ATL_APARTMENT_THREADED

//==========================================
#include "Scd_SHLB.h"

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#define _WIN32_DCOM     // This includes new DCOM calls

//------------------
//#include <afxtempl.h> // "xafxtempl.h"
#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>
#include <comdef.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <float.h>
#include <malloc.h>
#include <memory.h>

#define MDLLIBNAME "ScdCOM"
//*************


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__2D321788_1C93_473B_8339_AE2E18366C11__INCLUDED)




