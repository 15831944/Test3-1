//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  OPC ClientDoc.cpp
//  $Author    :  Jim Hansen
//
//  Description:  The OPC actions are mostly handled by this Doc.
//
//
//**************************************************************************
#include "stdafx.h"
#include "OPC Client.h"
#include "OPC ClientDoc.h"
#include "OPCServerDlg.h"
#include "ServerStatus.h"
#include "GroupParamsDlg.h"
#include "AddItemDlg.h"
#include "WriteItemDlg.h"
#include "ItemPropertiesDlg.h"
#include "ItemParamsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

OPCClientDoc* theDoc = NULL;
// The OPC data formats
UINT OPCSTMFORMATDATA = RegisterClipboardFormat(_T("OPCSTMFORMATDATA"));
UINT OPCSTMFORMATDATATIME = RegisterClipboardFormat(_T("OPCSTMFORMATDATATIME"));
UINT OPCSTMFORMATWRITECOMPLETE = RegisterClipboardFormat(_T("OPCSTMFORMATWRITECOMPLETE"));

#define FULL_TEST 1 // define this to test more interfaces

/////////////////////////////////////////////////////////////////////////////
// OPCClientDoc

IMPLEMENT_DYNCREATE(OPCClientDoc, CDocument)

BEGIN_MESSAGE_MAP(OPCClientDoc, CDocument)
   //{{AFX_MSG_MAP(OPCClientDoc)
   ON_COMMAND(ID_OPC_CONNECT, OnOpcConnect)
   ON_UPDATE_COMMAND_UI(ID_OPC_CONNECT, OnUpdateOpcConnect)
   ON_COMMAND(ID_OPC_DISCONNECT, OnOpcDisconnect)
   ON_UPDATE_COMMAND_UI(ID_OPC_DISCONNECT, OnUpdateOpcDisconnect)
   ON_COMMAND(ID_OPC_SERVERSTATUS, OnOpcServerstatus)
   ON_UPDATE_COMMAND_UI(ID_OPC_SERVERSTATUS, OnUpdateOpcServerstatus)
   ON_COMMAND(ID_OPC_GROUPPARAMETERS, OnOpcGroupparameters)
   ON_UPDATE_COMMAND_UI(ID_OPC_GROUPPARAMETERS, OnUpdateOpcGroupparameters)
   ON_COMMAND(ID_OPC_ADDITEM, OnOpcAdditem)
   ON_UPDATE_COMMAND_UI(ID_OPC_ADDITEM, OnUpdateOpcAdditem)
   ON_COMMAND(ID_OPC_ITEMPARAMETERS, OnOpcItemProperties)
   ON_UPDATE_COMMAND_UI(ID_OPC_ITEMPARAMETERS, OnUpdateOpcItemProperties)
   ON_COMMAND(ID_OPC_ITEMATTRIBUTES, OnOpcItemattributes)
   ON_UPDATE_COMMAND_UI(ID_OPC_ITEMATTRIBUTES, OnUpdateOpcItemattributes)
   ON_COMMAND(ID_OPC_WRITEVALUETOITEM, OnOpcWritevaluetoitem)
   ON_UPDATE_COMMAND_UI(ID_OPC_WRITEVALUETOITEM, OnUpdateOpcWritevaluetoitem)
   ON_COMMAND(ID_OPC_READITEM, OnOpcReaditem)
   ON_UPDATE_COMMAND_UI(ID_OPC_READITEM, OnUpdateOpcReaditem)
   ON_COMMAND(ID_OPC_REFRESH, OnOpcRefresh)
   ON_UPDATE_COMMAND_UI(ID_OPC_REFRESH, OnUpdateOpcRefresh)
   ON_COMMAND(ID_OPC_REMOVEITEM, OnOpcRemoveitem)
   ON_UPDATE_COMMAND_UI(ID_OPC_REMOVEITEM, OnUpdateOpcRemoveitem)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//**************************************************************************
// OPCClientDoc construction/destruction

OPCClientDoc::OPCClientDoc()
{
   groupHandle = 0;
   pCurrentItem = NULL;
   hView = NULL;
   transactionID = 0;
   dwConnection1 = 0;
   dwConnection2 = 0;
   testSink = new CAdviseSink;   // create the advise sink for notifications
   testSink->AddRef();

   callbackCP = new OPCCallbackObject;   // create the ConnectionPoint for notifications
   callbackCP->AddRef();
   shutdownCP = new OPCShutdownObject;   // create the ConnectionPoint for notifications
   shutdownCP->AddRef();
   dwShutdownConnection = 0;
   usingCP = FALSE;

   AfxOleLockApp();
   theDoc = this;

   // Everyone can connect back to IAdviseSink
   HRESULT hr = CoInitializeSecurity(NULL, -1, NULL, NULL,
      RPC_C_AUTHN_LEVEL_NONE, RPC_C_IMP_LEVEL_IDENTIFY, NULL, EOAC_NONE, NULL);
   if (FAILED(hr))
   {
      TRACE(_T("CoInitializeSecurity failed, %lx"), hr);
   }

   // Get last server, node, and comments
   lastServer = AfxGetApp()->GetProfileString(_T("Recent"), _T("Server"));
   lastNode = AfxGetApp()->GetProfileString(_T("Recent"), _T("Node"));
}

