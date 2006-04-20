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
#include "ScdIODB.h"

#include "ListView.h"
#include "Mainfrm.h"
#include "TagDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

enum Column_Ids{
  eCOLUMN_NO, 
  eCOLUMN_NAME, 
  eCOLUMN_TYPE, 
  // eCOLUMN_LOC, 
  // eCOLUMN_PROCESSING,
  eCOLUMN_VALUE, 
  //eCOLUMN_QUALITY, 
  eCOLUMN_DESC
  };
LPARAM      lastCol = 555;

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CValueEdit, CEdit)
  ON_WM_CHAR()
  ON_WM_KEYDOWN()
  ON_WM_SETFOCUS()
  ON_WM_KILLFOCUS()
  ON_WM_DESTROY()
//  ON_COMMAND(ID_EDIT_PASTE, OnPaste)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

CValueEdit::~CValueEdit()
  {
  TRACE("x");
  }

//---------------------------------------------------------------------------

void CValueEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
  {
  switch (nChar)
    {
    case VK_RETURN  :
      if (m_pEdtCtrlView)
        m_pEdtCtrlView->SetFocus();
      break;
    case VK_ESCAPE  :
      if (m_pEdtCtrlView)
        m_pEdtCtrlView->SetFocus();
      break;
    default         :
      CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
      break;
    }
  }

//---------------------------------------------------------------------------

void CValueEdit::OnDestroy( )
  {

  CString T;
  GetWindowText(T);
  double f=atof(T);

  OModbusTag* pTag=m_pEdtCtrlView->m_pTag;

  switch( pTag->value.vt )
    {
    case VT_EMPTY:
    case VT_NULL:
      pTag->value.vt = VT_R8;
      pTag->value.dblVal = 0.0;
      break;
    case VT_BOOL:
      pTag->value.boolVal = f!=0.0;
      break;
    case VT_UI1: // uchar
      pTag->value.bVal = (unsigned char)f;
      break;
    case VT_UI2 :   // short
      pTag->value.uiVal = (WORD)f;
      break;
    case VT_UI4 :  // long
      pTag->value.ulVal = (DWORD)f; 
      break;
    case VT_I1: // uchar
      pTag->value.cVal = (char)f;
    case VT_I2 :   // short
      pTag->value.iVal = (short)f;
      break;
    case VT_I4 :  // long
      pTag->value.lVal = (long)f; 
      break;
    case VT_R4 :
      pTag->value.fltVal = (float)f;
      break;
    case VT_R8 :
      pTag->value.dblVal = f; 
      break;
    default: ASSERT( FALSE );
    }
  CoFileTimeNow( &pTag->timestamp );
  pTag->quality=OPC_QUALITY_GOOD;
    
  m_pEdtCtrlView->m_pEdit=NULL;
    //pEdtControl=NULL;
  CEdit::OnDestroy();
  }

//---------------------------------------------------------------------------

void CValueEdit::OnKillFocus( CWnd* pNewWnd )
  {
  CEdit::OnKillFocus( pNewWnd );
  DestroyWindow();
  };

//*******************************************************************
// CTagView

IMPLEMENT_DYNCREATE(CTagView, CListView)

BEGIN_MESSAGE_MAP(CTagView, CListView)
   //{{AFX_MSG_MAP(CTagView)
   ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
   ON_COMMAND(ID_EDIT_PROPERTIES, OnEditProperties)
   ON_UPDATE_COMMAND_UI(ID_EDIT_PROPERTIES, OnUpdateEditProperties)
   ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemchanged)
   ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
   ON_COMMAND(ID_EDIT_DELETE, OnEditDelete)
   ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, OnUpdateEditDelete)
   ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
   ON_NOTIFY_REFLECT(LVN_BEGINDRAG, OnBegindrag)
   ON_COMMAND(ID_EDIT_CUT, OnEditCut)
   ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
   ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
   ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
   ON_WM_CREATE()
   ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//*******************************************************************
// CTagView construction/destruction

CTagView::CTagView()
   {
   // set the CListCtrl attributes
   m_dwDefaultStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT;
   m_pTag = NULL;
   lastCol = 555;
   m_pEdit =  NULL;
   }

