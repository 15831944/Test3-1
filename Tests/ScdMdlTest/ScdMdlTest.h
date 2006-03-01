// ScdMdlTest.h : main header file for the SCDMDLTEST application
//

#if !defined(AFX_SCDMDLTEST_H__4EC06AF8_A498_44A3_821C_9AFB78D186EE__INCLUDED_)
#define AFX_SCDMDLTEST_H__4EC06AF8_A498_44A3_821C_9AFB78D186EE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CScdMdlTestApp:
// See ScdMdlTest.cpp for the implementation of this class
//

class CScdMdlTestApp : public CWinApp
{
public:
	CScdMdlTestApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScdMdlTestApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CScdMdlTestApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCDMDLTEST_H__4EC06AF8_A498_44A3_821C_9AFB78D186EE__INCLUDED_)
