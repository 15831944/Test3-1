// ScdMdlTestDlg.h : header file
//

#if !defined(AFX_SCDMDLTESTDLG_H__A128C706_F961_47C5_BAD1_7D9A752D7C88__INCLUDED_)
#define AFX_SCDMDLTESTDLG_H__A128C706_F961_47C5_BAD1_7D9A752D7C88__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CScdMdlTestDlg dialog

class CScdMdlTestDlg : public CDialog
{
// Construction
public:
  CScdMdlTestDlg(CWnd* pParent = NULL); // standard constructor
  ~CScdMdlTestDlg();
  
// Dialog Data
  //{{AFX_DATA(CScdMdlTestDlg)
  enum { IDD = IDD_SCDMDLTEST_DIALOG };
  CListBox  m_SpMdls;
  //}}AFX_DATA

  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CScdMdlTestDlg)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);  // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:
  HICON m_hIcon;

  // Generated message map functions
  //{{AFX_MSG(CScdMdlTestDlg)
  virtual BOOL OnInitDialog();
  afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
  afx_msg void OnPaint();
  afx_msg HCURSOR OnQueryDragIcon();
  afx_msg void OnClose();
  virtual void OnOK();
  virtual void OnCancel();
  afx_msg void OnSpdb();
  afx_msg void OnSpmodel();
  afx_msg void OnUsrspmdl();
  afx_msg void OnCategory();
  afx_msg void OnClear();
  afx_msg void OnInitialise();
  afx_msg void OnAll();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

  // members

  IScdApplicationPtr g_spApp;
  IScdEnvironmentPtr g_spEnv;
  //P(IScdApplication) g_spEnv;

  //GIP(IScdSpDB) g_spSpDB;
  IScdSpDBPtr g_spSpDB;

  //IScdSpModelPtr g_spSpMdl;
  //
  //IScdSpModelPtr g_spSpMdl1;
  //IDemoSMPtr g_spUSpMdl;
  //
  ////IScdSpMass1Ptr g_spMass1;
  ////IScdSpMass1Ptr g_spUMass1;
  //
  //IScdSpPropsPtr g_spProps;
  //IScdSpPropsPtr g_spUProps;
  //
  //IScdSpAluminaPropsPtr g_spAProps;
  //IScdSpAluminaPropsPtr g_spUAProps;

  // methods
  void LogIt(char * fmt, ...);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCDMDLTESTDLG_H__A128C706_F961_47C5_BAD1_7D9A752D7C88__INCLUDED_)