CTagView::~CTagView()
   {
   }

//*******************************************************************
BOOL CTagView::PreCreateWindow(CREATESTRUCT& cs)
   {
   // TODO: Modify the Window class or styles here by modifying
   //  the CREATESTRUCT cs

   return CListView::PreCreateWindow(cs);
   }

//*******************************************************************
int CTagView::OnCreate(LPCREATESTRUCT lpCreateStruct)
   {
   if (CListView::OnCreate(lpCreateStruct) == -1)
      return -1;

   // Create the Image List
   m_ctlImage.Create(IDB_IMAGELIST,16,0,RGB(255,0,255));
   m_ctlImage.SetBkColor(GetSysColor(COLOR_WINDOW));

   /// Attach image list to List
   CListCtrl& list = GetListCtrl();
   list.SetImageList(&m_ctlImage, LVSIL_SMALL);

   CDoc* pDoc = GetDocument();
   pDoc->SetTagView( this );

   list.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

   return 0;
   }

//*******************************************************************
// CTagView drawing

void CTagView::OnDraw(CDC* pDC)
   {
   CDoc* pDoc = GetDocument();
   ASSERT_VALID(pDoc);

   // TODO: add draw code for native data here
   }

BOOL initialized = FALSE;
//*******************************************************************
void CTagView::OnInitialUpdate()
   {
   if( initialized )
      return;

   initialized = TRUE;
   CListCtrl& list = GetListCtrl();
   while( list.DeleteColumn( 0 ) )
      ;
   list.InsertColumn( eCOLUMN_NO,    _T("No"),           LVCFMT_RIGHT, 40, eCOLUMN_NO);  
   list.InsertColumn( eCOLUMN_NAME,  _T("Name"),         LVCFMT_LEFT, 110, eCOLUMN_NAME);
   list.InsertColumn( eCOLUMN_TYPE,  _T("Type"),         LVCFMT_LEFT,  80, eCOLUMN_TYPE);
   list.InsertColumn( eCOLUMN_VALUE, _T("Value"),        LVCFMT_RIGHT, 80, eCOLUMN_VALUE);
   list.InsertColumn( eCOLUMN_DESC,  _T("Description"),  LVCFMT_LEFT, 120, eCOLUMN_DESC);

   VERIFY( DDE_ID == SetTimer( DDE_ID, 100, NULL ) );

   CListView::OnInitialUpdate();
   }

//*******************************************************************
// CTagView diagnostics

#ifdef _DEBUG
void CTagView::AssertValid() const
   {
   CListView::AssertValid();
   }

void CTagView::Dump(CDumpContext& dc) const
   {
   CListView::Dump(dc);
   }

CDoc* CTagView::GetDocument() // non-debug version is inline
   {
   ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDoc)));
   return (CDoc*)m_pDocument;
   }
#endif //_DEBUG

