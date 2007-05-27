##   Time-stamp: <2007-05-27 17:13:33 Rod Stephenson Transcritical Pty Ltd>
##   smdkgen.py - generate all the files necessary for generic SMDK project
##   Basically just specify the dll group name and go from there...

from string import Template

dllGroupName = "TTDynTest"
udllGroupName = dllGroupName.upper()



stdafxhFile = """\
// stdafx.h : include file for standard system include files,
//   Time-stamp: <2007-05-26 06:43:25 Rod Stephenson Transcritical Pty Ltd>
//  or project specific include files that are used frequently, but
//      are changed infrequently
//--------------------------------------------------------------------------

//SMDK: NB you must define your SMDK user DLL group name here!
#define DLL_GroupName "${dllGroupName}"
//--------------------------------------------------------------------------

#if !defined(STDAFX_${udllGroupName}_H_)
#define STDAFX_${udllGroupName}_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxtempl.h>

#include "ScdIF.h"

//#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(STDAFX__H_)
"""

s1  = Template(stdafxhFile)
s1 = s1.safe_substitute(dllGroupName=dllGroupName, udllGroupName=udllGroupName)
print s1


stdafxcppFile = """\
// stdafx.cpp : source file that includes just the standard includes
//   Time-stamp: <2006-11-01 11:56:06 Rod Stephenson Transcritical Pty Ltd>

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file
"""

