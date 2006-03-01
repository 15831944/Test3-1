// MainFrm.h : interface of the CMainFrame class
//
///////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_MAINFRM_H__B4276B50_BCC6_4889_B3F7_78D59CB967F5__INCLUDED_)
#define AFX_MAINFRM_H__B4276B50_BCC6_4889_B3F7_78D59CB967F5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMainFrame :
  public CFrameWnd
  {
  protected:  // create from serialization only
    CMainFrame(void);
    DECLARE_DYNCREATE(CMainFrame)
  // Attributes
  public:
  // Operations
  public:
    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CMainFrame)
  public:
    virtual BOOL  PreCreateWindow(CREATESTRUCT &cs);
  protected:
    virtual BOOL  OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext *pContext);
    //}}AFX_VIRTUAL
  // Implementation
  public:
    virtual       ~CMainFrame(void);
  #ifdef _DEBUG
    virtual void  AssertValid(void) const;
    virtual void  Dump(CDumpContext &dc) const;
  #endif

  // Attributes
  public:
    CSplitterWnd  m_wndSplitter;
    CSplitterWnd  m_wndSplitter2;
    CStatusBar    m_wndStatusBar;
    CToolBar      m_wndToolBar;
  protected:  // control bar embedded members

    // Testing
  
    void DoTestLoadpreset(long Start, long End, long Modulo);

  // Generated message map functions
  protected:
    //{{AFX_MSG(CMainFrame)
    afx_msg int   OnCreate(LPCREATESTRUCT lpCreateStruct);
	  afx_msg void OnConfigurationLoaddb();
	  afx_msg void OnUpdateConfigurationLoaddb(CCmdUI* pCmdUI);
	  afx_msg void OnConfigurationAdddb();
	  afx_msg void OnUpdateConfigurationAdddb(CCmdUI* pCmdUI);
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  };

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_MAINFRM_H__B4276B50_BCC6_4889_B3F7_78D59CB967F5__INCLUDED_)
