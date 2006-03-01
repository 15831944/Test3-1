//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#define ForSysCAD 1

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#if ForSysCAD
#include "Scd_SHLB.h"

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxole.h>         // MFC OLE classes

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows 95 Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxtempl.h>			

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <float.h>
#include <malloc.h>
#include <memory.h>

#include "..\..\..\SMDK\Include\ScdIF.h"


#else

#include <afxwin.h>         // MFC core and standard components

#define DllImport __declspec( dllimport )
#define DllExport __declspec( dllexport )
typedef unsigned char          byte;
typedef unsigned char          flag;
#if defined( __cplusplus )
#  define _PDEF(name) typedef name * p##name;
#  define _RDEF(name) typedef name & r##name;
#  define _RPDEF(name) typedef name *& rp##name;
#  define _FWDDEF(name) class name; \
    _PDEF(name) \
    _RDEF(name) \
    _RPDEF(name)
#endif
#define IMPLEMENT_MEMSTATS(Obj)
#define DEFINE_MEMSTATS(Obj)
template <class T> T Min(const T x, const T y) { return (x < y) ? x : y; };
template <class T> T Max(const T x, const T y) { return (x > y) ? x : y; };
template <class T> T Range(const T n, const T x, const T m) { return (x >= n) ? ((x <= m) ? x : m ) : n; };

#define WMU_HSTQRYUPDATE         (WM_USER + 539)

#endif

#define MDLLIBNAME "ScHist"

