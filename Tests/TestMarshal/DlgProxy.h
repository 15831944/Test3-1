// DlgProxy.h : header file
//

#if !defined(AFX_DLGPROXY_H__E1DAF5D7_DCDD_463C_AA03_E3C039EA0FD3__INCLUDED_)
#define AFX_DLGPROXY_H__E1DAF5D7_DCDD_463C_AA03_E3C039EA0FD3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTestMarshalDlg;

/////////////////////////////////////////////////////////////////////////////
// CTestMarshalDlgAutoProxy command target

class CTestMarshalDlgAutoProxy : public CCmdTarget
{
	DECLARE_DYNCREATE(CTestMarshalDlgAutoProxy)

	CTestMarshalDlgAutoProxy();           // protected constructor used by dynamic creation

// Attributes
public:
	CTestMarshalDlg* m_pDialog;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestMarshalDlgAutoProxy)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CTestMarshalDlgAutoProxy();

	// Generated message map functions
	//{{AFX_MSG(CTestMarshalDlgAutoProxy)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	DECLARE_OLECREATE(CTestMarshalDlgAutoProxy)

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CTestMarshalDlgAutoProxy)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPROXY_H__E1DAF5D7_DCDD_463C_AA03_E3C039EA0FD3__INCLUDED_)
