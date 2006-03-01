//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  TreeView.cpp
//  $Author    :  Jim Hansen
//
//  Description:  A tree view to display devices and groups
//
//
//**************************************************************************
#include "stdafx.h"
#include "SysCADMarshal.h"
#include "SysCADMarshalDoc.h"

#include "TreeView.h"
#include "Mainfrm.h"
#include "slotmngr.h"

//#include "DeviceDialog.h"
//#include "GroupDialog.h"
//#include "CopyTagDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//-------------------------------------------------------------------

// CDeviceView
IMPLEMENT_DYNCREATE(CDeviceView, CTreeView)
BEGIN_MESSAGE_MAP(CDeviceView, CTreeView)
//{{AFX_MSG_MAP(CDeviceView)
  ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
  ON_NOTIFY_REFLECT(TVN_KEYDOWN, OnKeydown)
  ON_WM_CREATE()
// ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBegindrag)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()
//*******************************************************************

// CDeviceView construction/destruction
CDeviceView::CDeviceView(void)
  {
  // set the CTreeCtrl attributes
  m_dwDefaultStyle = WS_CHILD |
    WS_VISIBLE |
    WS_BORDER |
    TVS_HASLINES |
    TVS_HASBUTTONS |
    TVS_LINESATROOT |
    TVS_SHOWSELALWAYS;

  m_Deleting = false;
  m_hLinks = NULL;
  m_hSlots = NULL;
  m_hEdits = NULL;
  m_hCdBlks = NULL;
  m_hChanges = NULL;
  }

CDeviceView::~CDeviceView(void)
  {
  }

//-------------------------------------------------------------------

BOOL CDeviceView::PreCreateWindow(CREATESTRUCT &cs)
  {
  // TODO: Modify the Window class or styles here by modifying
  //  the CREATESTRUCT cs
  return CTreeView::PreCreateWindow(cs);
  }

int CDeviceView::OnCreate(LPCREATESTRUCT lpCreateStruct)
  {
  if(CTreeView::OnCreate(lpCreateStruct) == -1) return -1;

  // Create the Image List
  VERIFY(m_ctlImage.Create(IDB_IMAGELIST, 16, 0, RGB(255, 0, 255)));
  m_ctlImage.SetBkColor(GetSysColor(COLOR_WINDOW));

  /// Attach image list to Tree
  CTreeCtrl &ctlTree = GetTreeCtrl();
  ctlTree.SetImageList(&m_ctlImage, TVSIL_NORMAL);

  return 0;
  }

//-------------------------------------------------------------------

// CDeviceView drawing
void CDeviceView::OnDraw(CDC *pDC)
  {
  CSysCADMarshalDoc *pDoc = GetDocument();
  ASSERT_VALID(pDoc);
  }

void CDeviceView::OnInitialUpdate(void)
  {
  CTreeView::OnInitialUpdate();
  }

//-------------------------------------------------------------------

// CDeviceView diagnostics
#ifdef _DEBUG
void CDeviceView::AssertValid(void) const
  {
  CTreeView::AssertValid();
  }

void CDeviceView::Dump(CDumpContext &dc) const
  {
  CTreeView::Dump(dc);
  }

CSysCADMarshalDoc *CDeviceView::GetDocument(void) // non-debug version is inline
  {
  ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSysCADMarshalDoc)));
  return (CSysCADMarshalDoc *) m_pDocument;
  }
#endif //_DEBUG

//-------------------------------------------------------------------