//*******************************************************************
// CTagView message handlers
void CTagView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
   {
   CListView::OnUpdate(pSender, lHint, pHint);
   CDoc* pDoc = GetDocument();

   CListCtrl& list = GetListCtrl();
   if ((lHint == CLEAR_ALL ) || pDoc->pCurrent==NULL)
   {
      list.DeleteAllItems();
      return;
   }

   // These don't apply here
   if( lHint==UPDATE_DEVICES || lHint==ADD_DEVICE)
      return;

   LV_ITEM lv_item;
   lv_item.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
   lv_item.iImage = 5;
   lv_item.iSubItem = eCOLUMN_NO;

   // if pHint, then this one tag needs updating
   if( pHint != NULL )
      {
      ASSERT_KINDOF( OModbusTag, pHint );
      OModbusTag* pTag = (OModbusTag*)pHint;
      LV_FINDINFO fi;
      fi.flags = LVFI_PARAM;
      fi.lParam =  (LPARAM)pHint;
      int foundItem = list.FindItem( &fi );
      if( foundItem != -1 )
         {
         CString N;
         N.Format("%i", pTag->idno);
         lv_item.iItem = foundItem;
         lv_item.pszText = N.GetBuffer(0);
         lv_item.lParam = (LPARAM)pTag;
         if( pTag->value.vt == VT_BOOL)
//           >range == OModbusTag::INPUT_BOOL
//         || pTag->range == OModbusTag::OUTPUT_BOOL )
            lv_item.iImage = 8;
         else if( pTag->value.vt == VT_R4 || pTag->value.vt == VT_R8)
            lv_item.iImage = 6;
         else
            lv_item.iImage = 7;
         list.SetItem(&lv_item);
         list.SetItem(foundItem,eCOLUMN_NAME,LVIF_TEXT,pTag->name.GetBuffer(0),0,0,0,0);
         list.SetItem(foundItem,eCOLUMN_TYPE,LVIF_TEXT,pTag->TypeString(),0,0,0,0);
         //list.SetItem(foundItem,eCOLUMN_LOC,LVIF_TEXT,pTag->LocationString().GetBuffer(0),0,0,0,0);
         //if( pTag->enableProcessing )
         //   list.SetItem(foundItem,eCOLUMN_PROCESSING,LVIF_TEXT,_T("Custom"),0,0,0,0);
         //list.SetItem(foundItem,eCOLUMN_QUALITY,LVIF_TEXT,_T(""),0,0,0,0);
         list.SetItem(foundItem,eCOLUMN_DESC,LVIF_TEXT,pTag->description,0,0,0,0);
         }
      return;
      }

   list.SetRedraw( FALSE );
   if( lHint==UPDATE_TAG_VALUES )
      {
      for( int index=0; index < list.GetItemCount(); index++ )
         {
         if (rand()>RAND_MAX/20) 
           continue;
         OModbusTag* pTag = (OModbusTag*)list.GetItemData( index );
         ASSERT_KINDOF( OModbusTag, pTag );
         CString text(_T("?"));
         switch( pTag->value.vt )
            {
            case VT_BOOL:
               text = pTag->value.boolVal ? _T("On") : _T("Off");
               break;
            case VT_I1:
               text.Format( _T("%hu"), (char)pTag->value.bVal );
               break;
            case VT_I2:
               text.Format( _T("%hu"), (short)pTag->value.iVal );
               break;
            case VT_I4:
               text.Format( _T("%lu"), (long)pTag->value.lVal );
               break;
            case VT_UI1:
               text.Format( _T("%hu"), (unsigned char)pTag->value.cVal );
               break;
            case VT_UI2:
               text.Format( _T("%hu"), (USHORT)pTag->value.uiVal );
               break;
            case VT_UI4:
               text.Format( _T("%lu"), (ULONG)pTag->value.ulVal );
               break;
            case VT_R4:
               text.Format( _T("%g"), pTag->value.fltVal );
               break;
            case VT_R8:
               text.Format( _T("%g"), pTag->value.dblVal );
               if( pTag->enableProcessing )
                  text += pTag->pProcess->EngUnits;
               break;
            case VT_BSTR:
               text = pTag->value.bstrVal;
               break;
            }
//         if( (pTag->quality & OPC_QUALITY_MASK) != OPC_QUALITY_GOOD )
//            {
//            if( (pTag->quality & OPC_QUALITY_MASK) == OPC_QUALITY_UNCERTAIN )
//               text += _T(" (uncertain)");
//            else if( (pTag->quality & OPC_QUALITY_MASK) == OPC_QUALITY_BAD )
//               text = _T("Bad");
//            }
         list.SetItem( index, eCOLUMN_VALUE, LVIF_TEXT, text.GetBuffer(0),0,0,0,0);
         text="";
         if( (pTag->quality & OPC_QUALITY_MASK) != OPC_QUALITY_GOOD )
           {
           if( (pTag->quality & OPC_QUALITY_MASK) == OPC_QUALITY_UNCERTAIN )
             text = _T("Uncertain");
           else if( (pTag->quality & OPC_QUALITY_MASK) == OPC_QUALITY_BAD )
             text = _T("Bad");
           }
         //list.SetItem( index, eCOLUMN_QUALITY, LVIF_TEXT, text.GetBuffer(0),0,0,0,0);
         }
      list.SetRedraw( TRUE );
      return;
      }

   // Otherwise, a new device was selected, clear tags and refill with new ones
   list.DeleteAllItems();
   UpdateWindow();
   list.SetItemCount( pDoc->pCurrent->tags.GetCount() );
   lastCol = 555;

   POSITION pos = pDoc->pCurrent->tags.GetHeadPosition();
   for( int index=0; pos!=NULL; index++)
      {
      OModbusTag* pTag = pDoc->pCurrent->tags.GetNext( pos );
      CString N;
      N.Format("%i", pTag->idno);
      lv_item.iItem = index;
      lv_item.pszText = N.GetBuffer(0);
      lv_item.lParam = (LPARAM)pTag;
       if( pTag->value.vt == VT_BOOL)
          lv_item.iImage = 8;
       else if( pTag->value.vt == VT_R4 || pTag->value.vt == VT_R8)
          lv_item.iImage = 6;
       else
          lv_item.iImage = 7;
      //if( pTag->range == OModbusTag::INPUT_BOOL
      //|| pTag->range == OModbusTag::OUTPUT_BOOL )
      //   lv_item.iImage = 7;
      //else
      //   lv_item.iImage = 6;
      int item = list.InsertItem(&lv_item);
      list.SetItem(item,eCOLUMN_NAME,LVIF_TEXT,pTag->name.GetBuffer(0),0,0,0,0);
      list.SetItem(item,eCOLUMN_TYPE,LVIF_TEXT,pTag->TypeString(),0,0,0,0);
      //list.SetItem(item,eCOLUMN_LOC,LVIF_TEXT,(LPTSTR)(LPCTSTR)pTag->LocationString(),0,0,0,0);
      //if( pTag->enableProcessing )
      //   list.SetItem(item,eCOLUMN_PROCESSING,LVIF_TEXT,_T("Custom"),0,0,0,0);
      //list.SetItem(item,eCOLUMN_QUALITY,LVIF_TEXT,_T(""),0,0,0,0);
      list.SetItem(item,eCOLUMN_DESC,LVIF_TEXT,pTag->description,0,0,0,0);
      }
   list.SetRedraw( TRUE );
   }

