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
#include "ScdIODB.h"

#include "TreeView.h"
#include "Mainfrm.h"
#include "DeviceDialog.h"
#include "GroupDialog.h"
#include "CopyTagDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CRITICAL_SECTION g_cs;  // Added 2.02
extern ServerList servers;
extern CDWordArray LCIDs;      // Added 2.02

//*******************************************************************
// CDeviceView

IMPLEMENT_DYNCREATE(CDeviceView, CTreeView)

BEGIN_MESSAGE_MAP(CDeviceView, CTreeView)
   //{{AFX_MSG_MAP(CDeviceView)
   ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
   ON_COMMAND(ID_EDIT_PROPERTIES, OnEditProperties)
   ON_UPDATE_COMMAND_UI(ID_EDIT_PROPERTIES, OnUpdateEditProperties)
   ON_NOTIFY_REFLECT(TVN_KEYDOWN, OnKeydown)
   ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
   ON_COMMAND(ID_EDIT_DELETE, OnEditDelete)
   ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, OnUpdateEditDelete)
   ON_WM_CREATE()
   ON_UPDATE_COMMAND_UI(ID_SUMMARY, OnUpdateSummary)
// ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBegindrag)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//*******************************************************************
// CDeviceView construction/destruction

CDeviceView::CDeviceView()
{
   clipFormatAvailable = FALSE;
   deleting = FALSE;
   // set the CTreeCtrl attributes
   m_dwDefaultStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | TVS_HASLINES
                    | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_SHOWSELALWAYS;
}

CDeviceView::~CDeviceView()
{
}

//*******************************************************************
BOOL CDeviceView::PreCreateWindow(CREATESTRUCT& cs)
{
   // TODO: Modify the Window class or styles here by modifying
   //  the CREATESTRUCT cs

   return CTreeView::PreCreateWindow(cs);
}

int CDeviceView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   if (CTreeView::OnCreate(lpCreateStruct) == -1)
      return -1;

   // Create the Image List
   VERIFY( m_ctlImage.Create(IDB_IMAGELIST,16,0,RGB(255,0,255)) );
   m_ctlImage.SetBkColor(GetSysColor(COLOR_WINDOW));

   /// Attach image list to Tree
   CTreeCtrl& ctlTree = GetTreeCtrl();
   ctlTree.SetImageList(&m_ctlImage, TVSIL_NORMAL);

   m_dropTarget.Register(this);

   return 0;
}

//*******************************************************************
// CDeviceView drawing

void CDeviceView::OnDraw(CDC* pDC)
{
   CDoc* pDoc = GetDocument();
   ASSERT_VALID(pDoc);
}

void CDeviceView::OnInitialUpdate()
{
   CDoc* pDoc = GetDocument();
   pDoc->nIDClipFormat = RegisterClipboardFormat(_T("ScdTgSrcScdTgSrc"));
   ASSERT( pDoc->nIDClipFormat );
   CTreeView::OnInitialUpdate();
}

//*******************************************************************
// CDeviceView diagnostics

#ifdef _DEBUG
void CDeviceView::AssertValid() const
{
   CTreeView::AssertValid();
}

void CDeviceView::Dump(CDumpContext& dc) const
{
   CTreeView::Dump(dc);
}

CDoc* CDeviceView::GetDocument() // non-debug version is inline
{
   ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDoc)));
   return (CDoc*)m_pDocument;
}
#endif //_DEBUG

//*******************************************************************
// CDeviceView message handlers

