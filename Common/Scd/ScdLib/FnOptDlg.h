#if !defined(AFX_FNOPTDLG_H__44DDEF5D_3274_11D2_BB0B_0000E82A5623__INCLUDED_)
#define AFX_FNOPTDLG_H__44DDEF5D_3274_11D2_BB0B_0000E82A5623__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// FnOptDlg.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CFnOptDlg dialog

class CFnOptDlg : public CDialog
{
// Construction
public:
	CFnOptDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	CString	m_FullFileRmt;
	CString	m_FullFileLcl;
	CString	m_FullFileUse;

  static int m_AllOK;  
  static int m_SkipDlg;

  int       OkRmt, OkLcl;
  int       OldRmt, OldLcl;
	int		    m_UseRmt;
	int		    m_UseLclDefault;
	int		    m_UseRmtDefault;
  int       m_HasNote;
  CString   m_TheNote;
  
  //{{AFX_DATA(CFnOptDlg)
	enum { IDD = IDD_FNOPTDLG };
	CString	m_FileRmt;
	CString	m_FileLcl;
	CString	m_TimeRmt;
	CString	m_TimeLcl;
	CString	m_Note;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFnOptDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  void UpdateRadios();
  bool UpdateInfo();
  void SetFileUse(int Rmt);
  void SetReason();

	// Generated message map functions
	//{{AFX_MSG(CFnOptDlg)
	virtual void OnOK();
	afx_msg void OnOkall();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnUselcl();
	afx_msg void OnUsenewest();
	afx_msg void OnReason1();
	afx_msg void OnReason2();
	afx_msg void OnUsermt();
	afx_msg void OnBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FNOPTDLG_H__44DDEF5D_3274_11D2_BB0B_0000E82A5623__INCLUDED_)
