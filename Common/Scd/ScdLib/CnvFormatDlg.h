#if !defined(AFX_CNVFORMATDLG_H__B2098608_31C0_4AA2_8623_6BEF82A39FB7__INCLUDED_)
#define AFX_CNVFORMATDLG_H__B2098608_31C0_4AA2_8623_6BEF82A39FB7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CnvFormatDlg.h : header file
//
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CCnvFormatDlg dialog

class CCnvFormatDlg : public CDialog
{
// Construction
public:
	CCnvFormatDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCnvFormatDlg)
	enum { IDD = IDD_CNVFMT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCnvFormatDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCnvFormatDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CNVFORMATDLG_H__B2098608_31C0_4AA2_8623_6BEF82A39FB7__INCLUDED_)
