// ScdLic.h : main header file for the SCDLIC application
//

#if !defined(AFX_SCDLIC_H__F5E565F3_EC0D_11D4_81E4_00A0C903F84F__INCLUDED_)
#define AFX_SCDLIC_H__F5E565F3_EC0D_11D4_81E4_00A0C903F84F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

class CDevLicApp : public CWinApp
{
public:
	CString sLicenseLoc;
  CString sCmdLine; //keep copy of command line
	bool CheckLicense();
	CDevLicApp();
	//{{AFX_VIRTUAL(CDevLicApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL
	//{{AFX_MSG(CDevLicApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern char* CK_KeyFile;
extern char* CK_AppName;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCDLIC_H__F5E565F3_EC0D_11D4_81E4_00A0C903F84F__INCLUDED_)
