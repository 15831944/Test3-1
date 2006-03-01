//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  OPC ClientView.cpp
//  $Author    :  Jim Hansen
//
//  Description:  Standard MFC ListView that displays the items and values
//
//
//**************************************************************************
#include "stdafx.h"
#include "OPC Client.h"

#include "OPC ClientDoc.h"
#include "OPC ClientView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// OPCClientView

IMPLEMENT_DYNCREATE(OPCClientView, CListView)

BEGIN_MESSAGE_MAP(OPCClientView, CListView)
   //{{AFX_MSG_MAP(OPCClientView)
   ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemchanged)
   ON_MESSAGE(WM_OPCDATA, OnOPCDataChange)
   ON_MESSAGE(WM_OPCWRITE, OnOPCWriteError)
   ON_MESSAGE(WM_OPCSHUTDOWN, OnOPCShutdown)
   ON_WM_CONTEXTMENU()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// OPCClientView construction/destruction

OPCClientView::OPCClientView()
{
   m_dwDefaultStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT;
}

OPCClientView::~OPCClientView()
{
}

BOOL OPCClientView::PreCreateWindow(CREATESTRUCT& cs)
{
   // TODO: Modify the Window class or styles here by modifying
   //  the CREATESTRUCT cs

   return CListView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// OPCClientView drawing

void OPCClientView::OnDraw(CDC* pDC)
{
   OPCClientDoc* pDoc = GetDocument();
   ASSERT_VALID(pDoc);

   // TODO: add draw code for native data here
}

enum {COLUMN_NAME, COLUMN_VALUE, COLUMN_TIME};

//**************************************************************************
// Create column headings
void OPCClientView::OnInitialUpdate()
{
   CListView::OnInitialUpdate();

   GetDocument()->hView = GetSafeHwnd();

   CListCtrl& list = GetListCtrl();
   while( list.DeleteColumn( 0 ) )
      ;
   list.InsertColumn( COLUMN_NAME,  _T("Tag"), LVCFMT_LEFT, 220, 0 );
   list.InsertColumn( COLUMN_VALUE,  _T("Value"), LVCFMT_LEFT, 150, 1 );
#ifdef DATATIMEFORMAT
   list.InsertColumn( COLUMN_TIME,  _T("Time"), LVCFMT_LEFT, 150, 1 );
#endif // DATATIMEFORMAT
}

//**************************************************************************
void OPCClientView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
   OPCClientDoc* pDoc = GetDocument();

   CListCtrl& list = GetListCtrl();
   list.SetRedraw( FALSE );

   if( lHint==32 )   // update values only (a data advise)
      {
      CString number;
      for( int index=0; index < list.GetItemCount(); index++ )
         {
         Item* pItem = (Item*)list.GetItemData( index );
         VariantToString( pItem->value, number );
         if( (pItem->quality & OPC_QUALITY_MASK) != OPC_QUALITY_GOOD )
            {
            if( (pItem->quality & OPC_QUALITY_MASK) == OPC_QUALITY_UNCERTAIN )
               number += _T(" (uncertain)");
            else if( (pItem->quality & OPC_QUALITY_MASK) == OPC_QUALITY_BAD )
               number = _T("Bad");
            }
         list.SetItem(index,COLUMN_VALUE,LVIF_TEXT,(LPTSTR)(LPCTSTR)number,0,0,0,0);
#ifdef DATATIMEFORMAT
         if( pItem->timestamp.dwHighDateTime != 0 )
         {
            CTime time( pItem->timestamp );
            list.SetItem(index,COLUMN_TIME,LVIF_TEXT, time.Format(_T("%c")),0,0,0,0);
         }
#endif // DATATIMEFORMAT
         }
      list.SetRedraw( TRUE );
      return;
      }

   LV_ITEM lv_item;
   lv_item.mask = LVIF_TEXT | LVIF_PARAM;
   lv_item.iSubItem = COLUMN_NAME;

   list.DeleteAllItems();
   UpdateWindow();
   list.SetItemCount( pDoc->items.GetCount() );

   POSITION pos = pDoc->items.GetHeadPosition();
   for( int index=0; pos!=NULL; index++)
      {
      Item* pItem = pDoc->items.GetNext( pos );
      lv_item.iItem = index;
      lv_item.pszText = (LPTSTR)(LPCTSTR)pItem->name;
      lv_item.lParam = (LPARAM)pItem;
      int item = list.InsertItem(&lv_item);
      CString number;
      switch( pItem->value.vt )
         {
         case VT_BOOL:
            number = pItem->value.boolVal ? _T("On") : _T("Off");
            break;
      case VT_UI1:
         number.Format( _T("%hd"), pItem->value.bVal );
         break;
         case VT_I2:
            number.Format( _T("%hd"), pItem->value.iVal );
            break;
         case VT_I4:
            number.Format( _T("%d"), pItem->value.lVal );
            break;
         case VT_R8:
            number.Format( _T("%g"), pItem->value.dblVal );
            break;
         case VT_BSTR:
            number = pItem->value.bstrVal;
            break;
         default:
            number = _T("?");
         }
      if( (pItem->quality & OPC_QUALITY_MASK) != OPC_QUALITY_GOOD )
         {
         if( (pItem->quality & OPC_QUALITY_MASK) == OPC_QUALITY_UNCERTAIN )
            number += _T(" (uncertain)");
         else if( (pItem->quality & OPC_QUALITY_MASK) == OPC_QUALITY_BAD )
            number = _T("Bad");
         }
      list.SetItem(item,COLUMN_VALUE,LVIF_TEXT,(LPTSTR)(LPCTSTR)number,0,0,0,0);
      }
   list.SetItem(index, COLUMN_NAME, LVIF_STATE, NULL, 0, LVIS_SELECTED, 0, 0 );
   list.SetRedraw( TRUE );
}

