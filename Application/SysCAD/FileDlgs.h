//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __FILEDLGS_H
#define __FILEDLGS_H

#ifndef  __GPWFUNCS_H
#include "gpwfuncs.h"
#endif
#include "scdver.h"
#include "resource.h"


//#ifdef __FILEDLGS_CPP
//  #define DllImportExport DllExport
//#elif !defined(SCAPP1)
//  #define DllImportExport DllImport
//#else
  #define DllImportExport
//#endif


/////////////////////////////////////////////////////////////////////////////
// CSCDFileNewDlg dialog

class DllImportExport CSCDFileNewDlg : public CDialog
{
// Construction
public:
	CSCDFileNewDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSCDFileNewDlg)
	enum { IDD = IDD_SCDFILENEW };
	CButton	m_NewCtrlTrend;
	CButton	m_NewTrend;
	CButton	m_NewVisio;
	CButton	m_NewGraphics;
	//}}AFX_DATA

  CDocTemplate * m_pReqdTemplate;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSCDFileNewDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL


// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSCDFileNewDlg)
	afx_msg void OnNewgraphics();
	afx_msg void OnNewtrend();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

  void SetEnables();
};

/////////////////////////////////////////////////////////////////////////////
// CNewGrfDocOptions dialog

class CNewGrfDocOptions : public CDialog
{
// Construction
public:
	CNewGrfDocOptions(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNewGrfDocOptions)
	enum { IDD = IDD_NEWGRFDOC };
	CEdit m_NewFileCtrl;
	CEdit m_InitFileCtrl;
	BOOL m_ExistingFile;
	//}}AFX_DATA
	Strng	m_NewFile;
	Strng	m_InitFile;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewGrfDocOptions)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CProfINIFile PF;
	// Generated message map functions
	//{{AFX_MSG(CNewGrfDocOptions)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnBrowseinifile();
	afx_msg void OnBrowsenewfile();
	afx_msg void OnChangeEditinitfile();
	afx_msg void OnChangeEditnewfile();
	afx_msg void OnExistingfile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
  void UpdateEnables();
};

#undef DllImportExport

#endif