void CDeviceView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
   CTreeView::OnUpdate(pSender, lHint, pHint);

   if( lHint == UPDATE_TAGS || lHint == UPDATE_TAG_VALUES )
      return;

   CDoc* pDoc = GetDocument();
   CTreeCtrl& tree = GetTreeCtrl();
   switch( lHint )
      {
      case CLEAR_ALL:
         deleting = TRUE;
         tree.DeleteAllItems();
         deleting = FALSE;
         break;
      case UPDATE_ALL:
      case UPDATE_DEVICES:
         if( pHint )
            {
            OTagGroup* pDevice = (OTagGroup*)pHint;
            ASSERT_KINDOF( OTagGroup, pDevice );
            // find this item in the tree
            ASSERT( pDevice == (OTagGroup*)tree.GetItemData( pDevice->hItem ) );
            // Set new stuff (name)
            tree.SetItem( pDevice->hItem, TVIF_TEXT, pDevice->name, 0, 0, 0, 0, 0 );
            }
         else
            {
            deleting = TRUE;
            tree.DeleteAllItems();
            deleting = FALSE;

            POSITION pos = pDoc->devices.groups.GetHeadPosition();
            while( pos )
               {
               OTagGroup* pDevice = pDoc->devices.groups.GetNext( pos );

               pDevice->hItem = tree.InsertItem( pDevice->name, 1, 2 );
               tree.SetItemData( pDevice->hItem, (DWORD)pDevice );
               POSITION pos = pDevice->groups.GetHeadPosition();
               while( pos )
                  {
                  OTagGroup* pGroup = pDevice->groups.GetNext( pos );
                  pGroup->hItem = tree.InsertItem( pGroup->name, 3, 4, pDevice->hItem );
                  tree.SetItemData( pGroup->hItem, (DWORD)pGroup );
                  POSITION pos = pGroup->groups.GetHeadPosition();
                  while( pos )
                     {
                     OTagGroup* pGroup2 = pGroup->groups.GetNext( pos );
                     pGroup2->hItem = tree.InsertItem( pGroup2->name, 3, 4, pGroup->hItem );
                     tree.SetItemData( pGroup2->hItem, (DWORD)pGroup2 );
                     POSITION pos = pGroup2->groups.GetHeadPosition();
                     while( pos )
                        {
                        OTagGroup* pGroup3 = pGroup2->groups.GetNext( pos );
                        pGroup3->hItem = tree.InsertItem( pGroup3->name, 3, 4, pGroup2->hItem );
                        tree.SetItemData( pGroup3->hItem, (DWORD)pGroup3 );
                        }
                     }
                  }
               }
            if (1)
              {
              pos = servers.GetHeadPosition();
              while( pos )
                {
                OPCServerBase * p = servers.GetNext(pos);
                HTREEITEM h=tree.InsertItem( p->m_client, 1, 2 );

                OPCGroupObject* pGroup = NULL;
                LPVOID key = 0;
                POSITION pos1 = p->m_groupMap.GetStartPosition();
                while( pos1 )
                  {
                  p->m_groupMap.GetNextAssoc( pos1, key, pGroup );
                  ASSERT( pGroup );
            
                  CString Str=pGroup->GetGroupName();
                  HTREEITEM h2=tree.InsertItem( Str, 1, 2, h);

/**
                  COPCItem* pItem = NULL;
                  LPVOID key2 = 0;
                  POSITION pos2 = pGroup->m_itemMap.GetStartPosition();
                  while( pos2 )
                    {
                    pGroup->m_itemMap.GetNextAssoc( pos2, key2, pItem);
                    ASSERT( pItem);
            

                    CString Str=((CModbusItem*)pItem)->pTag->name;
                    tree.InsertItem( Str, 1, 2, h2);
                    }
**/
                  }
                }
              }

            if( pDoc->pCurrent )
               {
               tree.SelectItem( pDoc->pCurrent->hItem );
               tree.EnsureVisible( pDoc->pCurrent->hItem );
               }

            }
         break;
      case ADD_DEVICE:
         {
         OTagGroup* pDevice = (OTagGroup*)pHint;
         ASSERT_KINDOF( OTagGroup, pDevice );
         ASSERT( pDevice->hItem == 0 );

         HTREEITEM prevItem = TVI_ROOT;
         if( pDevice->parent != &pDoc->devices )   // this is a child
            {
            // look for previous group
            POSITION pos = pDevice->parent->groups.Find( pDevice );
            if( pos )
               {
               pDevice->parent->groups.GetPrev( pos );  // moves pointer back one
               if( pos )
                  {
                  OTagGroup* pGroup = pDevice->parent->groups.GetAt( pos );
                  prevItem = pDevice->parent->groups.GetAt( pos )->hItem;
                  }
               }
            pDevice->hItem = tree.InsertItem( pDevice->name, 3, 4,
                                              pDevice->parent->hItem, prevItem );
            }
         else  // insert at the root level
            {
            // look for previous device
            POSITION pos = pDoc->devices.groups.Find( pDevice );
            if( pos )
               {
               pDoc->devices.groups.GetPrev( pos );  // moves pointer back one
               if( pos )
                  {
                  OTagGroup* pGroup = pDoc->devices.groups.GetAt( pos );
                  prevItem = pGroup->hItem;
                  }
               }
            pDevice->hItem = tree.InsertItem( pDevice->name, 1, 2,
                                              TVI_ROOT, prevItem );
            }
         tree.SetItemData( pDevice->hItem, (DWORD)pDevice );
         tree.SelectItem( pDevice->hItem );
         tree.EnsureVisible( pDevice->hItem );
         }
         break;
      default: ASSERT( FALSE );
      }
}

