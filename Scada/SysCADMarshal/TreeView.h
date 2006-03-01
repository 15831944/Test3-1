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
#ifndef __TREEVIEW_H__  // Only Include Once
#define __TREEVIEW_H__

class CSysCADMarshalDoc;
class CDeviceView :
  public CTreeView
  {
protected:              // create from serialization only
  CDeviceView(void);
  DECLARE_DYNCREATE(CDeviceView)
// Attributes
public:
  CSysCADMarshalDoc *GetDocument(void);

// Operations
public:
  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CDeviceView)
public:
  virtual void  OnDraw(CDC *pDC);       // overridden to draw this view
  virtual BOOL  PreCreateWindow(CREATESTRUCT &cs);
protected:
  virtual void  OnInitialUpdate(void);  // called first time after construct
  virtual void  OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint);
  //}}AFX_VIRTUAL
// Implementation
public:
  virtual       ~CDeviceView(void);
#ifdef _DEBUG
  virtual void  AssertValid(void) const;
  virtual void  Dump(CDumpContext &dc) const;
#endif
protected:
  CImageList    m_ctlImage;

  //COleDropTarget m_dropTarget;
  //BOOL       clipFormatAvailable; // only valid during drag n drop
  bool        m_Deleting;
  HTREEITEM   m_hLinks;
  HTREEITEM   m_hSlots;
  HTREEITEM   m_hEdits;
  HTREEITEM   m_hCdBlks;
  HTREEITEM   m_hChanges;

// Generated message map functions
protected:
  //{{AFX_MSG(CDeviceView)
  afx_msg void  OnSelchanged(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void  OnKeydown(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg int   OnCreate(LPCREATESTRUCT lpCreateStruct);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
  };

#ifndef _DEBUG  // debug version in TreeView.cpp
inline CSysCADMarshalDoc *CDeviceView::GetDocument(void)
  {
  return (CSysCADMarshalDoc *) m_pDocument;
  }
#endif

/////////////////////////////////////////////////////////////////////////////
#endif
