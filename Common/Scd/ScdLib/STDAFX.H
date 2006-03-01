//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#include "Scd_SHLB.h"
//#include "Scd_SHLB.h"

#define VC_EXTRALEAN       // Exclude rarely-used stuff from Windows headers

#include <afxwin.h>        // MFC core and standard components
#include <afxext.h>        // MFC extensions
#include <afxdisp.h>        // MFC extensions
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>        // MFC support for Windows 95 Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxtempl.h> // "xafxtempl.h"  

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <float.h>
#include <malloc.h>
#include <memory.h>

#include "..\..\..\SMDK\Include\ScdIF.h"

#define MDLLIBNAME "ScdLib"