//*******************************************************************
// store the selected device/group pointer in the doc
void CDeviceView::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult)
{
   if( deleting ) // items are not valid during deletion
      return;

   CDoc* pDoc = GetDocument();
   CTreeCtrl& tree = GetTreeCtrl();
   HTREEITEM item = tree.GetSelectedItem( );
   pDoc->pCurrent = (OTagGroup*)tree.GetItemData( item );

   pDoc->UpdateAllViews(this, UPDATE_TAGS, (CObject*)NULL);
   *pResult = 0;
}

//*******************************************************************
void CDeviceView::OnEditProperties()
{
   CDoc* pDoc = GetDocument();
   ASSERT_VALID( pDoc->pCurrent );

   CTreeCtrl& tree = GetTreeCtrl();

   if(pDoc->pCurrent->IsKindOf(RUNTIME_CLASS(ODevice)) )
      {
      DeviceDialog dlg( (ODevice*)pDoc->pCurrent, pDoc, this );
      if( dlg.DoModal() == IDOK )
         {
         dlg.UpdateDevice( );
         // match port names up with ports
         ODevice* pDevice = (ODevice*)pDoc->pCurrent;
         //POSITION portPos = pDoc->ports.GetHeadPosition();
         //while( portPos )
         //   {
         //   OPort* pPort = pDoc->ports.GetNext( portPos );
         //   if( pPort->name == pDevice->primary.portName )
         //      pDevice->primary.pPort = pPort;
         //   }

         pDoc->UpdateAllViews(NULL, UPDATE_DEVICES, (CObject*)pDoc->pCurrent);
         pDoc->SetModifiedFlag();
         }
      }
   else
      {
      GroupDialog dlg(pDoc->pCurrent, this);
      if( dlg.DoModal() == IDOK )
         {
         pDoc->pCurrent->name = dlg.m_name;
         pDoc->UpdateAllViews(NULL, UPDATE_DEVICES, (CObject*)pDoc->pCurrent);
         pDoc->SetModifiedFlag();
         }
      }
}

void CDeviceView::OnUpdateEditProperties(CCmdUI* pCmdUI)
{
   pCmdUI->Enable( GetDocument()->pCurrent!=NULL );
}

//*******************************************************************
// Just handle delete key
void CDeviceView::OnKeydown(NMHDR* pNMHDR, LRESULT* pResult)
{
   *pResult = 0;
   if( GetDocument()->pCurrent==NULL )
      {
      return;
      }

   TV_KEYDOWN* pKeyDown = (TV_KEYDOWN*)pNMHDR;

   if (pKeyDown->wVKey == VK_DELETE)
      {
      OnEditDelete();
      }
}

//*******************************************************************
void CDeviceView::OnRclick(NMHDR* pNMHDR, LRESULT* pResult)
{
   *pResult = 0;
   CTreeCtrl& tree = GetTreeCtrl();

   // make sure frame window is active
   GetParentFrame()->ActivateFrame();

   CPoint curPoint;
   GetCursorPos(&curPoint);
   CPoint client( curPoint );
   ScreenToClient(&client);
   HTREEITEM item = tree.HitTest(client);
   if( item )
      {
      GetDocument()->pCurrent = (OTagGroup*)tree.GetItemData( item );

      CMenu popMenu;
      if( popMenu.LoadMenu(IDR_DEVICE_INSPECTOR) )
         {
         popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
                                                curPoint.x,curPoint.y,
                                                AfxGetMainWnd());
         }
      }
}

//*******************************************************************
void CDeviceView::OnEditDelete()
{
   if( GetDocument()->pCurrent!=NULL
    && !GetDocument()->pCurrent->InUse()
    && !GetDocument()->IsRunning() )
   {
      CDoc* pDoc = GetDocument();
      if( !pDoc->pCurrent ) return; // may be called from a keydown
      ASSERT_VALID( pDoc->pCurrent );

      pDoc->DeleteCurrent();
   }
}

void CDeviceView::OnUpdateEditDelete(CCmdUI* pCmdUI)
{
   pCmdUI->Enable( GetDocument()->pCurrent!=NULL
                && !GetDocument()->pCurrent->InUse()
                && !GetDocument()->IsRunning() );
}

