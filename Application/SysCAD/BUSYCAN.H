//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// busycan.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CBusyCan dialog

class CBusyCan : public CDialog
{
// Construction
public:
	CBusyCan(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBusyCan)
	enum { IDD = IDD_BusyCancel };
	CString	m_Msg;
	//}}AFX_DATA

  BOOL m_Cancelled;
  void SetMsg(char * pMsg) {m_Msg=pMsg; UpdateData(FALSE);};

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBusyCan)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBusyCan)
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