//*******************************************************************
void CTagView::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult)
   {
   if (m_pTag)
     {
     if (m_pTag->nativeType==VT_BOOL)
       {
       m_pTag->quality = OPC_QUALITY_GOOD;
       m_pTag->value.boolVal = !m_pTag->value.boolVal;
       CoFileTimeNow( &m_pTag->timestamp );
       }
     else
       {
       //OnEditProperties();
       m_pEdit = new CValueEdit;

       m_pEdit->Create(ES_RIGHT | WS_CHILD | WS_VISIBLE | WS_BORDER, m_Rct, this, 1);
       m_pEdit->m_pEdtCtrlView=this;
       m_pEdit->SetFont(GetListCtrl().GetFont());

       CString T;
       switch( m_pTag->nativeType )
        {
        default: ; break;
        case VT_EMPTY:T.Format("0");                                 break;
        case VT_BOOL: T.Format("%s", m_pTag->value.boolVal?"1":"0"); break;
        case VT_I1:   T.Format("%i", m_pTag->value.cVal);            break;
        case VT_I2:   T.Format("%i", m_pTag->value.iVal);            break;
        case VT_I4:   T.Format("%i", m_pTag->value.lVal);            break;
        case VT_UI1:  T.Format("%i", m_pTag->value.bVal);            break;
        case VT_UI2:  T.Format("%i", m_pTag->value.uiVal);           break;
        case VT_UI4:  T.Format("%i", m_pTag->value.ulVal);           break;
        case VT_R4:   T.Format("%i", m_pTag->value.lVal);            break;
        case VT_R8:   T.Format("%g", m_pTag->value.fltVal);          break;
        case VT_BSTR: T.Format("%g", m_pTag->value.dblVal);          break;
        }

       m_pEdit->SetWindowText(T);
       m_pEdit->SetSel(0,-1);
       m_pEdit->SetFocus();
       }
     }
   
   *pResult = 0;
   }