//*******************************************************************
void CDeviceView::OnUpdateSummary(CCmdUI* pCmdUI)
{
   CDoc* pDoc = GetDocument();
   pCmdUI->Enable( TRUE );
   if( pDoc->pCurrent )
      {
      CString msg;
      msg.Format(_T("%s has %d Tags"), pDoc->pCurrent->name,
                 pDoc->pCurrent->tags.GetCount() );
      pCmdUI->SetText( msg );
      }
   else
      pCmdUI->SetText( _T("") );
}

//*******************************************************************
DROPEFFECT CDeviceView::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
   CDoc* pDoc = GetDocument();
   if (pDataObject->IsDataAvailable(pDoc->nIDClipFormat))
      {
      clipFormatAvailable = TRUE;
      if( dwKeyState & MK_CONTROL )
         return DROPEFFECT_COPY;
      else
         return DROPEFFECT_MOVE;
      }
   else
      {
      clipFormatAvailable = FALSE;
      }

   return CTreeView::OnDragEnter(pDataObject, dwKeyState, point);
}

DROPEFFECT CDeviceView::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
   CTreeCtrl& tree = GetTreeCtrl();
   HTREEITEM item = tree.HitTest(point);
   if( item != NULL )
   {
      tree.SelectDropTarget( item );
      CDoc* pDoc = GetDocument();
      if( clipFormatAvailable )
         if( dwKeyState & MK_CONTROL )
            return DROPEFFECT_COPY;
         else
            return DROPEFFECT_MOVE;
   }
   return CTreeView::OnDragOver(pDataObject, dwKeyState, point);
}

void CDeviceView::OnDragLeave()
{
   CTreeCtrl& tree = GetTreeCtrl();
   tree.SelectDropTarget( NULL );   // let selection be highlighted again

   CTreeView::OnDragLeave();
}

BOOL CDeviceView::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
   CDoc* pDoc = GetDocument();
   if (!pDataObject->IsDataAvailable(pDoc->nIDClipFormat))
      return CTreeView::OnDrop(pDataObject, dropEffect, point);

   // get the target device/group
   CTreeCtrl& tree = GetTreeCtrl();
   HTREEITEM item = tree.GetDropHilightItem();
   if (!item)
      return FALSE;
   OTagGroup* pGroup = (OTagGroup*)tree.GetItemData( item );
   if (!pGroup)
      return FALSE;

   tree.SelectDropTarget( NULL );   // let selection be highlighted

   if( pGroup == pDoc->pCurrent )   // no sense copying/moving to itself
      return FALSE;

   CFile* pFile = pDataObject->GetFileData(pDoc->nIDClipFormat);
   if (pFile == NULL)
      return FALSE;

   CArchive ar(pFile,CArchive::load);
   ar.m_pDocument = pDoc; // set back-pointer in archive

   OTagList list;
   TRY
      {
      list.Serialize( ar );
      }
   CATCH_ALL(e)
      {
      ar.Close();
      delete pFile;
      THROW_LAST();
      }
   END_CATCH_ALL
   ar.Close();
   delete pFile;

   // insert tags into this device/group
   POSITION pos = list.GetHeadPosition();
   while( pos )
      {
      OModbusTag* pTag = list.GetNext( pos );
rename:
      // check for name conflicts
      POSITION searchPos = pGroup->tags.GetHeadPosition();
      while( searchPos )
         {
         OModbusTag* pTestTag = pGroup->tags.GetNext( searchPos );
         if( pTag->name.CompareNoCase( pTestTag->name ) == 0 )
            { // found a tag by the same name
            CCopyTagDlg dlg( pTag, pTestTag );
            int action = dlg.DoModal();
            switch( action )
               {
               case IDC_RENAME:
                  pTag->name = dlg.m_name;   // try a new name
                  goto rename;
               case IDC_OVERWRITE:
                  *pTestTag = *pTag;   // copy into current tag
                  delete pTag;         // don't insert this one
                  goto skip;
               case IDC_SKIP:
                  delete pTag;         // don't insert this one
                  goto skip;
               case IDCANCEL:
                  delete pTag;         // don't insert this one
                  while( pos )         // clear out the rest of the list
                     {
                     delete list.GetNext( pos );
                     }
                  goto cancel;
               }
            searchPos = NULL; // escape the loop
            }
         }
      {
      CSLock wait( &uiCS );
      pGroup->AddTag( pTag );
     pTag->pGroup = pGroup;
     }
skip:
      ;
      }
cancel:

   pDoc->SetModifiedFlag();

   return TRUE;
}

//*******************************************************************