OPCClientDoc::~OPCClientDoc()
{
   AfxOleUnlockApp();
   if( opcServer.IsOk() )
      OnOpcDisconnect();
   testSink->Release(); // OLE should clean this up, but may not have time!
   callbackCP->Release();
   shutdownCP->Release();
   Sleep( 100 );

   AfxGetApp()->WriteProfileString(_T("Recent"), _T("Server"), lastServer);
   AfxGetApp()->WriteProfileString(_T("Recent"), _T("Node"), lastNode);
}

BOOL OPCClientDoc::OnNewDocument()
{
   if (!CDocument::OnNewDocument())
      return FALSE;

   return TRUE;
}

//**************************************************************************
// OPCClientDoc serialization

void OPCClientDoc::Serialize(CArchive& ar)
{
   if (ar.IsStoring())
   {
      // TODO: add storing code here
   }
   else
   {
      // TODO: add loading code here
   }
}

//**************************************************************************
// OPCClientDoc diagnostics

#ifdef _DEBUG
void OPCClientDoc::AssertValid() const
{
   CDocument::AssertValid();
}

void OPCClientDoc::Dump(CDumpContext& dc) const
{
   CDocument::Dump(dc);
}
#endif //_DEBUG

//**************************************************************************
// OnOpcConnect()
// Called to find a server name from the prompt dialog box,
//    and connect to that server.
// The node name is used to connect to servers on remote nodes.
//
//**************************************************************************
void OPCClientDoc::OnOpcConnect()
{
USES_CONVERSION;
   ASSERT( !opcServer.IsOk() );
   HRESULT hr = S_OK;

   // Prompt user for the server name (from list of installed servers)
   OPCServerDlg dlg;
   dlg.m_Server = lastServer;
   dlg.m_Node   = lastNode;
   if( dlg.DoModal() != IDOK )
      return;

   CWaitCursor wait;
   lastServer = dlg.m_Server;
   lastNode   = dlg.m_Node;

   // Check, if the specified node is the local node
   // In this case continue as if no node has been specified.
   BOOL useNode = TRUE;
   if ( dlg.m_Node.IsEmpty()
    || _tcsicmp(_T(""), dlg.m_Node) == 0
    || _tcsicmp(_T("127.0.0.1"), dlg.m_Node) == 0
    || _tcsicmp(_T("localhost"), dlg.m_Node) == 0)
      useNode = FALSE;
   if( useNode )
   {
      TCHAR szLocalHost[MAX_COMPUTERNAME_LENGTH + 1]; // my computer name
      DWORD dwHostSize = sizeof(szLocalHost);
      // get own computer name
      if (GetComputerName(szLocalHost, &dwHostSize) == TRUE)
         if (_tcsicmp(szLocalHost, dlg.m_Node) == 0 )
            useNode = FALSE;
   }


   // Create a running object from the class ID
   // (CLSCTX_ALL will allow in-proc, local and remote)
   LPUNKNOWN pUnkn = NULL;
   if( !useNode )
   {
      hr = CoCreateInstance(dlg.m_clsid, NULL, CLSCTX_ALL, IID_IUnknown, (LPVOID *)&pUnkn);
      if( FAILED(hr) || pUnkn == NULL)
      {
         CString format( (LPCSTR)IDS_CONNECT_ERROR );
         CString msg;
         msg.Format( format, dlg.m_Server );
         msg += _T("CoCreateInstance: ");
         ReportError( msg, hr );
         return;
      }
   }
   else  // use the node name
   {
      COSERVERINFO si;
      MULTI_QI  qi;

      si.dwReserved1 = 0;
      si.pwszName = T2OLE(dlg.m_Node.GetBuffer(0));
      si.pAuthInfo = NULL;
      si.dwReserved2 = 0;

      qi.pIID = &IID_IOPCServer;
      qi.pItf = NULL;
      qi.hr = 0;

      hr = CoCreateInstanceEx(dlg.m_clsid, NULL, CLSCTX_REMOTE_SERVER, &si, 1, &qi);
      if (FAILED(hr))
      {
         CString format( (LPCSTR)IDS_CONNECT_ERROR );
         CString msg;
         msg.Format( format, dlg.m_Server );
         msg += _T("CoCreateInstance: ");
         ReportError( msg, hr );
         return;
      }
      if (FAILED(qi.hr))
      {
         ReportError( _T("MultiQI: "), qi.hr );
         return;
      }
      pUnkn = qi.pItf;
   }

   // Get the IOPCServer interface.
   hr = opcServer.Attach( pUnkn );
   pUnkn->Release();  // Don't need this anymore.
   pUnkn = NULL;
   if( FAILED(hr) )
   {
      CString appName((LPCSTR)AFX_IDS_APP_TITLE);
      MessageBox(0, _T("You may not have registered the OPC Proxy dll!\n"), appName, MB_OK);
      return;
   }

   SetTitle( dlg.m_Server );  // display the server name

   // OPC 2.0 Server shutdown ConnectionPoint
   hr = AtlAdvise( opcServer, shutdownCP->GetUnknown(),IID_IOPCShutdown, &dwShutdownConnection );

   // Create a single group that will contain all the items
   FLOAT deadband = 0.0;
   DWORD rate;
   hr = opcServer.AddGroup( L"Fred", TRUE, 1000,  // name, active, rate
                             1324, NULL, &deadband,  // handle, bias, band
                             0, &groupHandle, &rate,
                             IID_IOPCGroupStateMgt,   // interface to return
                             opcGroup );              // this holds the group ptr
   if( FAILED(hr) )
   {
      ReportError( _T("AddGroup: "), hr );
      return;
   }

   // Test GetGroupByName and SetName
#ifdef FULL_TEST
   IOPCGroupStateMgt* pTest=NULL;
   hr = opcServer.GetGroupByName( L"Fred", IID_IOPCGroupStateMgt, (LPUNKNOWN*)&pTest );
   if( SUCCEEDED(hr) )
   {
      ASSERT( pTest == (IOPCGroupStateMgt*)opcGroup );   // should get the same
      hr = pTest->SetName( L"Group one" );               // set new name
      pTest->Release();
      if( FAILED(hr) )
      {
         ReportError( _T("IOPCGroupStateMgt::SetName: "), hr );
      }
      else
      {
         // should now go by this new name
         hr = opcServer.GetGroupByName( L"Group one", IID_IOPCGroupStateMgt, (LPUNKNOWN*)&pTest );
         if( SUCCEEDED(hr) )
         {
            ASSERT( pTest == (IOPCGroupStateMgt*)opcGroup );
            pTest->Release();
         }
      }
   }
#endif // FULL_TEST

   // OPC 2.0 ConnectionPoints
   hr = AtlAdvise( opcGroup, callbackCP->GetUnknown(),IID_IOPCDataCallback, &dwConnection1 );
   if( SUCCEEDED(hr) )  // This server supports 2.0
      usingCP = TRUE;

   if( !usingCP )
   {
      // OPC 1.0 data advise format
      FORMATETC formatEtc ;

      formatEtc.tymed =  TYMED_HGLOBAL;
      formatEtc.ptd = NULL;
      formatEtc.dwAspect = DVASPECT_CONTENT;
      formatEtc.lindex = -1;

      // IAdviseSink is an interface on OUR object that is passed to
      // the server for callbacks
      IAdviseSink *pAdviseSink = NULL;
      hr = testSink->QueryInterface(IID_IAdviseSink, (LPVOID *)&pAdviseSink);
      if( FAILED(hr) )
      {
         ReportError( _T("IAdviseSink: "), hr );
         opcGroup.Detach();
         opcServer.Detach();
         return;
      }

      // Get an IDataObject interface on the group
      DataObject dataObject;
      hr = dataObject.Attach( opcGroup );
      if(FAILED(hr) || !dataObject.IsOk() )
      {
         //  some servers don't do this, so don't quit altogether
         MessageBox( 0, _T("IDataObject not supported by this server\nNo data notifications will take place"), _T("FactorySoft Client"), MB_OK );
         return;
      }

      // Register our IAdvise with the group
      // Need to register both formats: data change, and write complete
      formatEtc.cfFormat = OPCSTMFORMATWRITECOMPLETE ;
      hr = dataObject.DAdvise(&formatEtc,
                                ADVF_PRIMEFIRST,    // ADVF flag
                                pAdviseSink,
                                &dwConnection2);
      if( FAILED(hr) )
      {
         ReportError( _T("IDataObject::DAdvise: "), hr );
         return;
      }

#ifdef DATATIMEFORMAT
      formatEtc.cfFormat = OPCSTMFORMATDATATIME ;
#else
      formatEtc.cfFormat = OPCSTMFORMATDATA ;
#endif // DATATIMEFORMAT
      hr = dataObject.DAdvise(&formatEtc,
                                ADVF_PRIMEFIRST,    // ADVF flag
                                pAdviseSink,
                                &dwConnection1);
      pAdviseSink->Release();
      if( FAILED(hr) )
      {
         ReportError( _T("IDataObject::DAdvise: "), hr );
         return;
      }
   }
}