//*******************************************************************
void CTagView::OnRclick(NMHDR* pNMHDR, LRESULT* pResult)
   {
   *pResult = 0;

   // make sure parent window is active
   GetParentFrame()->ActivateFrame();

   CPoint curPoint;
   GetCursorPos(&curPoint);

   CMenu popMenu;

   if( popMenu.LoadMenu(IDR_TAG_INSPECTOR) )
      {
      popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
                                             curPoint.x,curPoint.y,
                                             AfxGetMainWnd());
      }
   }

//*******************************************************************
void CTagView::OnEditProperties()
   {
   if( NULL == m_pTag )
      return;

   ASSERT_VALID( m_pTag );
   CString name(m_pTag->name);

   TagDialog dlg( m_pTag, GetDocument(), this);
   if( dlg.DoModal() == IDOK )
      {
      CDoc* pDoc = GetDocument();
      dlg.UpdateTag( m_pTag );
      pDoc->SetModifiedFlag();
      if(name != m_pTag->name)
        {
        POSITION pos = pDoc->pCurrent->tags.Find(m_pTag);
        if( pos )
            pDoc->pCurrent->tags.RemoveAt(pos);
        //InsertAlphabetically( m_pTag, pDoc->pCurrent->tags );
        InsertInOrder( m_pTag, pDoc->pCurrent->tags );
        }
      OnUpdate(NULL, UPDATE_TAGS, m_pTag);
      }
   }

void CTagView::OnUpdateEditProperties(CCmdUI* pCmdUI)
   {
   pCmdUI->Enable( m_pTag!=NULL );
   }

//*******************************************************************
void CTagView::OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult)
   {
   // save the current tag
   NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
   m_pTag = (OModbusTag*)pNMListView->lParam;
   ASSERT_VALID( m_pTag );
   // CNM
   CListCtrl& list = GetListCtrl();
   list.GetItemRect(pNMListView->iItem, &m_Rct, LVIR_BOUNDS);
   for (int i=0; i<eCOLUMN_VALUE; i++)
     m_Rct.left+=list.GetColumnWidth(i);
   m_Rct.right=m_Rct.left+list.GetColumnWidth(i);

   *pResult = 0;
   }

//*******************************************************************
// delete all selected tags
void CTagView::OnEditDelete()
   {
   CDoc* pDoc = GetDocument();
   CListCtrl& list = GetListCtrl();

   int foundItem = -1;
   foundItem = list.GetNextItem( foundItem, LVNI_SELECTED );
   while( foundItem != -1 )
      {
      OModbusTag* pTag = (OModbusTag*)list.GetItemData( foundItem );
      pDoc->pCurrent->DeleteTag( pTag );
      foundItem = list.GetNextItem( foundItem, LVNI_SELECTED );
      }
   m_pTag = NULL;
   pDoc->UpdateAllViews(NULL, UPDATE_TAGS, (CObject*)NULL);
   pDoc->SetModifiedFlag();
   }

void CTagView::OnUpdateEditDelete(CCmdUI* pCmdUI)
   {
   pCmdUI->Enable( m_pTag!=NULL
                && !m_pTag->InUse()
                && GetDocument()->pCurrent!=NULL
                && !GetDocument()->IsRunning() );
   }

//*******************************************************************
// Column sorting proc
int CALLBACK
CompareTags( OModbusTag* pTag1, OModbusTag* pTag2, LPARAM lCol )
   {
   int nCmp = 0;
   switch( lCol )
      {
      case eCOLUMN_NO:
         nCmp = pTag1->idno-pTag2->idno;
         break;
      case eCOLUMN_NAME:
         nCmp = pTag1->name.CompareNoCase( pTag2->name );
         break;
      case eCOLUMN_TYPE:
         nCmp=pTag1->value.vt-pTag2->value.vt;
         break;
      //case eCOLUMN_LOC:
      //   nCmp = 0;//pTag1->LocationString().CompareNoCase( pTag2->LocationString() );
      //   break;
      case eCOLUMN_DESC:
         nCmp = 0;//pTag1->description.CompareNoCase( pTag2->description );
         break;
      default:
         nCmp = 0;
         break;
      }
   if(lastCol == lCol)  // reverse order
      nCmp = -nCmp;

   return nCmp;
   }

