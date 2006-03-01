//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  ListView.cpp
//  $Author    :  Jim Hansen
//
//  Description:  ListView that displays tags in report mode
//
//
//**************************************************************************
#include "stdafx.h"
#include "SysCADMarshal.h"
#include "SysCADMarshalDoc.h"

#include "ErrorView.h"
#include "Mainfrm.h"
#include "slotmngr.h"
#include "opcstuff.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
enum
  {
  COLUMN_NO,
  COLUMN_TAG,
  COLUMN_TIME,
  COLUMN_ERROR,
  };

//*******************************************************************
// CErrorView
IMPLEMENT_DYNCREATE(CErrorView, CListView)
BEGIN_MESSAGE_MAP(CErrorView, CListView)
//{{AFX_MSG_MAP(CErrorView)
  ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemchanged)
  ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
  ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
  ON_WM_CREATE()
  ON_WM_TIMER()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()
//*******************************************************************

// CErrorView construction/destruction
CErrorView::CErrorView(void)
  {
  // set the CListCtrl attributes
  m_dwDefaultStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT;

  //m_pEdit = NULL;
  m_lErrorListStats=-2;
  m_iSelect = -1;
  }

CErrorView::~CErrorView(void)
  {
  }

//*******************************************************************
BOOL CErrorView::PreCreateWindow(CREATESTRUCT &cs)
  {
  // TODO: Modify the Window class or styles here by modifying
  //  the CREATESTRUCT cs
  return CListView::PreCreateWindow(cs);
  }

//*******************************************************************
int CErrorView::OnCreate(LPCREATESTRUCT lpCreateStruct)
  {
  if(CListView::OnCreate(lpCreateStruct) == -1) return -1;

  // Create the Image List
  m_ctlImage.Create(IDB_IMAGELIST, 16, 0, RGB(255, 0, 255));
  m_ctlImage.SetBkColor(GetSysColor(COLOR_WINDOW));

  /// Attach image list to List
  CListCtrl &list = GetListCtrl();
  list.SetImageList(&m_ctlImage, LVSIL_SMALL);

  CSysCADMarshalDoc *pDoc = GetDocument();
  //pDoc->SetTagView(this);

  list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

  return 0;
  }

//*******************************************************************

// CErrorView drawing
void CErrorView::OnDraw(CDC *pDC)
  {
  CSysCADMarshalDoc *pDoc = GetDocument();
  ASSERT_VALID(pDoc);

  // TODO: add draw code for native data here
  }

//BOOL  initialized = FALSE;

//*******************************************************************
void CErrorView::OnInitialUpdate(void)
  {
  CRect Rct;
  GetClientRect(&Rct);
  long W=Rct.Width();

  CListCtrl &list = GetListCtrl();
  while(list.DeleteColumn(0)) {};

  long w0=(W*4)/80;
  long w1=(W*15)/80;
  long w2=(W*6)/80;
  long w3=W-(w0+w1+w2);

  list.InsertColumn(COLUMN_NO, _T("No"), LVCFMT_RIGHT, w0, COLUMN_NO);
  list.InsertColumn(COLUMN_TAG, _T("Tag"), LVCFMT_LEFT, w1, COLUMN_TAG);
  list.InsertColumn(COLUMN_TIME, _T("Time"), LVCFMT_RIGHT, w2, COLUMN_TIME);
  list.InsertColumn(COLUMN_ERROR, _T("Error"), LVCFMT_LEFT, w3, COLUMN_ERROR);

  //   VERIFY( DDE_ID == SetTimer( DDE_ID, 100, NULL ) );
  CListView::OnInitialUpdate();
  }

//*******************************************************************
// CErrorView diagnostics
#ifdef _DEBUG
void CErrorView::AssertValid(void) const
  {
  CListView::AssertValid();
  }

void CErrorView::Dump(CDumpContext &dc) const
  {
  CListView::Dump(dc);
  }

CSysCADMarshalDoc *CErrorView::GetDocument(void)  // non-debug version is inline
  {
  ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSysCADMarshalDoc)));
  return (CSysCADMarshalDoc *) m_pDocument;
  }
#endif //_DEBUG

//*******************************************************************

// CErrorView message handlers
void CErrorView::OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint)
  {
  //CListView::OnUpdate(pSender, lHint, pHint);

  CSysCADMarshalDoc *pDoc = GetDocument();
  CListCtrl     &list = GetListCtrl();

  long Hint=TheHint(lHint);
  long Index=HintIndex(lHint);

  if ((Hint == HINT_CLEAR_ALL ))// || pDoc->pCurrent==NULL)
    {
    list.DeleteAllItems();
    return;
    }

  if (Hint==HINT_ERROR_ADDED)
    {
    LV_ITEM lv_item;
    lv_item.mask = LVIF_TEXT;// | LVIF_IMAGE;
    lv_item.iImage = -1;
    lv_item.iSubItem = COLUMN_NO;

    //m_lErrorListStats = pDoc->m_ErrorList.Stats();
    //
    //list.DeleteAllItems();
    //UpdateWindow();

    CErrorItem *pErr=pDoc->m_ErrorList.Head();
    CString N,V;
    long lItem=-1;
    for(long i=0; pErr!=NULL; pErr=pErr->m_pNext, i++)
      {
      if (i<Index)
        continue;
      N.Format("%i", i);//pErr->.m_lIdNo);
      lv_item.iItem = i;
      lv_item.pszText = N.GetBuffer(0);

      lItem = list.InsertItem(&lv_item);
      list.SetItem(lItem ,COLUMN_TAG,LVIF_TEXT,pErr->m_sTag,0,0,0,0);
      TimeStampToString(pErr->m_ftTimeStamp, V, NULL);
      list.SetItem(lItem ,COLUMN_TIME,LVIF_TEXT,V,0,0,0,0);
      list.SetItem(lItem ,COLUMN_ERROR,LVIF_TEXT,pErr->m_sError,0,0,0,0);
      }
    if (lItem>=0)
      list.EnsureVisible(lItem , false);
    }
  }

//*******************************************************************
void CErrorView::OnDblclk(NMHDR *pNMHDR, LRESULT *pResult)
  {
  *pResult = 0;
  }

//*******************************************************************

void CErrorView::OnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
  {
  NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
  *pResult = 0;
  }

//*******************************************************************

// reverse the order of sorting if the same col is clicked again
void CErrorView::OnColumnclick(NMHDR *pNMHDR, LRESULT *pResult)
  {
  *pResult = 0;
  }

//*******************************************************************
void CErrorView::OnTimer(UINT nIDEvent)
  {
  }

