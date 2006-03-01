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
#ifndef __ERRORVIEW_H__  // Only Include Once
#define __ERRORVIEW_H__

class CErrorView;
class CSysCADMarshalDoc;

class CErrorView :
  public CListView
  {
  friend class  CValueEdit;
protected:              // create from serialization only
  CErrorView(void);
  DECLARE_DYNCREATE(CErrorView)
// Attributes
public:
  CSysCADMarshalDoc *GetDocument(void);

// Operations
public:
  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CErrorView)
public:
  virtual void  OnDraw(CDC *pDC);       // overridden to draw this view
  virtual BOOL  PreCreateWindow(CREATESTRUCT &cs);
protected:
  virtual void  OnInitialUpdate(void);  // called first time after construct
  virtual void  OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint);
  //  virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
  //}}AFX_VIRTUAL
// Implementation
public:
  virtual       ~CErrorView(void);
#ifdef _DEBUG
  virtual void  AssertValid(void) const;
  virtual void  Dump(CDumpContext &dc) const;
#endif
protected:
  CImageList    m_ctlImage;

  //OModbusTag* m_pTag;
  //RECT          m_Rct;
  //CValueEdit    *m_pEdit;
  long          m_lErrorListStats;
  long          m_iSelect;

  //void          FixColumnWidths();

  //long TmpColWidth[20];
  //long ColWidth[20];

// Generated message map functions
protected:
  //{{AFX_MSG(CErrorView)
  afx_msg void  OnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void  OnDblclk(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void  OnColumnclick(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg int   OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void  OnTimer(UINT nIDEvent);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
  };

#ifndef _DEBUG  // debug version in TreeView.cpp
inline CSysCADMarshalDoc *CErrorView::GetDocument(void)
  {
  return (CSysCADMarshalDoc *) m_pDocument;
  }
#endif

/////////////////////////////////////////////////////////////////////////////
#endif
