// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__8D76488F_94AC_4E68_924C_38ABB2E1CD26__INCLUDED_)
#define AFX_STDAFX_H__8D76488F_94AC_4E68_924C_38ABB2E1CD26__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN    // Exclude rarely-used stuff from Windows headers
#define _WIN32_DCOM     // This includes new DCOM calls - CoInitailzeEx ...

//#ifdef _DEBUG
//#define _ATL_DEBUG_INTERFACES
//#define _ATL_DEBUG_REFCOUNT
//#define _ATL_DEBUG_QI
//#endif

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>   // MFC support for Internet Explorer 4 Common Controls

#include <comdef.h>        
#import "c:\program files\common files\system\ado\msado15.dll" rename("EOF", "adEOF") no_implementation
#import "c:\Program Files\Common Files\system\ado\msadox.dll" no_implementation

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>     // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxcview.h>     // MFC support for Windows Common Controls

#include <afxmt.h>
#include <afxtempl.h>
#include <comdef.h>
#include <math.h>
#include <float.h>

#include "..\..\..\SMDK\Include\ScdIF.h"
#include "..\..\..\SMDK\Include\md_share1.h"
#include "..\..\Common\Scd\Scdlib\sc_defs.h"
#include "..\..\Common\Scd\Scdlib\mathlib.h"
#include "..\..\Common\Scd\Scdlib\noise.h"
#include "..\..\Common\Scd\ScExec\pgm_e.h"
#include "OPCWrap.h"
#include "OPCError.h"


#define _ATL_APARTMENT_THREADED
#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
class CSysCADMarshalModule : public CComModule
{
public:
  LONG Unlock();
  LONG Lock();
  DWORD dwThreadID;
};
extern CSysCADMarshalModule _Module;
#include <atlcom.h>

#include "scdtempl.h"
#include "tagobj.h"


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__8D76488F_94AC_4E68_924C_38ABB2E1CD26__INCLUDED_)