void OPCClientDoc::OnUpdateOpcConnect(CCmdUI* pCmdUI)
{
   pCmdUI->Enable( !opcServer.IsOk() );
}

//*******************************************************************
void OPCClientDoc::OnOpcDisconnect()
{
   CWaitCursor wait;
   HRESULT hr = S_OK;
   if( opcServer.IsOk() && opcGroup.IsOk() )
   {
      if( dwShutdownConnection )
      {
         hr = AtlUnadvise( opcServer,IID_IOPCShutdown, dwShutdownConnection );
      }
      if( usingCP )
      {
         // OPC 2.0 ConnectionPoints
         hr = AtlUnadvise( opcGroup,IID_IOPCDataCallback, dwConnection1 );
      }
      else
      // call IDataObject::DUnadvise to turn off data notification
      {
          DataObject dataObject;
          HRESULT hr = dataObject.Attach( opcGroup );
          if( SUCCEEDED(hr) )
          {
             if( dwConnection1 )
                hr = dataObject.DUnadvise(dwConnection1);
             if( dwConnection2 )
                hr = dataObject.DUnadvise(dwConnection2);
             dataObject.Detach();
          }
       }
       // test RemoveItems (RemoveGroup cleans up anyway.)
#ifdef FULL_TEST
       OPCItemMgt itemMgt;
       hr = itemMgt.Attach( opcGroup );
       if( SUCCEEDED(hr) && items.GetCount()>0 )
       {
          HRESULT *pErrors=0;
          OPCHANDLE* handles = new OPCHANDLE[items.GetCount()];
          POSITION pos = items.GetHeadPosition();
          for( int index=0; pos; index++ )
          {
             Item* pItem = items.GetNext( pos );
             handles[index] = pItem->hServerHandle;
          }
          hr = itemMgt.RemoveItems( items.GetCount(), handles, &pErrors );
          if( SUCCEEDED(hr) )
             CoTaskMemFree( pErrors );
          delete [] handles;
       }
#endif // FULL_TEST

       opcServer.RemoveGroup(groupHandle, FALSE);

   }
   opcGroup.Detach();
   opcServer.Detach();

   Sleep( 100 );

   // now that the group is released and unadvised, no more data will
   // be sent from the server. It is safe to delete the items
   while( !items.IsEmpty() )
      delete items.RemoveTail();
   pCurrentItem = NULL;
   UpdateAllViews(NULL);
   SetTitle( _T("Unconnected ") );
}

void OPCClientDoc::OnUpdateOpcDisconnect(CCmdUI* pCmdUI)
{
   pCmdUI->Enable( opcServer.IsOk() );
}

//*******************************************************************
void OPCClientDoc::OnOpcServerstatus()
{
   ASSERT( opcServer.IsOk() );
   CServerStatus dlg(opcServer);
   dlg.DoModal();
}

void OPCClientDoc::OnUpdateOpcServerstatus(CCmdUI* pCmdUI)
{
   pCmdUI->Enable( opcServer.IsOk() );
}

//*******************************************************************
void OPCClientDoc::OnOpcGroupparameters()
{
   GroupParamsDlg dlg( opcGroup );
   dlg.DoModal();
}

void OPCClientDoc::OnUpdateOpcGroupparameters(CCmdUI* pCmdUI)
{
   pCmdUI->Enable( opcGroup.IsOk() );
}

//*******************************************************************
void OPCClientDoc::OnOpcAdditem()
{
   ASSERT( opcGroup.IsOk() );

   AddItemDlg dlg( this, opcServer );
   dlg.DoModal();
}

void OPCClientDoc::OnUpdateOpcAdditem(CCmdUI* pCmdUI)
{
   pCmdUI->Enable( opcServer.IsOk() && opcGroup.IsOk() );
}

