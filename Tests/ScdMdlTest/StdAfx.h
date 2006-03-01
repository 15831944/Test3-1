// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__03AF9333_FE6C_4DCE_954B_7E52335C04DC__INCLUDED_)
#define AFX_STDAFX_H__03AF9333_FE6C_4DCE_954B_7E52335C04DC__INCLUDED_

//#include "Scd_SHLB.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#define _WIN32_DCOM    

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxtempl.h>			

//#ifdef _DEBUG
//#define _ATL_DEBUG_INTERFACES
//#define _ATL_DEBUG_QI
//#define _ATL_DEBUG_REFCOUNT
//#endif

#include <atlbase.h>
#include "..\..\com\ScdMdl\scdcatids.h"
//#include "..\ScdIF\ScdIF.h"
#import "..\..\ScdMDK\ScdIF.tlb"  no_namespace, named_guids
#include "..\..\..\SMDK\Include\scdcomtmpl.h"
//#include "..\scdmdl\scdmdl.h"

#import "..\..\ScdMDK\ScdMdl.tlb"  no_namespace, named_guids
#import "..\..\ScdMDK\ScdULB.tlb"  no_namespace, named_guids//, exclude("IScdApplication"), exclude("IScdSpDB"), exclude("IScdSpModel"), exclude("IScdSpProps"), exclude("IScdSpProps"), exclude("IScdSpAluminaProps")
#import "..\..\ScdMDK\SysCAD.tlb"  no_namespace, named_guids//, exclude("IScdSpDB"), exclude("IScdSpModel"), exclude("IScdSpProps"), exclude("IScdSpProps"), exclude("IScdSpAluminaProps")


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__03AF9333_FE6C_4DCE_954B_7E52335C04DC__INCLUDED_)
