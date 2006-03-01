// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#ifdef _DEBUG
#define _ATL_DEBUG_INTERFACES
#define _ATL_DEBUG_REFCOUNT
//#define _ATL_DEBUG_QI
#endif

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC OLE automation classes
#include <afxtempl.h>        
#include <comdef.h>        
#endif // _AFX_NO_OLE_SUPPORT


#import "c:\program files\common files\system\ado\msado15.dll" rename("EOF", "adEOF"), no_implementation
#import "c:\Program Files\Common Files\system\ado\msadox.dll" no_implementation
#import "c:\Program Files\Common Files\system\ado\msjro.dll"  no_implementation

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT





