//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  ListView.h
//  $Author    :  Jim Hansen
//
//  Subsystem  :
//
//  Description:  Tags are displayed in this list view.
//
//
//**************************************************************************

#ifndef __LISTVIEW_H__              // Only Include Once
#define __LISTVIEW_H__

class CTagView;
class CValueEdit : public CEdit
  {
  public:
    virtual ~CValueEdit();
    CTagView *m_pEdtCtrlView;

    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnDestroy();
    afx_msg void OnKillFocus( CWnd* pNewWnd );
    DECLARE_MESSAGE_MAP()
  };

class CTagView : public CListView
{
friend class CValueEdit;
protected: // create from serialization only
   CTagView();
   DECLARE_DYNCREATE(CTagView)

// Attributes
public:
   CDoc* GetDocument();

// Operations
public:

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CTagView)
	public:
   virtual void OnDraw(CDC* pDC);  // overridden to draw this view
   virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
   virtual void OnInitialUpdate(); // called first time after construct
   virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
//	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
public:
   virtual ~CTagView();
#ifdef _DEBUG
   virtual void AssertValid() const;
   virtual void Dump(CDumpContext& dc) const;
#endif

protected:
   CImageList  m_ctlImage;
   OModbusTag* m_pTag;
   RECT        m_Rct;
   CValueEdit* m_pEdit;

// Generated message map functions
protected:
   //{{AFX_MSG(CTagView)
   afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnEditProperties();
   afx_msg void OnUpdateEditProperties(CCmdUI* pCmdUI);
   afx_msg void OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnEditDelete();
   afx_msg void OnUpdateEditDelete(CCmdUI* pCmdUI);
   afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnEditCut();
   afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
   afx_msg void OnEditCopy();
   afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in TreeView.cpp
inline CDoc* CTagView::GetDocument()
   { return (CDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
#endif