//*******************************************************************
void OPCClientDoc::AddItem(LPCTSTR itemID, LPCTSTR accessPath, VARTYPE type)
{
USES_CONVERSION;
   CWaitCursor wait;
   // get an interface pointer
   OPCItemMgt itemMgt;
   HRESULT hr = itemMgt.Attach( opcGroup );
   if( FAILED(hr) )
      return;

   // Add a new item
   Item* item = new Item;
   item->quality = OPC_QUALITY_GOOD;
   item->name = itemID;

   // fill out its definition
   OPCITEMDEF idef;
   idef.szItemID = T2OLE((LPTSTR)itemID);
   idef.dwBlobSize = 0;
   idef.pBlob = NULL;
   idef.bActive = TRUE;
   idef.hClient = (OPCHANDLE)item;  // pointer to item is its "handle"
   idef.szAccessPath = T2OLE((LPTSTR)accessPath);
   idef.vtRequestedDataType = type;

   // add the item
   OPCITEMRESULT * pResults;
   HRESULT *pErrors;

   hr = itemMgt.AddItems(1, &idef, &pResults, &pErrors);
   if( FAILED( hr ) )   // if the call failed, get out
   {
      ReportError( _T("AddItems: "), hr );
      delete item;
      return;
   }
   // If the call was successful, memory must be cleaned up
   item->hServerHandle = pResults->hServer;  // save the server handle
   type = pResults->vtCanonicalDataType;
   // Save the result in pErrors before freeing
   HRESULT itemResult = pErrors[0];          // and the item's result
   if( pResults->pBlob != NULL )
      CoTaskMemFree( pResults->pBlob );
   CoTaskMemFree( pResults );
   CoTaskMemFree( pErrors );

   // It this item failed, don't keep it (the server didn't)
   if( FAILED(itemResult) )
   {
      ReportError( _T("AddItems: "), itemResult );
      delete item;
      return;
   }

   items.AddTail( item );  // store this item in the item list

   // Read its initial value
   OPCSyncIO opcSyncIO;
   if( opcSyncIO.Attach( opcGroup ) == S_OK )
   {
      OPCITEMSTATE* pItemState;
      hr = opcSyncIO.Read(OPC_DS_CACHE, 1, &item->hServerHandle, &pItemState, &pErrors);
      if( SUCCEEDED(hr) )
      {
         ASSERT( pItemState->hClient == (OPCHANDLE)item );
         item->quality = pItemState->wQuality;
         item->value = pItemState->vDataValue;
         VariantClear( &pItemState->vDataValue );
         CoTaskMemFree( pItemState );
         CoTaskMemFree( pErrors );
      }
      else
      {
         ReportError( _T("Sync Read: "), hr );
         return;
      }
   }

   // test some more interfaces
#ifdef FULL_TEST
   if( itemMgt.IsOk() )
   {
      hr = itemMgt.SetActiveState( 1, &item->hServerHandle, FALSE, &pErrors);
      if( SUCCEEDED(hr) )
         CoTaskMemFree( pErrors );
      hr = itemMgt.SetActiveState( 1, &item->hServerHandle, TRUE, &pErrors);
      if( SUCCEEDED(hr) )
         CoTaskMemFree( pErrors );
      hr = itemMgt.SetClientHandles( 1, &item->hServerHandle, (OPCHANDLE*)&item, &pErrors);
      if( SUCCEEDED(hr) )
         CoTaskMemFree( pErrors );
   }
#endif // FULL_TEST

   UpdateAllViews(NULL);
}
//*******************************************************************
// This is the 2.0 Item Properties interface
void OPCClientDoc::OnOpcItemProperties()
{
    CItemPropertiesDlg dlg(pCurrentItem->name);
    dlg.SetOPCPointer( (IUnknown*)opcServer );
    dlg.DoModal();
}

void OPCClientDoc::OnUpdateOpcItemProperties(CCmdUI* pCmdUI)
{
   pCmdUI->Enable( opcServer.IsOk() && pCurrentItem );
}

//*******************************************************************
void OPCClientDoc::OnOpcItemattributes()
{
   // get an interface pointer
   OPCItemMgt itemMgt;
   HRESULT hr = itemMgt.Attach( opcGroup );
   if( FAILED(hr) )
      return;

   // test the item attributes enumerator
   EnumOPCItemAttributes enumerator;
   hr = itemMgt.CreateEnumerator( IID_IEnumOPCItemAttributes, enumerator );
   while( hr==S_OK && enumerator.IsOk() )
   {
      OPCITEMATTRIBUTES* pAttributes = NULL;
      ULONG count=0;
      hr = enumerator.Next( 1, &pAttributes, &count );
      if( hr == S_OK )
      {
         if( pAttributes->hClient == (OPCHANDLE)pCurrentItem )
         {
            // found the matching attributes for this item
            ItemParamsDlg dlg;
            dlg.pItemAttr = pAttributes;
            if( dlg.DoModal() == IDOK )
            {
               HRESULT *pErrors;
               hr = itemMgt.SetActiveState( 1, &pCurrentItem->hServerHandle, dlg.m_active, &pErrors);
               if( SUCCEEDED(hr) )
                  CoTaskMemFree( pErrors );
            }
         }
         VariantClear( &pAttributes->vEUInfo );
         CoTaskMemFree( pAttributes->szAccessPath );
         CoTaskMemFree( pAttributes->szItemID );
         if( pAttributes->pBlob )
            CoTaskMemFree( pAttributes->pBlob );
         CoTaskMemFree( pAttributes );
      }
      else if( FAILED(hr) )
         ReportError( _T("EnumOPCItemAttributes: "), hr );
   }
}

//*******************************************************************
void OPCClientDoc::OnUpdateOpcItemattributes(CCmdUI* pCmdUI)
{
   pCmdUI->Enable( opcServer.IsOk() && pCurrentItem );
}
//*******************************************************************
// the user enters something to write to the item in the dialog.
// COleVariant's ChangeType converts the string into the proper format
// or else puts up a dialog that it couldn't convert.
// Write can be sync or async
void OPCClientDoc::OnOpcWritevaluetoitem()
{
   WriteItemDlg dlg;
   dlg.m_item = pCurrentItem->name;
   if( dlg.DoModal() == IDOK )
   {
      CWaitCursor wait;
      HRESULT *pErrors = NULL;
      if( dlg.m_async )
      {
          if( usingCP )
          {
             OPCAsyncIO2 opcAsyncIO2;
             if( opcAsyncIO2.Attach( opcGroup ) == S_OK )
             {
                transactionID = 2;   // any number the client wants
                COleVariant vt( dlg.m_value );   // initialize as a string
                vt.ChangeType( pCurrentItem->value.vt );  // let COleVariant convert!
                HRESULT hr = opcAsyncIO2.Write(1, &pCurrentItem->hServerHandle,
                                              vt, transactionID, &transactionID, &pErrors);
                if( SUCCEEDED(hr) )
                {
                   if( FAILED(pErrors[0]) )
                   {
                      ReportError( _T("ASync Write: "), pErrors[0] );
                   }
                   CoTaskMemFree( pErrors );
                }
                else
                {
                   ReportError( _T("ASync Write: "), hr );
                }
             }
         }
         else
         {
             OPCAsyncIO opcAsyncIO;
             if( opcAsyncIO.Attach( opcGroup ) == S_OK )
             {
                COleVariant vt( dlg.m_value );   // initialize as a string
                vt.ChangeType( pCurrentItem->value.vt );  // let COleVariant convert!
                HRESULT hr = opcAsyncIO.Write(dwConnection2, 1, &pCurrentItem->hServerHandle,
                                              vt, &transactionID, &pErrors);
                if( SUCCEEDED(hr) )
                {
                   if( FAILED(pErrors[0]) )
                   {
                      ReportError( _T("ASync Write: "), pErrors[0] );
                   }
                   CoTaskMemFree( pErrors );
                }
                else
                {
                   ReportError( _T("ASync Write: "), hr );
                }
             }
         }
      }
      else
      {
         OPCSyncIO opcSyncIO;
         if( opcSyncIO.Attach( opcGroup ) == S_OK )
         {
            COleVariant vt( dlg.m_value );   // initialize as a string
            vt.ChangeType( pCurrentItem->value.vt );  // let COleVariant convert!
            HRESULT hr = opcSyncIO.Write( 1, &pCurrentItem->hServerHandle, vt, &pErrors);
            if( SUCCEEDED(hr) )
            {
               if( FAILED(pErrors[0]) )
               {
                  ReportError( _T("Sync Write: "), pErrors[0] );
               }
               CoTaskMemFree( pErrors );
            }
            else
            {
               ReportError( _T("Sync Write: "), hr );
            }
         }
      }
   }
}

void OPCClientDoc::OnUpdateOpcWritevaluetoitem(CCmdUI* pCmdUI)
{
   pCmdUI->Enable( opcServer.IsOk() && pCurrentItem );
}

//*******************************************************************
// Test async reading by calling 4 times (server will queue them up)
void OPCClientDoc::OnOpcReaditem()
{
   HRESULT *pErrors;
   CWaitCursor wait;
   {  // also test sync read since data is returned differently
      OPCSyncIO opcSyncIO;
      if( opcSyncIO.Attach( opcGroup ) == S_OK )
      {
         OPCITEMSTATE* pItemState;
         HRESULT hr = opcSyncIO.Read(OPC_DS_DEVICE, 1, &pCurrentItem->hServerHandle, &pItemState, &pErrors);
         if( SUCCEEDED(hr) )
         {
            if( FAILED(pErrors[0]) )
            {
               ReportError( _T("Sync Read: "), pErrors[0] );
            }
            else
            {
               pCurrentItem->quality = pItemState->wQuality;
               pCurrentItem->value = pItemState->vDataValue;
            }
            VariantClear( &pItemState->vDataValue );
            CoTaskMemFree( pItemState );
            CoTaskMemFree( pErrors );
         }
         else
         {
            ReportError( _T("Sync Read: "), hr );
         }
      }
   }

   // Use AsyncIO2 here
   if( usingCP )
   {
      OPCAsyncIO2 opcAsyncIO2;
      if( opcAsyncIO2.Attach( opcGroup ) == S_OK )
      {
         transactionID = 3;
         HRESULT hr = opcAsyncIO2.Read(1, &pCurrentItem->hServerHandle, transactionID, &transactionID, &pErrors);
         if( SUCCEEDED(hr) )
         {
            if( FAILED(pErrors[0]) )
            {
               ReportError( _T("Async Read: "), pErrors[0] );
            }
            CoTaskMemFree( pErrors );
         }
         else
         {
            ReportError( _T("Async Read: "), hr );
         }
#ifdef FULL_TEST
         transactionID = 3;
         hr = opcAsyncIO2.Read(1, &pCurrentItem->hServerHandle, transactionID, &transactionID, &pErrors);
         if( SUCCEEDED(hr) )
         {
            CoTaskMemFree( pErrors );
         }
         transactionID = 3;
         hr = opcAsyncIO2.Read(1, &pCurrentItem->hServerHandle, transactionID, &transactionID, &pErrors);
         if( SUCCEEDED(hr) )
         {
            CoTaskMemFree( pErrors );
         }
         // ask for a few this time (it doesn't matter that its the same one)
         transactionID = 3;
         OPCHANDLE serverHandles[3];
         for( int i=0; i<3; i++ )
            serverHandles[i] = pCurrentItem->hServerHandle;
         hr = opcAsyncIO2.Read(3, serverHandles, transactionID, &transactionID, &pErrors);
         if( SUCCEEDED(hr) )
         {
            CoTaskMemFree( pErrors );
         }
#endif // FULL_TEST
      }
   }
   else
   {
      OPCAsyncIO opcAsyncIO;
      if( opcAsyncIO.Attach( opcGroup ) == S_OK )
      {
         HRESULT hr = opcAsyncIO.Read(dwConnection1, OPC_DS_CACHE, 1, &pCurrentItem->hServerHandle, &transactionID, &pErrors);
         if( SUCCEEDED(hr) )
         {
            if( FAILED(pErrors[0]) )
            {
               ReportError( _T("Async Read: "), pErrors[0] );
            }
            CoTaskMemFree( pErrors );
         }
         else
         {
            ReportError( _T("Async Read: "), hr );
         }
#ifdef FULL_TEST
         hr = opcAsyncIO.Read(dwConnection1, OPC_DS_DEVICE, 1, &pCurrentItem->hServerHandle, &transactionID, &pErrors);
         if( SUCCEEDED(hr) )
         {
            CoTaskMemFree( pErrors );
         }
         hr = opcAsyncIO.Read(dwConnection1, OPC_DS_CACHE, 1, &pCurrentItem->hServerHandle, &transactionID, &pErrors);
         if( SUCCEEDED(hr) )
         {
            CoTaskMemFree( pErrors );
         }
         // ask for a few this time (it doesn't matter that its the same one)
         OPCHANDLE serverHandles[3];
         for( int i=0; i<3; i++ )
            serverHandles[i] = pCurrentItem->hServerHandle;
         hr = opcAsyncIO.Read(dwConnection1, OPC_DS_DEVICE, 3, serverHandles, &transactionID, &pErrors);
         if( SUCCEEDED(hr) )
         {
            CoTaskMemFree( pErrors );
         }
#endif // FULL_TEST
      }
   }
   UpdateAllViews(NULL);
}

void OPCClientDoc::OnUpdateOpcReaditem(CCmdUI* pCmdUI)
{
   pCmdUI->Enable( opcServer.IsOk() && pCurrentItem );
}

//*******************************************************************
// Refresh causes all items to be advised immediately.
void OPCClientDoc::OnOpcRefresh()
{
   CWaitCursor wait;
   if( usingCP )
   {
      OPCAsyncIO2 opcAsyncIO2;
      if( opcAsyncIO2.Attach( opcGroup ) == S_OK )
      {
         // test both Device and Cache data sources
         // This should cause the server to queue a refresh while one is going on.
         transactionID = 2;   // any number the client wants
         HRESULT hr = opcAsyncIO2.Refresh2(OPC_DS_DEVICE, transactionID, &transactionID);
         if( FAILED(hr) )
         {
            ReportError( _T("Refresh2: "), hr );
            return;
         }
   #ifdef FULL_TEST
         transactionID = 3;   // any number the client wants
         hr = opcAsyncIO2.Refresh2(OPC_DS_DEVICE, transactionID, &transactionID);
         if( FAILED(hr) )
         {
            ReportError( _T("Refresh: "), hr );
            return;
         }
   #endif // FULL_TEST
      }
   }
   else
   {
      OPCAsyncIO opcAsyncIO;
      if( opcAsyncIO.Attach( opcGroup ) == S_OK )
      {
         // test both Device and Cache data sources
         // This should cause the server to queue a refresh while one is going on.
         HRESULT hr = opcAsyncIO.Refresh(dwConnection1, OPC_DS_DEVICE, &transactionID);
         if( FAILED(hr) )
         {
            ReportError( _T("Refresh: "), hr );
            return;
         }
   #ifdef FULL_TEST
         hr = opcAsyncIO.Refresh(dwConnection1, OPC_DS_CACHE, &transactionID);
         if( FAILED(hr) )
         {
            ReportError( _T("Refresh: "), hr );
            return;
         }
   #endif // FULL_TEST
      }
   }
}

void OPCClientDoc::OnUpdateOpcRefresh(CCmdUI* pCmdUI)
{
   pCmdUI->Enable( opcServer.IsOk() );
}


