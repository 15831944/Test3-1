// TestMarshal.h : main header file for the TESTMARSHAL application
//

#if !defined(AFX_TESTMARSHAL_H__3DBEDB39_A534_46E8_9C44_254288F582B5__INCLUDED_)
#define AFX_TESTMARSHAL_H__3DBEDB39_A534_46E8_9C44_254288F582B5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CTestMarshalApp:
// See TestMarshal.cpp for the implementation of this class
//

class CTestMarshalApp : public CWinApp
{
public:
	CTestMarshalApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestMarshalApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTestMarshalApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTMARSHAL_H__3DBEDB39_A534_46E8_9C44_254288F582B5__INCLUDED_)
