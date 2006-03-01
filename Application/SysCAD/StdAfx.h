// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__A49FC39C_E359_437B_8811_949CAC05B162__INCLUDED_)
#define AFX_STDAFX_H__A49FC39C_E359_437B_8811_949CAC05B162__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\scdlib\Scd_SHLB.h"

#ifdef _DEBUG
//#define _ATL_DEBUG_INTERFACES  
//#define _ATL_DEBUG_REFCOUNT
//#define _ATL_DEBUG_QI
#endif

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#define _WIN32_DCOM     // This includes new DCOM calls


#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls

#include <comdef.h>        
#import "c:\program files\common files\system\ado\msado15.dll" rename("EOF", "adEOF") no_implementation
#import "c:\Program Files\Common Files\system\ado\msadox.dll" no_implementation

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxtempl.h>			


#include <afxole.h>			
#include <afxconv.h>			
#include <afxpriv.h>			

//#define _ATL_APARTMENT_THREADED
#define _ATL_FREE_THREADED
#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
class CSysCADModule : public CComModule
{
public:
	LONG Unlock();
	LONG Lock();
	DWORD dwThreadID;
};
extern CSysCADModule _Module;
#include <atlcom.h>
#include <comdef.h>

#include "..\scdlib\sc_defs.h"
#include "..\scdlib\gpfuncs.h"

#pragma LIBCOMMENT("..\\..\\com\\scdcom\\", "\\scdcom")
#pragma LIBCOMMENT("..\\..\\scd\\scdlib\\", "\\scdlib")
#pragma LIBCOMMENT("..\\..\\scd\\scexec\\", "\\scexec")
#pragma LIBCOMMENT("..\\..\\scd\\flwlib\\", "\\flwlib")
#pragma LIBCOMMENT("..\\..\\scd\\schist\\", "\\schist")
//#pragma LIBCOMMENT("..\\..\\scd\\scapp1\\", "\\scapp1")

#pragma LIBCOMMENT("..\\..\\scd\\kwdb\\", "\\kwdb")

#pragma LIBCOMMENT("..\\..\\com\\ScdIF\\", "\\ScdIF")
#pragma LIBCOMMENT("..\\..\\com\\ScdMdl\\", "\\ScdMdl")
#pragma LIBCOMMENT("..\\..\\com\\ScdSlv\\", "\\ScdSlv")

#pragma LIBCOMMENT("..\\..\\CCDK\\", "\\CCDK" )

#include "..\..\Com\ScdIF\ScdIF.h"
#include "..\..\Com\ScdMdl\ScdMdl.h"        
#include "..\..\Com\ScdSlv\ScdSlv.h"        
#include "ScdCOMTmpl.h"

//#include "syscad.h"
#include "..\scdlib\scd_wm.h"
#include "..\..\Com\scdif\scdcomcmds.h"
#include "..\..\Com\scdif\scdcomevts.h"
#include "ScdCATIDS.h"

#include <vfw.h>



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A49FC39C_E359_437B_8811_949CAC05B162__INCLUDED_)