//*******************************************************************
void OPCClientDoc::OnOpcRemoveitem()
{
   OPCItemMgt itemMgt;
   HRESULT hr = itemMgt.Attach( opcGroup );
   if( SUCCEEDED(hr) )
   {
      HRESULT *pErrors=0;
      OPCHANDLE handle = pCurrentItem->hServerHandle;
      hr = itemMgt.RemoveItems( 1, &handle, &pErrors );
      if( SUCCEEDED(hr) )
         CoTaskMemFree( pErrors );
      POSITION pos = items.Find( pCurrentItem );
      items.RemoveAt( pos );
      delete pCurrentItem;
      pCurrentItem = NULL;
      UpdateAllViews(NULL);
   }
}

void OPCClientDoc::OnUpdateOpcRemoveitem(CCmdUI* pCmdUI)
{
   pCmdUI->Enable( opcServer.IsOk() && pCurrentItem );
}

//*******************************************************************
CString OPCClientDoc::GetErrorString(LPCTSTR description, HRESULT error)
{
   CString temp(description);
   temp += _T(" ");
   TCHAR msg[MAX_PATH*5];
   DWORD ok = FormatMessage( FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,
                  NULL,
                  error,
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
                  msg, MAX_PATH, NULL );
   if( ok )
   {
      TRACE(msg);
      TRACE(_T("\n"));
      temp += msg;
      return temp;
   }

   // else try the server
   LPWSTR pString = NULL;
   if( opcServer.IsOk() )
   {
      HRESULT hr = opcServer.GetErrorString( error, LOCALE_SYSTEM_DEFAULT, &pString );
      if( SUCCEEDED(hr) )
      {
         temp += pString;
         TRACE(temp);
         CoTaskMemFree( pString );
      }
      else
         temp += _T("OPC Server failed GetErrorString.");
   }
   else
      temp += _T("Unknown Error.");
   return temp;
}

//*******************************************************************
void OPCClientDoc::ReportError(LPCTSTR description, HRESULT error)
{
   CString msg( GetErrorString(description,error) );
   CString appName((LPCSTR)AFX_IDS_APP_TITLE);
   MessageBox(0, msg, appName, MB_OK);
}

//**************************************************************************
//
// VariantToString() convert the value to String format
//
//**************************************************************************
void VariantToString(VARIANT value, CString& strText)
{
    strText.Empty();
    switch(value.vt)
    {
        case VT_BOOL:
            strText = value.boolVal ? _T("On") : _T("Off");
            break;
        case VT_UI1:
            strText.Format( _T("%hd"), (USHORT)value.bVal );
            break;
        case VT_UI2:
        case VT_I2:
            strText.Format( _T("%hd"), value.iVal );
            break;
        case VT_I4:
            strText.Format( _T("%d"), value.lVal );
            break;
        case VT_R4:
            strText.Format( _T("%g"), value.fltVal );
            break;
        case VT_R8:
            strText.Format( _T("%g"), value.dblVal );
            break;
        case VT_BSTR:
            strText = value.bstrVal;
            break;
        default: // Arrays of simple types
        {
            if( (value.vt & VT_ARRAY)==VT_ARRAY )
            {
                CString temp;
                SAFEARRAY* pArray = value.parray;
                LONG lBound = 0, uBound = 0;
                SafeArrayGetLBound( pArray, 1, &lBound );
                SafeArrayGetUBound( pArray, 1, &uBound );
                for( long element=lBound; element<=uBound; element++ )
                {
                    if( strText.IsEmpty() )
                        strText.Format( _T("(%d) "), uBound-lBound+1);
                    else
                        strText += _T(", ");
                    switch( value.vt & ~VT_ARRAY )
                    {
                    case VT_BOOL:
                        {
                            VARIANT_BOOL b=0;
                            SafeArrayGetElement(pArray, &element, &b);
                            temp = b ? _T("1") : _T("0");
                        }
                        break;
                    case VT_UI1:
                        {
                            BYTE b=0;
                            SafeArrayGetElement(pArray, &element, &b);
                            temp.Format( _T("%hd"), b );
                        }
                        break;
                    case VT_UI2:
                    case VT_I2:
                        {
                            short b=0;
                            SafeArrayGetElement(pArray, &element, &b);
                            temp.Format( _T("%hd"), b );
                        }
                        break;
                    case VT_I4:
                        {
                            long b=0;
                            SafeArrayGetElement(pArray, &element, &b);
                            temp.Format( _T("%d"), b );
                        }
                        break;
                    case VT_R4:
                        {
                            float d=0;
                            SafeArrayGetElement(pArray, &element, &d);
                            temp.Format( _T("%g"), d );
                        }
                        break;
                    case VT_R8:
                        {
                            double d=0;
                            SafeArrayGetElement(pArray, &element, &d);
                            temp.Format( _T("%g"), d );
                        }
                        break;
                    case VT_BSTR:
                        {
                            BSTR b;
                            SafeArrayGetElement(pArray, &element, &b);
                            temp = b;
                        }
                        break;
                    }
                    strText += temp;
                }
            }
            else
                strText = _T("?");
        }
    }
}
