// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//--------------------------------------------------------------------------

//SMDK: NB you must define your SMDK user DLL group name here!
#define DLL_GroupName "LimnDW"
//--------------------------------------------------------------------------

#if !defined(STDAFX_DEMOUSER_H_)
#define STDAFX_DEMOUSER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxtempl.h>

#include "..\..\..\..\SMDK\Include\ScdIF.h"

#define C_LINKAGE /* nothing */
#define DLLMODEL  /* nothing */
#define CONFIGURATIONS 	gs_DWCfgs

#include "md_headers.h"

#include "..\..\newmat10\newmat.h"
#include "limnglbl.h"
#include "LimnStreamMatrix.h"
#include "DiamondFunctions.h"
#include "Spline.h"
#include "LimnModel_DiamondWizard_CommonFunctions.h"


#include "LimnModel_DiamondWizard_EfficiencyCurve.h"
#include "LimnModel_DiamondWizard_DualEfficiencyCurve.h"
#include "LimnModel_DiamondWizard_TrompCurve.h"
#include "LimnModel_DiamondWizard_GrindingMill.h"
#include "LimnModel_DiamondWizard_WhitenCrusher.h"

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(STDAFX_DEMOUSER_H_)