// sort the list based on the column clicked
// reverse the order of sorting if the same col is clicked again
void CTagView::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult)
   {
   NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
   CListCtrl& list = GetListCtrl();
   list.SetRedraw( FALSE );
   list.SortItems( (PFNLVCOMPARE)CompareTags, pNMListView->iSubItem );
   list.SetRedraw( TRUE );
   if( lastCol != pNMListView->iSubItem)
      lastCol = pNMListView->iSubItem;
   else
      lastCol = 555;
   *pResult = 0;
   }

//*******************************************************************
void CTagView::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult)
   {
   CListCtrl& list = GetListCtrl();
   CDoc* pDoc = GetDocument();

   // make a list of selected tags
   OTagList tagList;
   int foundItem = -1;
   foundItem = list.GetNextItem( foundItem, LVNI_SELECTED );
   while( foundItem != -1 )
      {
      OModbusTag* m_pTag = (OModbusTag*)list.GetItemData( foundItem );
      tagList.AddTail( m_pTag );
      foundItem = list.GetNextItem( foundItem, LVNI_SELECTED );
      }

   // Serialize the list
   CSharedFile globFile;
   CArchive ar(&globFile,CArchive::store);
   tagList.Serialize( ar );
   ar.Close();

   COleDataSource srcItem; // local ok since DoDragDrop returns after
                           // the data has been transferred

   srcItem.CacheGlobalData(pDoc->nIDClipFormat,globFile.Detach());
   if( DROPEFFECT_MOVE == srcItem.DoDragDrop() )
      OnEditDelete();

   *pResult = 0;
   }

//*******************************************************************
void CTagView::OnEditCut()
   {
   OnEditCopy();
   OnEditDelete();
   }

void CTagView::OnUpdateEditCut(CCmdUI* pCmdUI)
   {
   pCmdUI->Enable( m_pTag!=NULL
                && !m_pTag->InUse()
                && GetDocument()->pCurrent!=NULL );
   }

//*******************************************************************
void CTagView::OnEditCopy()
   {
   CListCtrl& list = GetListCtrl();
   CDoc* pDoc = GetDocument();

   // make a list of selected tags
   OTagList tagList;
   int foundItem = -1;
   foundItem = list.GetNextItem( foundItem, LVNI_SELECTED );
   while( foundItem != -1 )
      {
      OModbusTag* m_pTag = (OModbusTag*)list.GetItemData( foundItem );
      tagList.AddTail( m_pTag );
      foundItem = list.GetNextItem( foundItem, LVNI_SELECTED );
      }

   // Serialize the list
   CSharedFile globFile;
   CArchive ar(&globFile,CArchive::store);
   tagList.Serialize( ar );
   ar.Close();

   COleDataSource* pSrcItem = NULL;
   TRY
      {
      pSrcItem = new COleDataSource;  // don't delete, the clipboard owns it

      pSrcItem->CacheGlobalData(pDoc->nIDClipFormat,globFile.Detach());
      pSrcItem->SetClipboard();
      }
   CATCH_ALL(e)
      {
      delete pSrcItem;
      THROW_LAST();
      }
   END_CATCH_ALL
   }

void CTagView::OnUpdateEditCopy(CCmdUI* pCmdUI)
   {
   pCmdUI->Enable( m_pTag!=NULL && GetDocument()->pCurrent!=NULL );
   }

void CTagView::OnTimer(UINT nIDEvent)
{
#ifdef RSTOOLKIT
   ICKitOnServerIdle( g_serverId ); // always call this
#endif // RSTOOLKIT

   if( nIDEvent == MONITOR_ID )
      OnUpdate( NULL, UPDATE_TAG_VALUES, NULL );
   else if( nIDEvent != DDE_ID )
      CListView::OnTimer(nIDEvent);  // this kills the timer
}

