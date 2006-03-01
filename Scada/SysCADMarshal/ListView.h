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
#ifndef __LISTVIEW_H__  // Only Include Once
#define __LISTVIEW_H__

class CTagView;
class CDspSlot;
class CLink;
class CSysCADMarshalDoc;
class CFullValue;

const long MaxColumns = 26;

#include "slotcommon.h"
#include "slot.h"
#include "link.h"
#include "slotcfgdlg.h"

class CValueEdit :
  public CEdit
  {
  public:
    virtual       ~CValueEdit(void);
    CTagView      *m_pEdtCtrlView;
    CSysCADMarshalDoc *m_pDoc;
    static CValueEdit * Start(CFullValue & Value, eConnSrcDst Dst, long Item, RECT & Rct, CTagView *pTagView);

    afx_msg void  OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void  OnDestroy(void);
    afx_msg void  OnKillFocus(CWnd *pNewWnd);
    DECLARE_MESSAGE_MAP()
  };

class CTagView :
  public CListView
  {
  friend class  CValueEdit;
  protected:              // create from serialization only
    CTagView(void);
    DECLARE_DYNCREATE(CTagView)
  // Attributes
  public:
    CSysCADMarshalDoc *GetDocument(void);

  // Operations
  public:
    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CTagView)
	public:
    virtual BOOL  PreCreateWindow(CREATESTRUCT &cs);
	protected:
    virtual void  OnInitialUpdate(void);  // called first time after construct
    virtual void  OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint);
	//}}AFX_VIRTUAL
  // Implementation
  public:
    virtual       ~CTagView(void);
  #ifdef _DEBUG
    virtual void  AssertValid(void) const;
    virtual void  Dump(CDumpContext &dc) const;
  #endif
  protected:
    CImageList    m_ctlImage;

    CRect         m_Rct;
    CValueEdit    *m_pEdit;
    CSlotCfgDlg   *m_pCfgDlg;

    void          FixColumnWidths();
    void          FixVisibleItems();

    long          InsertItem(CSlotCfg *pSlot);
    void          SetRuntimeValues(long ListIndex, CSlotCfg * pSlot);
    long          InsertItem(CLinkCfg * pLink);
    void          SetRuntimeValues(long ListIndex, CLinkCfg * pLink);
    long          InsertItem(CCdBlkCfg * pCdBlk);
    long          InsertItem(CChangeItem *pChg);
    long          InsertItem(CSlotCfgStrings *pSlot);

    void          SetVisibleSlots(long Slot1, long Number);
    void          SetVisibleLinks(long Link1, long Number);

    //long          TmpColWidth[MaxColumns];
    //long          ColWidth[MaxColumns];
    void          KeepColumnWidths();
    eViews        m_ColView;


    long          m_lColumn_NO;
    long          m_lColumn_TYPE;
    long          m_lColumn_TAG;
    long          m_lColumn_SIMTAG;
    long          m_lColumn_SIMVAL;
    long          m_lColumn_DEVTAG;
    long          m_lColumn_DEVVAL;
    long          m_lColumn_SRC;
    long          m_lColumn_DST;
    long          m_lColumn_CHGVAL;
    long          m_lColumn_TIME;
    long          m_lColumn_DTIME;
    long          m_lColumn_DELAY;
    long          m_lColumn_CHGCNT;
    long          m_lColumn_IGNORE;
    long          m_lColumn_TRANS;
    long          m_lColumn_ERROR;
    long          m_lColumn_DESC;
    long          m_lColumn_CONN;
    long          m_lColumn_SCHED;
    long          m_lColumn_CDBLK;

    long          m_lColumn_sCON; 
    long          m_lColumn_sTYP; 
    long          m_lColumn_sDRV; 
    long          m_lColumn_sMOD; 
    long          m_lColumn_sRNG; 


    void          SetColumns(bool Initial, eViews What);
    //long *        ColumnWidthArray(long What);

    long          m_lLastSlotTop;
    long          m_lLastLinkTop;
    long          m_lLastCdBlkTop;
  // Generated message map functions
  protected:
    //{{AFX_MSG(CTagView)
    afx_msg void  OnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void  OnDblclk(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void  OnColumnclick(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg int   OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void  OnTimer(UINT nIDEvent);
	  afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	  afx_msg void OnSize(UINT nType, int cx, int cy);
	  afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
    afx_msg void  OnDestroy(void);
    DECLARE_MESSAGE_MAP()
  };

#ifndef _DEBUG  // debug version in TreeView.cpp
inline CSysCADMarshalDoc *CTagView::GetDocument(void)
  {
  return (CSysCADMarshalDoc *) m_pDocument;
  }
#endif

/////////////////////////////////////////////////////////////////////////////
#endif
