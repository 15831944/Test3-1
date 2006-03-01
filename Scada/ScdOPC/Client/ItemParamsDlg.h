#if !defined(AFX_ITEMPARAMSDLG_H__FC55FE99_BD44_11D1_8DEB_C0F27A000000__INCLUDED_)
#define AFX_ITEMPARAMSDLG_H__FC55FE99_BD44_11D1_8DEB_C0F27A000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ItemParamsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ItemParamsDlg dialog

class ItemParamsDlg : public CDialog
{
// Construction
public:
	ItemParamsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(ItemParamsDlg)
	enum { IDD = IDD_ITEM_PARAMS };
	BOOL	m_active;
	CString	m_accessPath;
	CString	m_accessRights;
	CString	m_clientHandle;
	CString	m_euType;
	CString	m_euValue;
	CString	m_itemID;
	CString	m_nativeData;
	CString	m_requestedData;
	CString	m_serverHandle;
	//}}AFX_DATA
    OPCITEMATTRIBUTES* pItemAttr;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ItemParamsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ItemParamsDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ITEMPARAMSDLG_H__FC55FE99_BD44_11D1_8DEB_C0F27A000000__INCLUDED_)
