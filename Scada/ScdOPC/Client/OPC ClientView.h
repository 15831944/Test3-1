//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  OPC ClientView.h
//  $Author    :  Jim Hansen
//
//  Description:  Standard MFC ListView that displays the items and values
//
//
//**************************************************************************

class OPCClientView : public CListView
{
protected: // create from serialization only
   OPCClientView();
   DECLARE_DYNCREATE(OPCClientView)

// Attributes
public:
   OPCClientDoc* GetDocument();

// Operations
public:

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(OPCClientView)
   public:
   virtual void OnDraw(CDC* pDC);  // overridden to draw this view
   virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
   protected:
   virtual void OnInitialUpdate(); // called first time after construct
   virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
   //}}AFX_VIRTUAL

// Implementation
public:
   virtual ~OPCClientView();
#ifdef _DEBUG
   virtual void AssertValid() const;
   virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
   //{{AFX_MSG(OPCClientView)
   afx_msg void OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg LONG OnOPCDataChange(UINT wParam,LONG lParam);
   afx_msg LONG OnOPCWriteError(UINT wParam,LONG lParam);
   afx_msg LONG OnOPCShutdown(UINT wParam,LONG lParam);
   afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in OPC ClientView.cpp
inline OPCClientDoc* OPCClientView::GetDocument()
   { return (OPCClientDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
