//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  TreeView.h
//  $Author    :  Jim Hansen
//
//  Subsystem  :
//
//  Description:  Devices and Groups are displayed in this tree view
//
//
//**************************************************************************

#ifndef __TREEVIEW_H__              // Only Include Once
#define __TREEVIEW_H__

class CDeviceView : public CTreeView
{
protected: // create from serialization only
   CDeviceView();
   DECLARE_DYNCREATE(CDeviceView)

// Attributes
public:
   CDoc* GetDocument();

// Operations
public:

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CDeviceView)
   public:
   virtual void OnDraw(CDC* pDC);  // overridden to draw this view
   virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
   virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
   virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
   virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
   virtual void OnDragLeave();
   protected:
   virtual void OnInitialUpdate(); // called first time after construct
   virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
   //}}AFX_VIRTUAL

// Implementation
public:
   virtual ~CDeviceView();
#ifdef _DEBUG
   virtual void AssertValid() const;
   virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    CImageList m_ctlImage;
    COleDropTarget m_dropTarget;
    BOOL       clipFormatAvailable; // only valid during drag n drop
    BOOL       deleting;

// Generated message map functions
protected:
   //{{AFX_MSG(CDeviceView)
   afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnEditProperties();
   afx_msg void OnUpdateEditProperties(CCmdUI* pCmdUI);
   afx_msg void OnKeydown(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnEditDelete();
   afx_msg void OnUpdateEditDelete(CCmdUI* pCmdUI);
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg void OnUpdateSummary(CCmdUI* pCmdUI);
// afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in TreeView.cpp
inline CDoc* CDeviceView::GetDocument()
   { return (CDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
#endif