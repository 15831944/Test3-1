// LicDlg.h : header file

#if !defined(AFX_SCDLICDLG_H__F5E565F5_EC0D_11D4_81E4_00A0C903F84F__INCLUDED_)
#define AFX_SCDLICDLG_H__F5E565F5_EC0D_11D4_81E4_00A0C903F84F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//---------------------------------------------------------------------------

class CDevLicDlg : public CDialog
  {
  public:
	  CDevLicDlg(CWnd* pParent = NULL);	// standard constructor
    ~CDevLicDlg();
	  //{{AFX_DATA(CDevLicDlg)
	  enum { IDD = IDD_SCDLIC_DIALOG };
	  //}}AFX_DATA
	  //{{AFX_VIRTUAL(CDevLicDlg)
	  protected:
	  virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	  //}}AFX_VIRTUAL
  public:
    bool bForcedLoc;
    CString sForcedLoc;
  protected:
	  HICON m_hIcon;
	  //{{AFX_MSG(CDevLicDlg)
	  virtual BOOL OnInitDialog();
	  afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	  afx_msg void OnPaint();
	  afx_msg HCURSOR OnQueryDragIcon();
	  afx_msg void OnInit();
	  afx_msg void OnLocation();
    afx_msg void OnUpdateLocation(CCmdUI* pCmdUI);
    afx_msg void OnButtonDll();
    afx_msg void OnLaunchLicense();
	afx_msg void OnCheckLicense();
	//}}AFX_MSG
	  DECLARE_MESSAGE_MAP()
  public:
    afx_msg void OnBnClickedClipboardcopy();
  };

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCDLICDLG_H__F5E565F5_EC0D_11D4_81E4_00A0C903F84F__INCLUDED_)
//---------------------------------------------------------------------------