/////////////////////////////////////////////////////////////////////////////
// OPCClientView diagnostics

#ifdef _DEBUG
void OPCClientView::AssertValid() const
{
   CListView::AssertValid();
}

void OPCClientView::Dump(CDumpContext& dc) const
{
   CListView::Dump(dc);
}

OPCClientDoc* OPCClientView::GetDocument() // non-debug version is inline
{
   ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(OPCClientDoc)));
   return (OPCClientDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// OPCClientView message handlers

void OPCClientView::OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult)
{
   NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
   GetDocument()->pCurrentItem = (Item*)pNMListView->lParam;

   *pResult = 0;
}

//**************************************************************************
// An asynchronous data change callback triggers this message to
// update the values in the display
//
LONG OPCClientView::OnOPCDataChange(UINT wParam,LONG lParam)
{
   OnUpdate(NULL, 32, NULL);

   return 0L;
}

//**************************************************************************
// An asynchronous write complete callback triggers this message
// if there is a message to display
//
LONG OPCClientView::OnOPCWriteError(UINT wParam,LONG lParam)
{
   GetDocument()->ReportError( _T("ASync Write: "), wParam );

   return 0L;
}

//**************************************************************************
// An asynchronous shutdown callback triggers this message
//
LONG OPCClientView::OnOPCShutdown(UINT wParam,LONG lParam)
{
   GetDocument()->Disconnect();

   CString msg;
   msg.Format(_T("Server shutting down,\n%s"), (LPCTSTR)wParam);
   CString appName((LPCSTR)AFX_IDS_APP_TITLE);
   MessageBox(msg, appName, MB_OK);

   return 0L;
}

void OPCClientView::OnContextMenu(CWnd* pWnd, CPoint point)
{
   CMenu popMenu;

   if( popMenu.LoadMenu(IDR_ITEM_MENU) )
   {
      popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
                                             point.x,point.y,
                                             AfxGetMainWnd());
   }
}
