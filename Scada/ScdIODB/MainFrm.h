//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  MainFrm.h
//  $Author    :  Jim Hansen
//
//  Subsystem  :
//
//  Description:  The main frame window containing a splitter and
//                two views: a tree of devices and groups, and a list of tags
//
//**************************************************************************

#ifndef __MAINFRM_H__               // Only Include Once
#define __MAINFRM_H__

class CMainFrame : public CFrameWnd
{
protected: // create from serialization only
   virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
   CMainFrame();
   DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
   CSplitterWnd m_wndSplitter;

// Operations
public:

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CMainFrame)
   virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
   virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
   virtual void OnSysCommand(UINT nID, LONG lParam);
   //}}AFX_VIRTUAL

// Implementation
public:
   virtual ~CMainFrame();
#ifdef _DEBUG
   virtual void AssertValid() const;
   virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
   CStatusBar  m_wndStatusBar;
   CToolBar    m_wndToolBar;
   long m_UpdRqstCount, m_UpdRqstCountLast;

// Generated message map functions
protected:
   //{{AFX_MSG(CMainFrame)
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClose();
	//}}AFX_MSG
  afx_msg void OnUpdateallRqst();
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif