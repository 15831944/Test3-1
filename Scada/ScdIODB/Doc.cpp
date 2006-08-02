//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  Doc.cpp
//  $Author    :  Jim Hansen
//
//  Description:  The doc contains the data for the tag system
//                (devices, groups, tags, ports) and most operations on them
//
//**************************************************************************

#include "stdafx.h"
#include "Mainfrm.h"
#include "Listview.h"
#include "message.h"
#include "ScdIODB.h"
#include "DeviceDialog.h"
#include "GroupDialog.h"
#include "TagDialog.h"
#include "CopyTagDlg.h"
#include "Ports.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDoc* theDoc = NULL;
//*******************************************************************
// CDoc

IMPLEMENT_SERIAL(CDoc, CDocument,  1 | VERSIONABLE_SCHEMA)

BEGIN_MESSAGE_MAP(CDoc, CDocument)
  //{{AFX_MSG_MAP(CDoc)
  ON_COMMAND(ID_ADD_NEWDEVICE, OnAddNewdevice)
  ON_COMMAND(ID_ADD_NEWGROUP, OnAddNewgroup)
  ON_UPDATE_COMMAND_UI(ID_ADD_NEWGROUP, OnUpdateAddNewgroup)
  ON_COMMAND(ID_ADD_NEWTAG, OnAddNewtag)
  ON_UPDATE_COMMAND_UI(ID_ADD_NEWTAG, OnUpdateAddNewtag)
  ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
  ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
  ON_COMMAND(ID_EDIT_PORTS, OnEditPorts)
  ON_COMMAND(ID_VIEW_MONITOR, OnMonitor)
  ON_UPDATE_COMMAND_UI(ID_VIEW_MONITOR, OnUpdateMonitor)
  ON_COMMAND(ID_UPDATEALL, OnUpdateall)
  //}}AFX_MSG_MAP
  // CNM I Have removed the following 2 from due to error message casting CDoc* to CWnd* 
  // I do not think taht they worked anyway !!
  //ON_MESSAGE(WM_ADDDEVICE, OnUpdateAddDevice)
  //ON_MESSAGE(WM_UPDATETAGS, OnUpdateTags)
END_MESSAGE_MAP()

//*******************************************************************
// CDoc construction/destruction

CDoc::CDoc()
{
   hScanThread = NULL;
   pCurrent = NULL;
   nextIndex = 1;
   run = FALSE;
   theDoc = this;
}

CDoc::~CDoc()
{
#ifdef RSTOOLKIT
   // Clean up the server toolkit.
   ICKitCleanUpDDEServerInterface( g_serverId );
#endif

// Delete all OPCServers

   if( hScanThread )   // stop monitoring?
   {
      run = FALSE;

      // Terminate the thread
      if( WAIT_FAILED == WaitForSingleObject( hScanThread, 5000 ) )
         {
         TRACE_MSG;
         }
      CloseHandle( hScanThread );
      hScanThread = NULL;
   }
   while( !devices.groups.IsEmpty() )
      delete devices.groups.RemoveTail();
//   while( !ports.IsEmpty() )
//      delete ports.RemoveTail();

   theDoc = NULL;
}

BOOL CDoc::OnNewDocument()
{
   if ( _Module.GetLockCount()>0 )
   {
      AfxMessageBox(_T("Cannot change files while OPC Clients are connected to this server.") );
      return FALSE;
   }
      
   // Turn off monitor mode if it is currently on.
   if (run)
      OnMonitor();
#ifdef RSTOOLKIT
   // Clean up the server toolkit. The tags it references
   // are disappearing.
   ICKitCleanUpDDEServerInterface( g_serverId );
#endif RSTOOLKIT

   {
   CSLock wait( &uiCS );
   pCurrent = NULL;
   // clear out existing contents
   while( !devices.groups.IsEmpty() )
      delete devices.groups.RemoveTail();
//   while( !ports.IsEmpty() )
//      delete ports.RemoveTail();
   nextIndex = 1;
   }
   UpdateAllViews(NULL, CLEAR_ALL);

   //OPort* pPort = new OPort;
   //pPort->name = _T("COM1");
   //ports.AddTail( pPort );
   //pPort = new OPort;
   //pPort->name = _T("COM2");
   //ports.AddTail( pPort );

#ifdef RSTOOLKIT
    // Re-Initialize the server toolkit.
    g_serverId = ICKitSetupDDEServerInterface( AfxGetMainWnd( )->GetSafeHwnd(),
               AfxGetApp( )->m_hInstance, "",
               FARPROC (ICSystemAction) );
#endif RSTOOLKIT

   if (!CDocument::OnNewDocument())
      return FALSE;

   // Switch Monitor On by default
   if (!run)
      OnMonitor();

   return TRUE;
}

BOOL CDoc::OnOpenDocument( LPCTSTR lpszPathName )
   {
   if ( _Module.GetLockCount()>0 )
   {
      AfxMessageBox(_T("Cannot change files while OPC Clients are connected to this server.") );
      return FALSE;
   }

   // Turn off monitor mode if it is currently on.
   if (run)
      OnMonitor();

#ifdef RSTOOLKIT
   // Clean up the server toolkit. The tags it references
   // are disappearing.
   ICKitCleanUpDDEServerInterface( g_serverId );
#endif RSTOOLKIT

   CSLock wait( &uiCS );
   // clear out existing contents
   while( !devices.groups.IsEmpty() )
      delete devices.groups.RemoveTail();
   //while( !ports.IsEmpty() )
   //   delete ports.RemoveTail();
   pCurrent = NULL;
   nextIndex = 1;
   UpdateAllViews(NULL, CLEAR_ALL);

#ifdef RSTOOLKIT
    // Re-Initialize the server toolkit.
    g_serverId = ICKitSetupDDEServerInterface( AfxGetMainWnd( )->GetSafeHwnd(),
               AfxGetApp( )->m_hInstance, "",
               FARPROC (ICSystemAction) );
#endif RSTOOLKIT

   if (!CDocument::OnOpenDocument(lpszPathName))
      return FALSE;

   if( !devices.groups.IsEmpty() )   // set selection to first device
      pCurrent = devices.groups.GetHead();

   UpdateAllViews(NULL);

   // Switch Monitor On by default
   if (!run)
      OnMonitor();
   
   return TRUE;
   }

//*******************************************************************
// CDoc serialization

void CDoc::Serialize(CArchive& ar)
{
   ar.SerializeClass(RUNTIME_CLASS(CDoc));   // version ourself
   if (ar.IsStoring())
   {
      ar << nextIndex;
      //ports.Serialize(ar);
      ar.SerializeClass(RUNTIME_CLASS(OTagGroup));
      devices.Serialize(ar);
   }
   else
   {
      UINT version = ar.m_nObjectSchema;
      ar >> nextIndex;
      //ports.Serialize(ar);
      ar.SerializeClass(RUNTIME_CLASS(OTagGroup));
      devices.Serialize(ar);
      devices.SetTagDevices();
   }
}

//*******************************************************************
// CDoc diagnostics

#ifdef _DEBUG
void CDoc::AssertValid() const
{
   CDocument::AssertValid();
}

void CDoc::Dump(CDumpContext& dc) const
{
   CDocument::Dump(dc);
}
#endif //_DEBUG

//*******************************************************************
// CDoc commands
void CDoc::DeleteCurrent()
{
   CSLock wait( &uiCS );
   ASSERT_VALID( pCurrent );
   if( pCurrent->InUse() || run )  // if monitoring or in use
      return;

   if(pCurrent->IsKindOf(RUNTIME_CLASS(ODevice)) )
      {
      POSITION pos = devices.groups.Find( pCurrent );
      ASSERT( pos );
      POSITION prev = pos;
      devices.groups.GetPrev( prev );         // move to prev pos
      if( prev )
         pCurrent=devices.groups.GetAt( prev );  // save prev item
      else
         pCurrent=NULL;
      delete devices.groups.GetAt( pos );     // delete original item
      devices.groups.RemoveAt( pos );         // and remove from list
      UpdateAllViews(NULL, UPDATE_ALL,NULL);
      }
   else  // its a group
      {
      POSITION pos = pCurrent->parent->groups.Find( pCurrent );
      ASSERT( pos );
      pCurrent->parent->groups.RemoveAt( pos );
      delete pCurrent;
      pCurrent = NULL;
      UpdateAllViews(NULL, UPDATE_ALL, (CObject*)pCurrent);
      }
   SetModifiedFlag();
}

//*******************************************************************
void CDoc::DoAddNewDevice(ODevice *pNewCurrent, BOOL UpdateByMessage)
  {      
  pCurrent = pNewCurrent;
  //dlg.UpdateDevice();

  pCurrent->parent = &devices;
  // insert into list in alphabetical order
  POSITION pos = devices.groups.GetHeadPosition();

  if( pos == NULL )    // list is empty
     {
     devices.groups.AddHead( (ODevice*)pCurrent );
     if (UpdateByMessage)
       {
       theApp.m_pMainWnd/*AfxGetMainWnd()*/->PostMessage(WM_COMMAND, ID_UPDATEALLRQST, 0);
       //ScdMainWnd()->PostMessage(WM_ADDDEVICE, 0, (LPARAM)pCurrent);
       }
     else
       {
       UpdateAllViews(NULL, ADD_DEVICE, (CObject*)pCurrent);
       SetModifiedFlag();
       }
     return;
     }

  while( pos )        // iterate over all objects
     {
     POSITION temp = pos;
     OTagGroup* pGroup = devices.groups.GetNext( temp );

     // same as: if( current->name < treeclass->name ) ignoring case
     if( pCurrent->name.CompareNoCase( pGroup->name ) == -1 )
        break;
     pos = temp;
     }

  if( pos )     // found a position
     devices.groups.InsertBefore( pos, (ODevice*)pCurrent );
  else              // past the end
     devices.groups.AddTail( (ODevice*)pCurrent );

  if (UpdateByMessage)
    {
    theApp.m_pMainWnd/*AfxGetMainWnd()*/->PostMessage(WM_COMMAND, ID_UPDATEALLRQST, 0);
    //ScdMainWnd()->PostMessage(WM_ADDDEVICE, 0, (LPARAM) pCurrent);
    }
  else
    {
    UpdateAllViews(NULL, ADD_DEVICE, (CObject*)pCurrent);
    SetModifiedFlag();
    }
  }

//*******************************************************************

void CDoc::OnAddNewdevice()
{
   ODevice* pNewCurrent;
   CSLock wait( &uiCS );

   pNewCurrent = new ODevice;
   pNewCurrent->name.Format( _T("Device%d"), nextIndex++ );
   DeviceDialog dlg( (ODevice*)pNewCurrent, this );
   if( dlg.DoModal() == IDOK )
      {
      pCurrent = pNewCurrent;
      dlg.UpdateDevice();
      DoAddNewDevice(pNewCurrent, false);
      }
   else
      {
      delete pNewCurrent;
      }
}

//*******************************************************************
void CDoc::OnAddNewgroup()
{
   OTagGroup* pGroup = new OTagGroup;
   pGroup->parent = pCurrent;
   GroupDialog dlg( pGroup );
   if( dlg.DoModal() == IDOK )
      {
      pGroup->name = dlg.m_name;
      // insert into list in alphabetical order
      POSITION pos = pCurrent->groups.GetHeadPosition();

      if( pos == NULL )    // list is empty
         {
         EnterCriticalSection( &uiCS );
         pCurrent->groups.AddHead( pGroup );
         LeaveCriticalSection( &uiCS );
         UpdateAllViews(NULL, ADD_DEVICE, (CObject*)pGroup);
         SetModifiedFlag();
         return;
         }

      while( pos )        // iterate over all objects
         {
         POSITION temp = pos;
         OTagGroup* pTestGroup = pCurrent->groups.GetNext( temp );

         // same as: if( current->name < treeclass->name ) ignoring case
         if( pGroup->name.CompareNoCase( pTestGroup->name ) == -1 )
            break;
         pos = temp;
         }

      if( pos )     // found a position
         pCurrent->groups.InsertBefore( pos, pGroup );
      else              // past the end
         pCurrent->groups.AddTail( pGroup );
      EnterCriticalSection( &uiCS );
      pCurrent = pGroup;
      LeaveCriticalSection( &uiCS );

     UpdateAllViews(NULL, ADD_DEVICE, (CObject*)pGroup);

      SetModifiedFlag();
      }
   else
      {
      delete pGroup;
      }
}

void CDoc::OnUpdateAddNewgroup(CCmdUI* pCmdUI)
{
   pCmdUI->Enable( (pCurrent!=NULL) );
}

//*******************************************************************
void CDoc::OnAddNewtag()
{
   OModbusTag* pTag = new OModbusTag;
   pTag->name.Format( _T("Tag%d"), pCurrent->nextIndex++ );
   pTag->pGroup = pCurrent;
   TagDialog dlg( pTag, this );
   if( dlg.DoModal() == IDOK )
      {
      dlg.UpdateTag( pTag );
      //InsertAlphabetically( pTag, pCurrent->tags );
      InsertInOrder( pTag, pCurrent->tags );

      UpdateAllViews(NULL, UPDATE_TAGS, NULL); //(CObject*)pTag);
      SetModifiedFlag();
      }
   else
      {
      delete pTag;
      }
}

void CDoc::OnUpdateAddNewtag(CCmdUI* pCmdUI)
{
   pCmdUI->Enable( (pCurrent!=NULL) );
}

//*******************************************************************
// Inserts tags into the view list alphabetically
void InsertAlphabetically(OModbusTag* pTag, OTagList& list )
{
   CSLock wait( &uiCS );
   POSITION pos = list.GetHeadPosition();

   if( pos == NULL )    // list is empty
      {
      list.AddHead( pTag );
      return ;
      }

   while( pos )        // iterate over all objects
      {
      POSITION temp = pos;
      OTag* pTestTag = list.GetNext( temp );

      // same as: if( pTag->name < pTestTag->name ) ignoring case
      if( pTag->name.CompareNoCase( pTestTag->name ) == -1 )
         break;
      pos = temp;
      }

   if( pos )     // found a position
      list.InsertBefore( pos, /*(CObject*)*/pTag );
   else              // past the end
      list.AddTail( pTag );
}

//*******************************************************************
// Inserts tags into the view list in order
void InsertInOrder(OModbusTag* pTag, OTagList& list )
{
   CSLock wait( &uiCS );
   list.AddTail( pTag );
}

//*******************************************************************
void CDoc::OnEditPaste()
{
   COleDataObject dataObject;
   dataObject.AttachClipboard();

   if (dataObject.IsDataAvailable(nIDClipFormat))
      {
      CFile* pFile = dataObject.GetFileData(nIDClipFormat);
      if (pFile == NULL)
         return;

      CArchive ar(pFile,CArchive::load);
      ar.m_pDocument = this; // set back-pointer in archive

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
         POSITION searchPos = pCurrent->tags.GetHeadPosition();
         while( searchPos )
            {
            OModbusTag* pTestTag = pCurrent->tags.GetNext( searchPos );
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
                     {
                     CSLock wait( &tagCS );
                     *pTestTag = *pTag;   // copy into current tag
                     delete pTag;         // don't insert this one
                     }
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
         pCurrent->AddTag( pTag );
         pTag->pGroup = pCurrent;
         }
skip:
         ;
         }
cancel:

      UpdateAllViews(NULL, UPDATE_TAGS, (CObject*)NULL);
      SetModifiedFlag();
      }
}

void CDoc::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
   COleDataObject dataObject;
   BOOL bEnable = dataObject.AttachClipboard() &&
      dataObject.IsDataAvailable(nIDClipFormat);
   pCmdUI->Enable( pCurrent!=NULL && bEnable );
}

//*******************************************************************
void CDoc::OnEditPorts()
{
   //OPortDlg dlg(this);
   //dlg.DoModal();
}

//*******************************************************************
// Keep scan tags ordered by address so that scanning can combine tags
// in a read packet
void InsertByAddress(OModbusTag* pTag, OTagList& list )
{
   CSLock wait( &tagCS );
   POSITION pos = list.GetHeadPosition();

   if( pos == NULL )    // list is empty
      {
      list.AddHead( pTag );
      return;
      }

   while( pos )        // iterate over all objects
      {
      POSITION temp = pos;
      OModbusTag* pTestTag = list.GetNext( temp );

      if( pTestTag == pTag )  // avoid duplicates
         return;

      //if( pTag->range < pTestTag->range
      //|| (pTag->range == pTestTag->range && pTag->offset < pTestTag->offset) )
      //   break;
      pos = temp;
      }

   if( pos )     // found a position
      list.InsertBefore( pos, pTag );
   else              // past the end
      list.AddTail( pTag );
}

//*******************************************************************
DWORD CDoc::ThreadStub(DWORD arg)
   {
   return ((CDoc*)arg)->DataScanThread();
   }

// The monitor thread scans the tags in the tag view
// so the view can update the values on screen
DWORD CDoc::DataScanThread()
   {
   OTagGroup* pDevice = NULL;
   OTagList tagList;

   // Now scan
   if (pCurrent)
     {
     while( run )
        {
        Sleep( 300 );
        if( pDevice != pCurrent )    // changed in view
           {
           CSLock wait( &uiCS );
           if( pDevice )
              InterlockedDecrement( &pDevice->refCount );    // release old device
           pDevice = pCurrent;
           tagList.RemoveAll();
           // create tag list ordered by address
           POSITION pos = pCurrent->tags.GetHeadPosition();
           while( pos )
              {
              OModbusTag* pTag = pCurrent->tags.GetNext( pos );
              InsertByAddress( pTag, tagList );
              }
           InterlockedIncrement( &pDevice->refCount );
           }
        pDevice->GetDevice()->UpdateTags( tagList, 300 );
        }
     }
   if( pDevice )
      InterlockedDecrement( &pDevice->refCount );

   return 1;
   }
//*******************************************************************
//*******************************************************************
// start/stop monitor mode
void CDoc::OnMonitor()
{
   CWaitCursor wait;
   if( hScanThread == NULL )   // start monitoring
      {
      run = TRUE;
      SECURITY_ATTRIBUTES sa;
      sa.nLength = sizeof(sa);
      sa.lpSecurityDescriptor = NULL;
      DWORD threadId=0;

      hScanThread = CreateThread( &sa, 0,
      (LPTHREAD_START_ROUTINE)&ThreadStub,
                              this,
                              0,
                              &threadId);
      if( hScanThread == NULL )
         {
         TRACE_MSG;
         return;
         }

      // start timer on the tag view
      VERIFY( MONITOR_ID == pTagView->SetTimer( MONITOR_ID, 600, NULL ) );
      }
   else
      {
      run = FALSE;

      // Kill the timer
      pTagView->KillTimer( MONITOR_ID );

      // Terminate the thread
      if( WAIT_FAILED == WaitForSingleObject( hScanThread, 5000 ) )
         {
         TRACE_MSG;
         }
      CloseHandle( hScanThread );
      hScanThread = NULL;

      UpdateAllViews(NULL, UPDATE_TAGS, NULL); // clear the value field
      }
}

//*******************************************************************
void CDoc::OnUpdateMonitor(CCmdUI* pCmdUI)
{
   pCmdUI->Enable( !devices.groups.IsEmpty() );
   pCmdUI->SetCheck( hScanThread != NULL );
}

//*******************************************************************
// starts recursive search for a named tag: eg. device.group.group.tag
OTag* CDoc::FindTag( const CString& targetin, WORD RequestedType, ODevice** pDevice )
   {
   CSLock wait( &uiCS );
   CString target(targetin);
   if (1)
     {
     // remove all '.'
     int dot;
     while ((dot=target.Find( _T('.')))>=0)
       target.SetAt(dot, '~');
     }   

   // CNM Force Tags with no device to be in First Device
   if (devices.groups.GetCount()==0)
     {
     ODevice* pNewCurrent;
     pNewCurrent = new ODevice;
     pNewCurrent->name.Format( _T("Device%d"), nextIndex++ );
     DoAddNewDevice(pNewCurrent, true);
     }
   
   int delimiter = target.Find( _T('.') );

   OTag * pRetTag=NULL;
   OTagGroup* pGroup=NULL;
   CString ThisTag=target;
   if( delimiter >=1 )
     {
     CString device( target.Left( delimiter ) );
     POSITION pos = devices.groups.GetHeadPosition();
     while( pos )
        {
        /*OTagGroup* */ pGroup = devices.groups.GetNext( pos );
        if( pGroup->name.CompareNoCase( device ) == 0 )
           {
           if( pDevice != NULL )
              *pDevice = (ODevice*)pGroup;
           ThisTag=target.Mid( delimiter+1 );
           pRetTag=pGroup->FindTag( ThisTag);
           goto SearchDone;
           }
        }
     }
   else
     {
     POSITION pos = devices.groups.GetHeadPosition();
     if ( pos )
        {
        /*OTagGroup* */ pGroup = devices.groups.GetNext( pos );
        if( pDevice != NULL )
          *pDevice = (ODevice*)pGroup;
        ThisTag=target;
        pRetTag=pGroup->FindTag( ThisTag);
        goto SearchDone;
        }
     }

SearchDone:
    if (!pRetTag)
      {
      // Tag not found -- AddIt
      OModbusTag* pTag = new OModbusTag;
      pTag->name = ThisTag;
      pTag->pGroup = pCurrent;

      pTag->m_IsOutput = true; // Must improve ??? 
      if (RequestedType==VT_BOOL)
        {
        pTag->nativeType = pTag->value.vt = VT_BOOL;
        pTag->value.boolVal = VARIANT_FALSE;
        }
      else
        {
        pTag->nativeType = pTag->value.vt = RequestedType;
        switch (RequestedType)
          {
          case VT_R4: pTag->value.fltVal = 0; break;
          case VT_R8: pTag->value.dblVal = 0; break;
          default: pTag->value.lVal = 0; break;
          }
        }
      
      //InsertAlphabetically( pTag, pCurrent->tags );
      if(!pCurrent &&  !devices.groups.IsEmpty() )   // set selection to first device
        pCurrent = devices.groups.GetHead();
      InsertInOrder( pTag, pCurrent->tags );

      theApp.m_pMainWnd->PostMessage(WM_COMMAND, ID_UPDATEALLRQST, 0);
      //ScdMainWnd()->PostMessage(WM_COMMAND, ID_UPDATEALLRQST, 0);
      //ScdMainWnd()->PostMessage(WM_UPDATETAGS, 0, 0);

      pRetTag = pTag;
      }

   return pRetTag;
   }

LRESULT CDoc:: OnUpdateAddDevice(WPARAM wParam, LPARAM lParam)
  {
  UpdateAllViews(NULL, ADD_DEVICE, (CObject*)lParam);
  SetModifiedFlag();
  return 0;
  }

LRESULT CDoc::OnUpdateTags(WPARAM wParam, LPARAM lParam) 
  {
  UpdateAllViews(NULL, UPDATE_TAGS, (CObject*)lParam);
  SetModifiedFlag();
  return 0;
  }

void CDoc::OnUpdateall() 
  {
  UpdateAllViews(NULL, UPDATE_ALL, NULL);
  SetModifiedFlag();
  }

//void CDoc::OnUpdateallRqst() 
//  {
//  ScdMainWnd()->PostMessage(WM_COMMAND, ID_UPDATEALLRQST, 0);
////  UpdateAllViews(NULL, UPDATE_ALL, NULL);
////  SetModifiedFlag();
//  }
