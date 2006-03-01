#if !defined(AFX_SLOTCFGDLG_H__00A417AC_8831_4163_8C57_4DB6DBFA5849__INCLUDED_)
#define AFX_SLOTCFGDLG_H__00A417AC_8831_4163_8C57_4DB6DBFA5849__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SlotCfgDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSlotCfgDlg dialog
class CTagView;

class CSlotCfgDlg : public CDialog
{
// Construction
public:
	CSlotCfgDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSlotCfgDlg)
	enum { IDD = IDD_SLOTCONFIG };
	CString m_sConnects;
	CString m_sDesc;
	CString m_sDriver;
	CString m_sModifier;
	CString m_sRange;
	CString m_sTag;
	CString m_sType;
	//}}AFX_DATA

  static CSlotCfgDlg * Start(CSlotCfgStrings & Cfg, CTagView *pTagView);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSlotCfgDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSlotCfgDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

  CTagView *m_pTagView;

  void AddEdit(eSlotCfgEdits What); 
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SLOTCFGDLG_H__00A417AC_8831_4163_8C57_4DB6DBFA5849__INCLUDED_)
