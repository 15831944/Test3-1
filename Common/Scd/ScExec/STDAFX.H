//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#include "Scd_SHLB.h"

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#define _WIN32_DCOM     // This includes new DCOM calls

#ifdef _DEBUG
#define _ATL_DEBUG_INTERFACES
#define _ATL_DEBUG_REFCOUNT
#define _ATL_DEBUG_QI
#endif

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxole.h>         // MFC OLE classes
//#include <afxodlgs.h>       // MFC OLE dialog classes
//#include <afxdisp.h>        // MFC OLE automation classes
#include <afxtempl.h>        

#include <comdef.h>        
#import "c:\program files\common files\system\ado\msado15.dll" rename("EOF", "adEOF"), no_implementation
#import "c:\Program Files\Common Files\system\ado\msadox.dll" no_implementation

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows 95 Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <float.h>
#include <malloc.h>
#include <memory.h>

#include "..\..\..\SMDK\Include\ScdIF.h"


#define MDLLIBNAME "SCExec"