// CDeviceView message handlers
void CDeviceView::OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint)
  {
  CTreeView::OnUpdate(pSender, lHint, pHint);

  long Hint=TheHint(lHint);
  long HIndex=HintIndex(lHint);

  switch ( Hint)
    {
    case HINT_UPDATE_TAGS:
    case HINT_UPDATE_TAG_VALUES:
      return;
    case HINT_CHANGE_VIEW:
      return;
    }

  CSysCADMarshalDoc* pDoc = GetDocument();
  CTreeCtrl& tree = GetTreeCtrl();
  CString V1,V2,V3,V4,V5;
  switch( Hint )
    {
    case HINT_CLEAR_ALL:
      m_Deleting = true;
      tree.DeleteAllItems();
      m_Deleting = false;
      break;
    case HINT_UPDATE_CFG:
      {
      m_Deleting = true;
      tree.DeleteAllItems();
      m_Deleting = false;

      m_hLinks=tree.InsertItem( "Links", 2, 3 );
      tree.SetItemData(m_hLinks, ItemData(ViewLinks, -1, -1));
      HTREEITEM hin=tree.InsertItem( "Gets", 2, 3, m_hLinks);
      tree.SetItemData(hin, ItemData(ViewLinks, 0, -1));
      HTREEITEM hout=tree.InsertItem( "Sets", 2, 3, m_hLinks);
      tree.SetItemData(hout, ItemData(ViewLinks, 1, -1));

      m_hSlots=tree.InsertItem( "Slots", 2, 3 );
      tree.SetItemData(m_hSlots, ItemData(ViewSlots, -1, -1));
      for (int i=0; i<pDoc->m_DeviceCfgs.GetSize(); i++)
        {
        HTREEITEM h, hDev=tree.InsertItem(pDoc->m_DeviceCfgs[i]->m_sServerName, 2, 3, m_hSlots);
        tree.SetItemData(hDev, ItemData(ViewSlots, i, -1));

        if (!pDoc->m_DeviceCfgs[i]->m_bLocalOnly)
          {
          V1="ProgID:"+pDoc->m_DeviceCfgs[i]->m_sProgID;
          V2="Node:"+pDoc->m_DeviceCfgs[i]->m_sNode;
          V3="Group:"+pDoc->m_DeviceCfgs[i]->m_sGroupName;
          V4.Format("Update:%i", pDoc->m_DeviceCfgs[i]->m_dwUpdateRate);
          V5.Format("TrickleCount:%i", pDoc->m_DeviceCfgs[i]->m_dwTrickleCount);

          h=tree.InsertItem(V1, 0, 0, hDev); tree.SetItemData(h, ItemData(ViewSlots, i, -1));
          h=tree.InsertItem(V2, 0, 0, hDev); tree.SetItemData(h, ItemData(ViewSlots, i, -1));
          h=tree.InsertItem(V3, 0, 0, hDev); tree.SetItemData(h, ItemData(ViewSlots, i, -1));
          h=tree.InsertItem(V4, 0, 0, hDev); tree.SetItemData(h, ItemData(ViewSlots, i, -1));
          h=tree.InsertItem(V5, 0, 0, hDev); tree.SetItemData(h, ItemData(ViewSlots, i, -1));
          }
        else
          {
          V1="No Connection";

          h=tree.InsertItem(V1, 0, 0, hDev); tree.SetItemData(h, ItemData(ViewSlots, i, -1));
          }
        }

      if (pDoc->m_SlotEdits.GetSize()>0)
        {
        m_hEdits=tree.InsertItem( "Edits", 2, 3 );
        tree.SetItemData(m_hEdits, ItemData(ViewEdits, -1, -1));
        }

      m_hCdBlks=tree.InsertItem( "CodeBlks", 2, 3 );
      tree.SetItemData(m_hCdBlks, ItemData(ViewCdBlks, -1, -1));

      m_hChanges=tree.InsertItem( "ChangeLog", 2, 3 );
      tree.SetItemData(m_hChanges, ItemData(ViewChanges, -1, -1));
      }

      break;
    case HINT_UPDATE_ONESLOTVALUE: 
    case HINT_UPDATE_ONELINKVALUE: 
    case HINT_UPDATE_CHANGES: 
    case HINT_UPDATE_TRACKING: 
    case HINT_UPDATE_CHGLOG: 
    case HINT_ERROR_ADDED:
     break;

    default: ASSERT( false );
    }

  }

//-------------------------------------------------------------------

// store the selected device/group pointer in the doc
void CDeviceView::OnSelchanged(NMHDR *pNMHDR, LRESULT *pResult)
  {
  if( m_Deleting ) // items are not valid during deletion
    return;

  CSysCADMarshalDoc* pDoc = GetDocument();
  CTreeCtrl& tree = GetTreeCtrl();
  HTREEITEM item = tree.GetSelectedItem( );
  long Data=tree.GetItemData(item);

  pDoc->m_lView=(eViews)ExtractWhat(Data);
  pDoc->m_lDeviceSelect=ExtractSelect(Data);
  pDoc->m_lMarkedItem=ExtractItem(Data);

  pDoc->UpdateAllViews(this, MakeHint(HINT_CHANGE_VIEW));
  pDoc->UpdateAllViews(this, MakeHint(HINT_UPDATE_TAGS));

  *pResult = 0;
  }

//-------------------------------------------------------------------

// Just handle delete key
void CDeviceView::OnKeydown(NMHDR *pNMHDR, LRESULT *pResult)
  {
  *pResult = 0;

  //if( GetDocument()->pCurrent==NULL )
  //   {
  //   return;
  //   }
  //
  //TV_KEYDOWN* pKeyDown = (TV_KEYDOWN*)pNMHDR;
  //
  //if (pKeyDown->wVKey == VK_DELETE)
  //   {
  //   OnEditDelete();
  //   }
  }

//-------------------------------------------------------------------
