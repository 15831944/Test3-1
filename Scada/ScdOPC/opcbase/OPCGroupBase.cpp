//**************************************************************************
//
//            Copyright (c) FactorySoft, INC. 1997, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  OPCGroupBase.cpp
//  $Author    :  Jim Hansen
//
//  Subsystem  :  OPC
//  Version    :  2.04
//
//  Description:  The OPC Group base class. Implements the COM interfaces
//                and active threads. Override virtual functions in your
//                subclass.
//
//**************************************************************************

#include "stdafx.h"
#include "OPCServerBase.h"
#include "OPCGroupBase.h"
#if !defined(_WIN32_WCE)
#include <process.h>
#include <COMDEF.H>
#endif
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//*******************************************************************
//    class COPCItem
//*******************************************************************
COPCItem::COPCItem()
: active(FALSE), clientHandle(0), changed(TRUE),
quality(OPC_QUALITY_BAD), clientType( VT_EMPTY ),
accessRights(OPC_READABLE | OPC_WRITEABLE)
  {
  CoFileTimeNow( &timestamp );
  }

COPCItem::~COPCItem()
  {
  }

//*******************************************************************
//    class OPCGroupBase
//*******************************************************************
// Do the same for IDataObject::Advise...
STDMETHODIMP OPCGroupBase::Advise( IUnknown *pUnk, DWORD *pdwCookie)
  {
  HRESULT hr = DataCallbackCP::Advise(pUnk,pdwCookie);
  if( SUCCEEDED(hr) )
    SetEvent( m_hTimer );
  return hr;
  }

// Initialize does the real work
OPCGroupBase::OPCGroupBase()
: m_name(), m_active(FALSE), m_updateRate(1000), m_clientHandle(0), m_timeBias(0),
m_deadBand(0.0), m_LCID(0), m_running(TRUE), m_dataWaiting(FALSE), m_removed(FALSE),
m_hASyncThread(NULL), m_hDataThread(NULL), m_hTimer(0), m_hAsyncRequest(0),
m_transactionID(2), m_parent(NULL), m_bEnable(TRUE)
  {
  InitializeCriticalSection( &m_cs );
  InitializeCriticalSection( &m_IOcs );
  m_DataAdviseSink = NULL;
  m_DataTimeAdviseSink = NULL;
  m_AsyncAdviseSink = NULL;
  m_cmdWaiting = FALSE;
  m_ASYNC_REQUEST_LIMIT = 1000;
  TIME_ZONE_INFORMATION tz;
  GetTimeZoneInformation( &tz );
  m_timeBias = tz.Bias;
  }

//*******************************************************************
OPCGroupBase::~OPCGroupBase()
  {
  Shutdown();

  // remove all items
  COPCItem* pItem = NULL;
  LPVOID key = 0;
  EnterCriticalSection( &m_cs );
  POSITION pos = m_itemMap.GetStartPosition();
  while( pos )
    {
    m_itemMap.GetNextAssoc( pos, key, pItem );
    delete pItem;
    }
  m_itemMap.RemoveAll();
  LeaveCriticalSection( &m_cs );

  DeleteCriticalSection( &m_cs );
  DeleteCriticalSection( &m_IOcs );
  }

//*******************************************************************
void OPCGroupBase::Shutdown()
  {
  EnterCriticalSection( &m_cs );
  // End the threads
  m_active = FALSE;
  m_running = FALSE;
  SetEvent( m_hAsyncRequest );
  SetEvent( m_hTimer );
  LeaveCriticalSection( &m_cs );
  if( m_hASyncThread != NULL )
    {
    WaitForSingleObject( m_hASyncThread, INFINITE );
    CloseHandle( m_hASyncThread );
    m_hASyncThread = NULL;
    }
  if( m_hDataThread != NULL )
    {
    WaitForSingleObject( m_hDataThread, INFINITE );
    CloseHandle( m_hDataThread );
    m_hDataThread = NULL;
    }

  // Remove async requests
  EnterCriticalSection( &m_cs );
  while( m_asyncRequests.GetCount() > 0 )
    {
    ASyncRequest* pRequest = m_asyncRequests.RemoveTail();
    delete pRequest;
    }
  LeaveCriticalSection( &m_cs );

  if(m_DataAdviseSink)
    ASSERT(!m_DataAdviseSink->Release());
  m_DataAdviseSink = NULL;
  if(m_DataTimeAdviseSink)
    ASSERT(!m_DataTimeAdviseSink->Release());
  m_DataTimeAdviseSink = NULL;
  if(m_AsyncAdviseSink)
    ASSERT(!m_AsyncAdviseSink->Release());
  m_AsyncAdviseSink = NULL;

  CloseHandle( m_hTimer );
  m_hTimer = 0;
  CloseHandle( m_hAsyncRequest );
  m_hAsyncRequest = 0;
  }

//*******************************************************************
void OPCGroupBase::Initialize(LPCWSTR szName,
                              BOOL bActive,
                              DWORD dwRequestedUpdateRate,
                              OPCHANDLE hClientGroup,
                              LONG* pTimeBias,
                              FLOAT*percentDeadband,
                              DWORD dwLCID,
                              OPCServerBase* pServer)
  {
  m_parent = pServer;
  m_name = szName;
  m_active = bActive;
  m_updateRate = dwRequestedUpdateRate;
  DoSetUpdateRate( m_updateRate );
  m_clientHandle = hClientGroup;
  if( pTimeBias )
    m_timeBias = *pTimeBias;
  if( percentDeadband )
    m_deadBand = *percentDeadband;
  m_LCID = dwLCID;

  // event to signal data scan thread
  m_hTimer = CreateEvent( NULL, FALSE, FALSE, NULL );
  ASSERT( m_hTimer );
  // event to signal async thread
  m_hAsyncRequest = CreateEvent( NULL, FALSE, FALSE, NULL );
  ASSERT( m_hAsyncRequest );

  // start the data scan thread
  SECURITY_ATTRIBUTES sa;
  sa.nLength = sizeof(sa);
  sa.lpSecurityDescriptor = NULL;
  DWORD threadId=0;
  m_hDataThread = CreateThread( &sa, 0,
    (LPTHREAD_START_ROUTINE)&ThreadStub,
    this,
    0,
    &threadId);
  ASSERT( m_hDataThread != NULL );
  }

//*******************************************************************
STDMETHODIMP OPCGroupBase::GetState(
                                    DWORD     * pUpdateRate,
                                    BOOL      * pActive,
                                    LPWSTR    * ppName,
                                    LONG      * pTimeBias,
                                    FLOAT     * pPercentDeadband,
                                    DWORD     * pLCID,
                                    OPCHANDLE * phClientGroup,
                                    OPCHANDLE * phServerGroup)
  {
  if( m_removed )  return E_FAIL;
  if( pUpdateRate )
    *pUpdateRate   = m_updateRate;
  if( pActive )
    *pActive       = m_active;
  if( pTimeBias )
    *pTimeBias     = m_timeBias;
  if( pPercentDeadband )
    *pPercentDeadband = m_deadBand;
  if( pLCID )
    *pLCID         = m_LCID;
  if( phClientGroup )
    *phClientGroup = m_clientHandle;
  if( phServerGroup )
    *phServerGroup = (OPCHANDLE)this;
  if( ppName )
    {
    *ppName     = (LPWSTR)CoTaskMemAlloc( 2*(m_name.GetLength()+1) );
    if( *ppName == NULL )
      return E_OUTOFMEMORY;
    USES_CONVERSION;
    wcscpy( *ppName, T2OLE(m_name.GetBuffer(0)) );
    }
  return S_OK;
  }

//*******************************************************************
STDMETHODIMP OPCGroupBase::SetState(
                                    DWORD     * pRequestedUpdateRate,
                                    DWORD     * pRevisedUpdateRate,
                                    BOOL      * pActive,
                                    LONG      * pTimeBias,
                                    FLOAT     * pPercentDeadband,
                                    DWORD     * pLCID,
                                    OPCHANDLE * phClientGroup)
  {
  if( pRevisedUpdateRate==NULL )
    return E_POINTER;
  if( m_removed )   return E_FAIL;

  HRESULT hr = S_OK;
  if( pRequestedUpdateRate )
    {
    //kga : possible bug here as *pRequestedUpdateRate seems to always be 0 !!!
    DoSetUpdateRate( *pRequestedUpdateRate );
    if( m_updateRate != *pRequestedUpdateRate )
      hr = OPC_S_UNSUPPORTEDRATE;
    }
  *pRevisedUpdateRate = m_updateRate;
  if( pTimeBias )
    m_timeBias = *pTimeBias;
  if( pPercentDeadband )
    m_deadBand = *pPercentDeadband;
  if( pLCID )
    m_LCID = *pLCID;
  if( phClientGroup )
    m_clientHandle = *phClientGroup;

  BOOL activeChanged = FALSE;
  if( pActive )
    if( m_active != *pActive )   // change of state (to active) is an advise condition
      {
      activeChanged = TRUE;
      m_active = *pActive;
      if( m_active )
        {
        m_dataWaiting = TRUE;
        return hr;
        }
      else  // Set all items to bad quality
        {
        EnterCriticalSection( &m_cs );
        COPCItem* pItem = NULL;
        LPVOID key = 0;
        POSITION pos = m_itemMap.GetStartPosition();
        while( pos )
          {
          m_itemMap.GetNextAssoc( pos, key, (COPCItem*&)pItem );
          pItem->quality = OPC_QUALITY_BAD | OPC_QUALITY_OUT_OF_SERVICE;
          }
        LeaveCriticalSection( &m_cs );
        }
      }

    DoSetState(activeChanged);
    return hr;
  }

//*******************************************************************
void OPCGroupBase::DoSetUpdateRate( DWORD newUpdateRate )
  {
  m_updateRate = newUpdateRate;
  }

//*******************************************************************
// The state parameters have been set by the client.
// If the active state has changed, activeChanged is true
void OPCGroupBase::DoSetState(BOOL activeChanged)
  {
  if( activeChanged )
    {
    // m_active contains the new active state
    }
  }

//*******************************************************************
STDMETHODIMP OPCGroupBase::SetName(
                                   LPCWSTR szName)
  {
  USES_CONVERSION;
  if( m_removed )   return E_FAIL;
  if( szName == NULL || wcscmp(szName, L"") == 0)  
    return E_INVALIDARG;
  if( m_name == szName )
    return S_OK;

  // check for duplicates
  CString groupName( szName );
  OPCGroupObject* pGroup = m_parent->FindNamedGroup( T2OLE(groupName.GetBuffer(0)) );
  if( pGroup && pGroup!=this )
    return OPC_E_DUPLICATENAME;

  m_name = szName;
  return S_OK;
  }

//*******************************************************************
STDMETHODIMP OPCGroupBase::CloneGroup(
                                      LPCWSTR     szName,
                                      REFIID      riid,
                                      LPUNKNOWN * ppUnk)
  {
  if( m_removed )   return E_FAIL;
  DWORD newServer=0;
  HRESULT hr = m_parent->AddGroup( szName,
    FALSE,
    m_updateRate,
    m_clientHandle,
    &m_timeBias,
    &m_deadBand,
    m_LCID,
    &newServer,
    NULL,
    riid,
    ppUnk);
  if( SUCCEEDED( hr ) )
    {
    OPCGroupBase* pGroup;
    HRESULT hr = (*ppUnk)->QueryInterface( IID_IOPCGroupStateMgt, (LPVOID*)&pGroup );
    if( FAILED(hr) ){;}
    else
      {
      hr = DoCloneGroupItems(pGroup);
      pGroup->Release();
      }
    }
  return hr;
  }

HRESULT OPCGroupBase::DoCloneGroupItems(OPCGroupBase* pGroup)
  {
  HRESULT hr = E_NOTIMPL;
  return hr;
  }

//*******************************************************************
STDMETHODIMP OPCGroupBase::Read(
                                OPCDATASOURCE   dwSource,
                                DWORD           dwNumItems,
                                OPCHANDLE     * phServer,
                                OPCITEMSTATE ** ppItemValues,
                                HRESULT      ** ppErrors)
  {
  if( m_removed ) return E_FAIL;
  HRESULT hr = S_OK;
  // All args should be valid
  if( phServer==NULL || ppItemValues==NULL || ppErrors==NULL || dwNumItems < 1 )
    return E_INVALIDARG;

  // create return data
  *ppErrors = NULL;
  *ppItemValues = (OPCITEMSTATE*)CoTaskMemAlloc(dwNumItems*sizeof(OPCITEMSTATE));
  if( *ppItemValues == NULL )
    return E_OUTOFMEMORY;
  memset( *ppItemValues, 0, dwNumItems*sizeof(OPCITEMSTATE));
  *ppErrors = (HRESULT*)CoTaskMemAlloc(dwNumItems*sizeof(HRESULT));
  if( *ppErrors == NULL )
    {
    CoTaskMemFree( *ppItemValues );
    *ppItemValues = NULL;
    return E_OUTOFMEMORY;
    }
  memset( *ppErrors, 0, dwNumItems*sizeof(HRESULT));

  // verify all server handles
  BOOL anyHandleOK = FALSE;
  BOOL anyRightsOK = FALSE;
  for( DWORD index=0; index<dwNumItems; index++ )
    {
    // server handle is the address of its Item
    COPCItem* pItem = (COPCItem*)phServer[index];
    (*ppErrors)[index] = S_OK;
    if( IsItemValid( pItem ) )
      {
      anyHandleOK = TRUE;
      }
    else
      {
      (*ppErrors)[index] = OPC_E_INVALIDHANDLE;
      hr = S_FALSE;
      }
    if( ((pItem->accessRights & OPC_READABLE) == 0) && (anyHandleOK))//If handle not ok then return OPC_E_INVALIDHANDLE
      {
      (*ppErrors)[index] = OPC_E_BADRIGHTS;
      hr = S_FALSE;
      }
    else
      anyRightsOK = TRUE;
    }

  if( !anyHandleOK || !anyRightsOK )  // return if ALL handles or rights are invalid
    return S_FALSE;

  hr = DoRead(dwSource, dwNumItems, (COPCItem**)phServer, *ppItemValues, *ppErrors);
  if( FAILED(hr) )
    {
    CoTaskMemFree( *ppItemValues );
    *ppItemValues = NULL;
    CoTaskMemFree( *ppErrors );
    *ppErrors = NULL;
    return hr;
    }

  // Convert to client datatypes - Added Ver 1.31
  for( index=0; index<dwNumItems; index++ )
    {
    COPCItem* pItem = (COPCItem*)phServer[index];
    if( (*ppErrors)[index] == S_OK
      && (pItem->quality & OPC_QUALITY_MASK) == OPC_QUALITY_GOOD
      && ((*ppItemValues)[index].vDataValue.vt & VT_ARRAY) != VT_ARRAY )
      {
      (*ppErrors)[index] = VariantChangeType( &((*ppItemValues)[index].vDataValue),
        &((*ppItemValues)[index].vDataValue),
        0, pItem->clientType );
      if( FAILED((*ppErrors)[index]) )
        hr = S_FALSE;
      }
    }
  return hr;
  }

HRESULT OPCGroupBase::DoRead(
                             OPCDATASOURCE   dwSource,
                             DWORD           dwNumItems,
                             COPCItem     ** ppItems,
                             OPCITEMSTATE  * pItemValues,
                             HRESULT       * pErrors)
  {
  HRESULT hr = S_OK;

  // Initialize the arrays
  for( DWORD index=0; index<dwNumItems; index++ )
    {
    pItemValues[index].hClient = ppItems[index]->clientHandle;
    CoFileTimeNow( &pItemValues[index].ftTimeStamp );
    pItemValues[index].wQuality = 0;
    pItemValues[index].wReserved = 0;
    VariantInit( &pItemValues[index].vDataValue );
    }
  return hr;
  }

//*******************************************************************
STDMETHODIMP OPCGroupBase::Write(
                                 DWORD        dwNumItems,
                                 OPCHANDLE  * phServer,
                                 VARIANT    * pItemValues,
                                 HRESULT   ** ppErrors)
  {
  if( m_removed ) return E_FAIL;
  // All args should be valid
  if( phServer==NULL || pItemValues==NULL || ppErrors==NULL || dwNumItems < 1 )
    return E_INVALIDARG;

  // create return data
  *ppErrors = (HRESULT*)CoTaskMemAlloc(dwNumItems*sizeof(HRESULT));
  if( *ppErrors == NULL )
    return E_OUTOFMEMORY;
  memset( *ppErrors, 0, dwNumItems*sizeof(HRESULT));

  HRESULT hr = S_OK;

  // verify all server handles
  BOOL anyHandleOK = FALSE;
  BOOL anyRightsOK = FALSE;
  for( DWORD index=0; index<dwNumItems; index++ )
    {
    // server handle is the address of its Item
    COPCItem* pItem = (COPCItem*)phServer[index];
    (*ppErrors)[index] = S_OK;
    if( IsItemValid( pItem ) )
      {
      anyHandleOK = TRUE;
      }
    else
      {
      (*ppErrors)[index] = OPC_E_INVALIDHANDLE;
      hr = S_FALSE;
      }
    if( ((pItem->accessRights & OPC_WRITEABLE) == 0) && (anyHandleOK) )//If handle not ok then return OPC_E_INVALIDHANDLE
      {
      (*ppErrors)[index] = OPC_E_BADRIGHTS;
      hr = S_FALSE;
      }
    else
      anyRightsOK = TRUE;
    }

  if( !anyHandleOK || !anyRightsOK )  // return if ALL handles or rights are invalid
    return S_FALSE;

  HRESULT hResult = DoWrite(dwNumItems,(COPCItem**)phServer,pItemValues,*ppErrors);
  if( FAILED(hResult) )
    {
    hr = hResult;
    CoTaskMemFree( *ppErrors );
    *ppErrors = NULL;
    }

  return hr;
  }

//*******************************************************************
HRESULT OPCGroupBase::DoWrite(
                              DWORD        dwNumItems,
                              COPCItem  ** ppItems,
                              VARIANT    * pItemValues,
                              HRESULT    * pErrors)
  {
  HRESULT hr = E_NOTIMPL;
  return hr;
  }

//*******************************************************************
// ASynchronous version 1.0
STDMETHODIMP OPCGroupBase::Read(
                                DWORD           dwConnection,
                                OPCDATASOURCE   dwSource,
                                DWORD           dwNumItems,
                                OPCHANDLE     * phServer,
                                DWORD         * pTransactionID,
                                HRESULT      ** ppErrors)
  {
  if( m_removed ) return E_FAIL;
  // All args should be valid
  if( phServer==NULL || pTransactionID==NULL || ppErrors==NULL || dwNumItems < 1)
    return E_INVALIDARG;
  if( dwConnection == OPCSTMFORMATDATA )
    {
    if ( m_DataAdviseSink == NULL )
      return CONNECT_E_NOCONNECTION;
    }
  else if( dwConnection == OPCSTMFORMATDATATIME )
    {
    if ( m_DataTimeAdviseSink == NULL )
      return CONNECT_E_NOCONNECTION;
    }
  else  // unknown format
    return E_INVALIDARG;

    {  // Check the number of pending requests
    CSLock wait( &m_cs );
    if( m_asyncRequests.GetCount() > m_ASYNC_REQUEST_LIMIT )
      return CONNECT_E_ADVISELIMIT;
    }

  HRESULT hr = S_OK;
  // create return data
  *ppErrors = (HRESULT*)CoTaskMemAlloc(dwNumItems*sizeof(HRESULT));
  if( *ppErrors == NULL )
    return E_OUTOFMEMORY;
  memset( *ppErrors, 0, dwNumItems*sizeof(HRESULT));

  // verify all server handles
  for( DWORD index=0; index<dwNumItems; index++ )
    {
    // server handle is the address of its Item
    COPCItem* pItem = (COPCItem*)phServer[index];
    (*ppErrors)[index] = S_OK;
    if( !IsItemValid( pItem ) )
      {
      (*ppErrors)[index] = OPC_E_INVALIDHANDLE;
      hr = S_FALSE;
      }
    }

  if( hr == S_FALSE )  // return if ANY handles are invalid
    return hr;

  // Create a request that will store this information for the thread
  ASyncRequest* request = new ASyncRequest;
  if( request == NULL )
    {
    CoTaskMemFree( *ppErrors );
    *ppErrors = NULL;
    return E_OUTOFMEMORY;
    }
  request->dwConnection = dwConnection;
  request->dwNumItems = dwNumItems;
  request->dwSource = dwSource;
  request->handles = new OPCHANDLE[dwNumItems];
  request->errors = new HRESULT[dwNumItems];
  if( request->handles == NULL || request->errors == NULL )
    {
    CoTaskMemFree( *ppErrors );
    *ppErrors = NULL;
    delete request;
    return E_OUTOFMEMORY;
    }
  request->type = ASyncRequest::READ;
  *pTransactionID = request->dwTransactionID = m_transactionID++;

  for( index=0; index<dwNumItems; index++ )
    {
    request->handles[index] = phServer[index];
    request->errors[index] = (*ppErrors)[index];
    }

    {  // protect access to the list
    CSLock wait( &m_cs );
    m_asyncRequests.AddHead( request );
    SetEvent( m_hAsyncRequest );
    }

  if( m_hASyncThread == NULL )
    {
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = NULL;
    DWORD threadId=0;
    m_hASyncThread = CreateThread( &sa, 0,
      (LPTHREAD_START_ROUTINE)&ASyncThreadStub,
      this,
      0,
      &threadId);
    ASSERT( m_hASyncThread != NULL );
    }
  if( m_hASyncThread == NULL )
    hr = E_FAIL;
  return hr;
  }

//*******************************************************************
// ASynchronous version 1.0
STDMETHODIMP OPCGroupBase::Write(
                                 DWORD       dwConnection,
                                 DWORD       dwNumItems,
                                 OPCHANDLE * phServer,
                                 VARIANT   * pItemValues,
                                 DWORD     * pTransactionID,
                                 HRESULT ** ppErrors)
  {
  if( m_removed ) return E_FAIL;
  // All args should be valid
  if( phServer==NULL || pItemValues==NULL || pTransactionID==NULL || ppErrors==NULL ||
    dwNumItems < 1)
    return E_INVALIDARG;
  if ( m_AsyncAdviseSink == NULL || dwConnection != OPCSTMFORMATWRITECOMPLETE )
    return CONNECT_E_NOCONNECTION;

    {  // Check the number of pending requests
    CSLock wait( &m_cs );
    if( m_asyncRequests.GetCount() > m_ASYNC_REQUEST_LIMIT )
      return CONNECT_E_ADVISELIMIT;
    }

  HRESULT hr = S_OK;
  // create return data
  *ppErrors = (HRESULT*)CoTaskMemAlloc(dwNumItems*sizeof(HRESULT));
  if( *ppErrors == NULL )
    return E_OUTOFMEMORY;
  memset( *ppErrors, 0, dwNumItems*sizeof(HRESULT));

  // verify all server handles
  for( DWORD index=0; index<dwNumItems; index++ )
    {
    // server handle is the address of its Item
    COPCItem* pItem = (COPCItem*)phServer[index];
    (*ppErrors)[index] = S_OK;
    if( !IsItemValid( pItem ) )
      {
      (*ppErrors)[index] = OPC_E_INVALIDHANDLE;
      hr = S_FALSE;
      }
    }

  if( hr == S_FALSE )  // return if ANY handles are invalid
    return hr;

  // Create a request that will store this information for the thread
  ASyncRequest* request = new ASyncRequest;
  if( request == NULL )
    {
    CoTaskMemFree( *ppErrors );
    *ppErrors = NULL;
    return E_OUTOFMEMORY;
    }
  request->dwConnection = dwConnection;
  request->dwNumItems = dwNumItems;
  request->handles = new OPCHANDLE[dwNumItems];
  request->values = new VARIANT[dwNumItems];
  request->errors = new HRESULT[dwNumItems];
  if( request->handles == NULL || request->values == NULL || request->errors == NULL )
    {
    CoTaskMemFree( *ppErrors );
    *ppErrors = NULL;
    delete request;
    return E_OUTOFMEMORY;
    }
  request->type = ASyncRequest::WRITE;
  *pTransactionID = request->dwTransactionID = m_transactionID++;

  for( index=0; index<dwNumItems; index++ )
    {
    request->handles[index] = phServer[index];
    VariantInit( &request->values[index] );
    VariantCopy( &request->values[index], &pItemValues[index] );
    request->errors[index] = (*ppErrors)[index];
    }

    {  // protect access to the list
    CSLock wait( &m_cs );
    m_asyncRequests.AddHead( request );
    SetEvent( m_hAsyncRequest );
    }

  if( m_hASyncThread == NULL )
    {
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = NULL;
    DWORD threadId=0;
    m_hASyncThread = CreateThread( &sa, 0,
      (LPTHREAD_START_ROUTINE)&ASyncThreadStub,
      this,
      0,
      &threadId);
    ASSERT( m_hASyncThread != NULL );
    }
  if( m_hASyncThread == NULL )
    hr = E_FAIL;
  return hr;
  }

//*******************************************************************
// ASynchronous version 1.0
STDMETHODIMP OPCGroupBase::Refresh(
                                   DWORD           dwConnection,
                                   OPCDATASOURCE   dwSource,
                                   DWORD         * pTransactionID)
  {
  if( m_removed ) return E_FAIL;
  if( !m_active ) return E_FAIL;
  // All args should be valid
  if( pTransactionID==NULL )
    return E_POINTER;
  if( dwConnection == OPCSTMFORMATDATA )
    {
    if ( m_DataAdviseSink == NULL )
      return CONNECT_E_NOCONNECTION;
    }
  else if( dwConnection == OPCSTMFORMATDATATIME )
    {
    if ( m_DataTimeAdviseSink == NULL )
      return CONNECT_E_NOCONNECTION;
    }
  else  // unknown format
    return E_INVALIDARG;

    {  // Check the number of pending requests
    CSLock wait( &m_cs );
    if( m_asyncRequests.GetCount() > m_ASYNC_REQUEST_LIMIT )
      return CONNECT_E_ADVISELIMIT;
    }

  HRESULT hr = S_OK;

  // Create a request that will store this information for the thread
  ASyncRequest* request = new ASyncRequest;
  if( request == NULL )
    {
    return E_OUTOFMEMORY;
    }
  request->dwConnection = dwConnection;
  request->dwNumItems = 0;
  request->dwSource = dwSource;
  request->type = ASyncRequest::REFRESH;
  *pTransactionID = request->dwTransactionID = m_transactionID++;

    {  // protect access to the list
    CSLock wait( &m_cs );
    m_asyncRequests.AddHead( request );
    SetEvent( m_hAsyncRequest );
    }

  if( m_hASyncThread == NULL )
    {
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = NULL;
    DWORD threadId=0;
    m_hASyncThread = CreateThread( &sa, 0,
      (LPTHREAD_START_ROUTINE)&ASyncThreadStub,
      this,
      0,
      &threadId);
    ASSERT( m_hASyncThread != NULL );
    }
  if( m_hASyncThread == NULL )
    hr = E_FAIL;
  return hr;
  }

//*******************************************************************
STDMETHODIMP OPCGroupBase::Cancel(
                                  DWORD dwTransactionID)
  {
  CSLock wait( &m_cs );
  POSITION pos = m_asyncRequests.GetHeadPosition();
  while( pos )
    {
    POSITION thispos = pos;
    ASyncRequest* pRequest = m_asyncRequests.GetNext( pos );
    if( pRequest->dwTransactionID == dwTransactionID )
      {
      m_asyncRequests.RemoveAt( thispos );
      delete pRequest;
      return S_OK;
      }
    }

  return E_FAIL;
  }

//*******************************************************************
// ASynchronous version 2
STDMETHODIMP OPCGroupBase::Read(
                                DWORD           dwNumItems,
                                OPCHANDLE     * phServer,
                                DWORD           TransactionID,
                                DWORD         * pCancelID,
                                HRESULT      ** ppErrors)
  {
  if(!(m_vec.begin() < m_vec.end()))
    return  CONNECT_E_NOCONNECTION;

  if( m_removed ) return E_FAIL;
  // All args should be valid
  if( phServer==NULL || pCancelID==NULL || ppErrors==NULL || dwNumItems < 1)
    return E_INVALIDARG;

    {  // Check the number of pending requests
    CSLock wait( &m_cs );
    if( m_asyncRequests.GetCount() > m_ASYNC_REQUEST_LIMIT )
      return CONNECT_E_ADVISELIMIT;
    }

  HRESULT hr = S_OK;
  // create return data
  *ppErrors = (HRESULT*)CoTaskMemAlloc(dwNumItems*sizeof(HRESULT));
  if( *ppErrors == NULL )
    return E_OUTOFMEMORY;
  memset( *ppErrors, 0, dwNumItems*sizeof(HRESULT));

  // verify all server handles
  BOOL anyHandleOK = FALSE;
  BOOL anyRightsOK = FALSE;
  DWORD numUsed = 0;
  for( DWORD index=0; index<dwNumItems; index++ )
    {
    // server handle is the address of its Item
    COPCItem* pItem = (COPCItem*)phServer[index];
    (*ppErrors)[index] = S_OK;
    if( IsItemValid( pItem ) )
      {
      anyHandleOK = TRUE;
      numUsed++;
      }
    else
      {
      (*ppErrors)[index] = OPC_E_INVALIDHANDLE;
      hr = S_FALSE;
      }
    if( ((pItem->accessRights & OPC_READABLE) == 0) && (anyHandleOK))//If handle not ok then return OPC_E_INVALIDHANDLE
      {
      (*ppErrors)[index] = OPC_E_BADRIGHTS;
      hr = S_FALSE;
      }
    else
      anyRightsOK = TRUE;
    }

  if( !anyHandleOK || !anyRightsOK )  // return if ALL handles or rights are invalid
    return S_FALSE;

  // Create a request that will store this information for the thread
  ASyncRequest* request = new ASyncRequest;
  if( request == NULL )
    {
    CoTaskMemFree( *ppErrors );
    *ppErrors = NULL;
    return E_OUTOFMEMORY;
    }
  request->dwConnection = 0;
  request->dwNumItems = numUsed;
  request->dwSource = OPC_DS_DEVICE;
  request->handles = new OPCHANDLE[numUsed];
  request->errors = new HRESULT[numUsed];
  if( request->handles == NULL || request->errors == NULL )
    {
    CoTaskMemFree( *ppErrors );
    *ppErrors = NULL;
    delete request;
    return E_OUTOFMEMORY;
    }
  request->type = ASyncRequest::READCP;
  request->dwTransactionID = TransactionID;
  *pCancelID = request->dwConnection = m_transactionID++;

  DWORD requestIndex = 0;
  for( index=0; index<dwNumItems; index++ )
    {
    if( SUCCEEDED((*ppErrors)[index]) )
      {
      request->handles[requestIndex] = phServer[index];
      request->errors[requestIndex] = (*ppErrors)[index];
      requestIndex++;
      }
    }

    {  // protect access to the list
    CSLock wait( &m_cs );
    m_asyncRequests.AddHead( request );
    SetEvent( m_hAsyncRequest );
    }

  if( m_hASyncThread == NULL )
    {
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = NULL;
    DWORD threadId=0;
    m_hASyncThread = CreateThread( &sa, 0,
      (LPTHREAD_START_ROUTINE)&ASyncThreadStub,
      this,
      0,
      &threadId);
    ASSERT( m_hASyncThread != NULL );
    }
  if( m_hASyncThread == NULL )
    hr = E_FAIL;
  return hr;
  }

//*******************************************************************
// ASynchronous version 2.0
STDMETHODIMP OPCGroupBase::Write(
                                 DWORD       dwNumItems,
                                 OPCHANDLE * phServer,
                                 VARIANT   * pItemValues,
                                 DWORD       TransactionID,
                                 DWORD     * pCancelID,
                                 HRESULT  ** ppErrors)
  {
  //CNM
  // Problems with OPCRelay / Mosref
  //   if(!(m_vec.begin() < m_vec.end()))
  //	   return  CONNECT_E_NOCONNECTION;

  if( m_removed ) return E_FAIL;
  // All args should be valid
  if( phServer==NULL || pItemValues==NULL || pCancelID==NULL || ppErrors==NULL ||
    dwNumItems < 1 )
    return E_INVALIDARG;

    {  // Check the number of pending requests
    CSLock wait( &m_cs );
    if( m_asyncRequests.GetCount() > m_ASYNC_REQUEST_LIMIT )
      return CONNECT_E_ADVISELIMIT;
    }

  HRESULT hr = S_OK;
  // create return data
  *ppErrors = (HRESULT*)CoTaskMemAlloc(dwNumItems*sizeof(HRESULT));
  if( *ppErrors == NULL )
    return E_OUTOFMEMORY;
  memset( *ppErrors, 0, dwNumItems*sizeof(HRESULT));

  // verify all server handles
  BOOL anyHandleOK = FALSE;
  BOOL anyRightsOK = FALSE;
  DWORD numUsed = 0;
  for( DWORD index=0; index<dwNumItems; index++ )
    {
    // server handle is the address of its Item
    COPCItem* pItem = (COPCItem*)phServer[index];
    (*ppErrors)[index] = S_OK;
    if( IsItemValid( pItem ) )
      {
      anyHandleOK = TRUE;
      numUsed++;
      }
    else
      {
      (*ppErrors)[index] = OPC_E_INVALIDHANDLE;
      hr = S_FALSE;
      }
    if( ((pItem->accessRights & OPC_WRITEABLE) == 0) && (anyHandleOK))//If handle not ok then return OPC_E_INVALIDHANDLE
      {
      (*ppErrors)[index] = OPC_E_BADRIGHTS;
      --numUsed;
      hr = S_FALSE;
      }
    else
      anyRightsOK = TRUE;
    }

  if( !anyHandleOK || !anyRightsOK )  // return if ALL handles or rights are invalid
    return S_FALSE;

  // Create a request that will store this information for the thread
  ASyncRequest* request = new ASyncRequest;
  if( request == NULL )
    {
    CoTaskMemFree( *ppErrors );
    *ppErrors = NULL;
    return E_OUTOFMEMORY;
    }
  request->dwConnection = 0;
  request->dwNumItems = numUsed;
  request->handles = new OPCHANDLE[numUsed];
  request->values = new VARIANT[numUsed];
  request->errors = new HRESULT[numUsed];
  if( request->handles == NULL || request->values == NULL || request->errors == NULL )
    {
    CoTaskMemFree( *ppErrors );
    *ppErrors = NULL;
    delete request;
    return E_OUTOFMEMORY;
    }
  request->type = ASyncRequest::WRITECP;
  request->dwTransactionID = TransactionID;
  *pCancelID = request->dwConnection = m_transactionID++;

  DWORD requestIndex = 0;
  for( index=0; index<dwNumItems; index++ )
    {
    if( SUCCEEDED((*ppErrors)[index]) )
      {
      request->handles[requestIndex] = phServer[index];
      VariantInit( &request->values[requestIndex] );
      VariantCopy( &request->values[requestIndex], &pItemValues[index] );
      request->errors[requestIndex] = (*ppErrors)[index];
      requestIndex++;
      }
    }

    {  // protect access to the list
    CSLock wait( &m_cs );
    m_asyncRequests.AddHead( request );
    SetEvent( m_hAsyncRequest );
    }

  if( m_hASyncThread == NULL )
    {
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = NULL;
    DWORD threadId=0;
    m_hASyncThread = CreateThread( &sa, 0,
      (LPTHREAD_START_ROUTINE)&ASyncThreadStub,
      this,
      0,
      &threadId);
    ASSERT( m_hASyncThread != NULL );
    }
  if( m_hASyncThread == NULL )
    hr = E_FAIL;
  return hr;
  }

//*******************************************************************
STDMETHODIMP OPCGroupBase::Refresh2(
                                    OPCDATASOURCE   dwSource,
                                    DWORD           TransactionID,
                                    DWORD         * pCancelID )
  {
  if(!(m_vec.begin() < m_vec.end()))
    return  CONNECT_E_NOCONNECTION;

  if( m_removed ) return E_FAIL;
  if( !m_active ) return E_FAIL;

  BOOL bNoActiveItems = TRUE;
  COPCItem* pItem = NULL;

  CSLock wait( &m_cs );
  ClearRemovedItems();
  POSITION pos = m_itemMap.GetStartPosition();
  while( pos )
    {
    LPVOID key;
    m_itemMap.GetNextAssoc( pos, key, pItem );
    ASSERT(pItem);
    if( pItem->active )
      {
      bNoActiveItems = FALSE;
      break;
      }
    }
  if(bNoActiveItems) return E_FAIL;

  // All args should be valid
  if( pCancelID==NULL )
    return E_POINTER;

  HRESULT hr = S_OK;

  // Create a request that will store this information for the thread
  ASyncRequest* request = new ASyncRequest;
  if( request == NULL )
    {
    return E_OUTOFMEMORY;
    }
  request->dwConnection = 0;
  request->dwNumItems = 0;
  request->dwSource = dwSource;
  request->type = ASyncRequest::REFRESHCP;
  request->dwTransactionID = TransactionID;
  *pCancelID = request->dwConnection = m_transactionID++;

    {  // protect access to the list
    CSLock wait( &m_cs );
    m_asyncRequests.AddHead( request );
    SetEvent( m_hAsyncRequest );
    }

  if( m_hASyncThread == NULL )
    {
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = NULL;
    DWORD threadId=0;
    m_hASyncThread = CreateThread( &sa, 0,
      (LPTHREAD_START_ROUTINE)&ASyncThreadStub,
      this,
      0,
      &threadId);
    ASSERT( m_hASyncThread != NULL );
    }
  if( m_hASyncThread == NULL )
    hr = E_FAIL;
  return hr;
  }

//*******************************************************************
STDMETHODIMP OPCGroupBase::Cancel2( DWORD dwCancelID )
  {
  CSLock wait( &m_cs );
  POSITION pos = m_asyncRequests.GetTailPosition();
  while( pos )
    {
    POSITION thispos = pos;
    ASyncRequest* pRequest = m_asyncRequests.GetPrev( pos );
    if( pRequest->dwConnection == dwCancelID )
      {
      m_asyncRequests.RemoveAt( thispos );
      delete pRequest;
      return S_OK;
      }
    }

  return E_FAIL;
  }

//*******************************************************************
STDMETHODIMP OPCGroupBase::SetEnable( BOOL bEnable )
  {
  if(!(m_vec.begin() < m_vec.end()))
    return  CONNECT_E_NOCONNECTION;

  if( m_removed ) return E_FAIL;

  m_bEnable = bEnable;
  return S_OK;
  }

//*******************************************************************
STDMETHODIMP OPCGroupBase::GetEnable( BOOL *pbEnable )
  {
  if(!(m_vec.begin() < m_vec.end()))
    return  CONNECT_E_NOCONNECTION;

  if( m_removed ) return E_FAIL;
  if( pbEnable == NULL )   return E_POINTER;

  *pbEnable = m_bEnable;
  return S_OK;
  }

//*******************************************************************
STDMETHODIMP OPCGroupBase::AddItems(
                                    DWORD            dwNumItems,
                                    OPCITEMDEF     * pItemArray,
                                    OPCITEMRESULT ** ppAddResults,
                                    HRESULT       ** ppErrors)
  {
  if( m_removed ) return E_FAIL;
  // All args should be valid
  if( pItemArray==NULL || ppAddResults==NULL || ppErrors==NULL || dwNumItems < 1 )
    return E_INVALIDARG;

  // create return data
  *ppErrors = NULL;
  *ppAddResults = (OPCITEMRESULT*)CoTaskMemAlloc(dwNumItems*sizeof(OPCITEMRESULT));
  if( *ppAddResults == NULL )
    return E_OUTOFMEMORY;
  memset( *ppAddResults, 0, dwNumItems*sizeof(OPCITEMRESULT));
  *ppErrors = (HRESULT*)CoTaskMemAlloc(dwNumItems*sizeof(HRESULT));
  if( *ppErrors == NULL )
    {
    CoTaskMemFree( *ppAddResults );
    *ppAddResults = NULL;
    return E_OUTOFMEMORY;
    }
  memset( *ppErrors, 0, dwNumItems*sizeof(HRESULT));

  HRESULT hr = DoAddItems(dwNumItems,pItemArray,*ppAddResults,*ppErrors);
  if( FAILED(hr) )
    {
    CoTaskMemFree( *ppAddResults );
    *ppAddResults = NULL;
    CoTaskMemFree( *ppErrors );
    *ppErrors = NULL;
    }
  return hr;
  }

//*******************************************************************
HRESULT OPCGroupBase::DoAddItems(
                                 DWORD            dwNumItems,
                                 OPCITEMDEF     * pItemArray,
                                 OPCITEMRESULT  * pAddResults,
                                 HRESULT        * pErrors)
  {
  HRESULT hr = E_NOTIMPL;
  return hr;
  }

//*******************************************************************
STDMETHODIMP OPCGroupBase::ValidateItems(
  DWORD             dwNumItems,
  OPCITEMDEF      * pItemArray,
  BOOL              bBlobUpdate,
  OPCITEMRESULT  ** ppValidationResults,
  HRESULT        ** ppErrors)
  {
  if( m_removed ) return E_FAIL;
  // All args should be valid
  if( pItemArray==NULL || ppValidationResults==NULL || ppErrors==NULL || dwNumItems < 1 )
    return E_INVALIDARG;
  if(!dwNumItems)
    return E_INVALIDARG;

  // create return data
  *ppErrors = NULL;
  *ppValidationResults = (OPCITEMRESULT*)CoTaskMemAlloc(dwNumItems*sizeof(OPCITEMRESULT));
  if( *ppValidationResults == NULL )
    return E_OUTOFMEMORY;
  memset( *ppValidationResults, 0, dwNumItems*sizeof(OPCITEMRESULT));
  *ppErrors = (HRESULT*)CoTaskMemAlloc(dwNumItems*sizeof(HRESULT));
  if( *ppErrors == NULL )
    {
    CoTaskMemFree( *ppValidationResults );
    *ppValidationResults = NULL;
    return E_OUTOFMEMORY;
    }
  memset( *ppErrors, 0, dwNumItems*sizeof(HRESULT));

  HRESULT hr = DoValidateItems(dwNumItems,pItemArray,*ppValidationResults,*ppErrors);
  if( FAILED(hr) )
    {
    CoTaskMemFree( *ppValidationResults );
    *ppValidationResults = NULL;
    CoTaskMemFree( *ppErrors );
    *ppErrors = NULL;
    }
  return hr;
  }

//*******************************************************************
HRESULT OPCGroupBase::DoValidateItems(
                                      DWORD             dwNumItems,
                                      OPCITEMDEF      * pItemArray,
                                      OPCITEMRESULT   * pValidationResults,
                                      HRESULT         * pErrors)
  {

  return E_NOTIMPL;
  }

//*******************************************************************
STDMETHODIMP OPCGroupBase::RemoveItems(
                                       DWORD        dwNumItems,
                                       OPCHANDLE  * phServer,
                                       HRESULT   ** ppErrors)
  {
  if( m_removed ) return E_FAIL;
  HRESULT hr = S_OK;
  // All args should be valid
  if( phServer==NULL || ppErrors==NULL || dwNumItems < 1 )
    return E_INVALIDARG;

  // create return data
  *ppErrors = (HRESULT*)CoTaskMemAlloc(dwNumItems*sizeof(HRESULT));
  if( *ppErrors == NULL )
    return E_OUTOFMEMORY;
  memset( *ppErrors, 0, dwNumItems*sizeof(HRESULT));

  CSLock wait( &m_cs);
  // verify all server handles
  for( DWORD index=0; index<dwNumItems; index++ )
    {
    // server handle is the address of its Item
    COPCItem* pItem = (COPCItem*)phServer[index];
    (*ppErrors)[index] = S_OK;
    if( !IsItemValid( pItem ) )
      {
      (*ppErrors)[index] = OPC_E_INVALIDHANDLE;
      hr = S_FALSE;
      }
    }

  hr = DoRemoveItems(dwNumItems,(COPCItem**)phServer,*ppErrors);
  if( FAILED(hr) )
    {
    CoTaskMemFree( *ppErrors );
    *ppErrors = NULL;
    }
  return hr;
  }

//*******************************************************************
HRESULT OPCGroupBase::DoRemoveItems(
                                    DWORD        dwNumItems,
                                    COPCItem  ** ppItems,
                                    HRESULT    * pErrors)
  {
  HRESULT hr = S_OK;
  CSLock wait( &m_cs );
  for( DWORD index=0; index<dwNumItems; index++ )
    {
    if( pErrors[index] == S_OK )
      {
      COPCItem* pItem = ppItems[index];
      // remove from map of all items
      VERIFY(m_itemMap.RemoveKey( (LPVOID)pItem ));
      delete pItem;
      }
    else
      hr = S_FALSE;
    }
  return hr;
  }

//*******************************************************************
STDMETHODIMP OPCGroupBase::SetActiveState(
  DWORD        dwNumItems,
  OPCHANDLE  * phServer,
  BOOL         bActive,
  HRESULT   ** ppErrors)
  {
  if( m_removed ) return E_FAIL;
  // All args should be valid
  if( phServer==NULL || ppErrors==NULL || dwNumItems < 1 )
    return E_INVALIDARG;

  // create return data
  *ppErrors = (HRESULT*)CoTaskMemAlloc(dwNumItems*sizeof(HRESULT));
  if( *ppErrors == NULL )
    return E_OUTOFMEMORY;
  memset( *ppErrors, 0, dwNumItems*sizeof(HRESULT));

  HRESULT hr = S_OK;
  // verify all server handles
  for( DWORD index=0; index<dwNumItems; index++ )
    {
    // server handle is the address of its Item
    COPCItem* pItem = (COPCItem*)phServer[index];
    (*ppErrors)[index] = S_OK;
    if( IsItemValid( pItem ) )
      {
      if( !bActive ) // set item quality bad
        pItem->quality = OPC_QUALITY_BAD | OPC_QUALITY_OUT_OF_SERVICE;
      }
    else
      {
      (*ppErrors)[index] = OPC_E_INVALIDHANDLE;
      hr = S_FALSE;
      }
    }

  hr = DoSetActiveState(dwNumItems,(COPCItem**)phServer,bActive,*ppErrors);
  if( FAILED(hr) )
    {
    CoTaskMemFree( *ppErrors );
    *ppErrors = NULL;
    }
  return hr;
  }

//*******************************************************************
HRESULT OPCGroupBase::DoSetActiveState(
                                       DWORD        dwNumItems,
                                       COPCItem  ** ppItems,
                                       BOOL         bActive,
                                       HRESULT    * pErrors)
  {
  HRESULT hr = S_OK;
  CSLock wait( &m_cs );
  for( DWORD index=0; index<dwNumItems; index++ )
    {
    if( pErrors[index] == S_OK )
      {
      COPCItem* pItem = ppItems[index];
      pItem->active = bActive;
      }
    else
      hr = S_FALSE;
    }
  return hr;
  }

//*******************************************************************
STDMETHODIMP OPCGroupBase::SetClientHandles(
  DWORD        dwNumItems,
  OPCHANDLE  * phServer,
  OPCHANDLE  * phClient,
  HRESULT   ** ppErrors)
  {
  if( m_removed ) return E_FAIL;
  // All args should be valid
  if( phServer==NULL || phClient==NULL || ppErrors==NULL || dwNumItems < 1 )
    return E_INVALIDARG;

  // create return data
  *ppErrors = (HRESULT*)CoTaskMemAlloc(dwNumItems*sizeof(HRESULT));
  if( *ppErrors == NULL )
    return E_OUTOFMEMORY;
  memset( *ppErrors, 0, dwNumItems*sizeof(HRESULT));

  HRESULT hr = S_OK;
  // verify all server handles
  for( DWORD index=0; index<dwNumItems; index++ )
    {
    // server handle is the address of its Item
    COPCItem* pItem = (COPCItem*)phServer[index];
    (*ppErrors)[index] = S_OK;
    if( !IsItemValid( pItem ) )
      {
      (*ppErrors)[index] = OPC_E_INVALIDHANDLE;
      hr = S_FALSE;
      }
    }

  hr = DoSetClientHandles(dwNumItems,(COPCItem**)phServer,phClient,*ppErrors);
  if( FAILED(hr) )
    {
    CoTaskMemFree( *ppErrors );
    *ppErrors = NULL;
    }
  return hr;
  }

//*******************************************************************
HRESULT OPCGroupBase::DoSetClientHandles(
  DWORD        dwNumItems,
  COPCItem  ** ppItems,
  OPCHANDLE  * phClient,
  HRESULT    * pErrors)
  {
  HRESULT hr = S_OK;

  CSLock wait( &m_cs );
  for( DWORD index=0; index<dwNumItems; index++ )
    {
    if( pErrors[index] == S_OK )
      {
      COPCItem* pItem = ppItems[index];
      pItem->clientHandle = phClient[index];
      }
    else
      hr = S_FALSE;
    }

  return hr;
  }

//*******************************************************************
STDMETHODIMP OPCGroupBase::SetDatatypes(
                                        DWORD        dwNumItems,
                                        OPCHANDLE  * phServer,
                                        VARTYPE    * pRequestedDatatypes,
                                        HRESULT   ** ppErrors)
  {
  if( m_removed ) return E_FAIL;
  // All args should be valid
  if( phServer==NULL || pRequestedDatatypes==NULL || ppErrors==NULL || dwNumItems < 1 )
    return E_INVALIDARG;

  // create return data
  *ppErrors = (HRESULT*)CoTaskMemAlloc(dwNumItems*sizeof(HRESULT));
  if( *ppErrors == NULL )
    return E_OUTOFMEMORY;
  memset( *ppErrors, 0, dwNumItems*sizeof(HRESULT));

  HRESULT hr = S_OK;
  // verify all server handles
  for( DWORD index=0; index<dwNumItems; index++ )
    {
    // server handle is the address of its Item
    COPCItem* pItem = (COPCItem*)phServer[index];
    (*ppErrors)[index] = S_OK;
    if( !IsItemValid( pItem ) )
      {
      (*ppErrors)[index] = OPC_E_INVALIDHANDLE;
      hr = S_FALSE;
      }
    }

  hr = DoSetDatatypes(dwNumItems,(COPCItem**)phServer,pRequestedDatatypes,*ppErrors);
  if( FAILED(hr) )
    {
    CoTaskMemFree( *ppErrors );
    *ppErrors = NULL;
    }
  return hr;
  }

//*******************************************************************
HRESULT OPCGroupBase::DoSetDatatypes(
                                     DWORD        dwNumItems,
                                     COPCItem  ** ppItems,
                                     VARTYPE    * pRequestedDatatypes,
                                     HRESULT    * pErrors)
  {
  HRESULT hr = S_OK;

  CSLock wait( &m_cs );
  for( DWORD index=0; index<dwNumItems; index++ )
    {
    if( pErrors[index] == S_OK )
      {
      COPCItem* pItem = ppItems[index];
      pItem->clientType = pRequestedDatatypes[index];
      if( pItem->clientType == VT_EMPTY )    // if none, use Double
        pItem->clientType = VT_R8;
      }
    else
      hr = S_FALSE;
    }

  return hr;
  }

//*******************************************************************
STDMETHODIMP OPCGroupBase::CreateEnumerator(
  REFIID      riid,
  LPUNKNOWN * ppUnk)
  {
  if( m_removed ) return E_FAIL;
  *ppUnk = NULL;
  IUnknown* pUnk = DoCreateEnumerator();
  if( pUnk == NULL )
    return E_OUTOFMEMORY;

  return pUnk->QueryInterface( riid, (LPVOID*)ppUnk );
  }

IUnknown* OPCGroupBase::DoCreateEnumerator()
  {
  return NULL;
  }
//*******************************************************************
// IDataObject
STDMETHODIMP OPCGroupBase::DAdvise(
                                   FORMATETC* pformatetc,
                                   DWORD advf,
                                   IAdviseSink* pAdvSink,
                                   DWORD* pdwConnection)
  {
  if( m_removed ) return E_FAIL;
  if( pformatetc == NULL ) return E_POINTER;
  if( pAdvSink == NULL ) return E_POINTER;
  if( pdwConnection == NULL ) return E_POINTER;
  // enforce OPC limitations on format
  if( pformatetc->dwAspect != DVASPECT_CONTENT )
    return E_FAIL;
  if( pformatetc->ptd != NULL )
    return E_FAIL;
  if( pformatetc->tymed != TYMED_HGLOBAL )
    return E_FAIL;
  if( pformatetc->lindex != -1 )
    return E_FAIL;
  *pdwConnection = 0;

  HRESULT hr = E_FAIL;
  // Allow one advise per format
  if( pformatetc->cfFormat == OPCSTMFORMATDATA )
    {
    if ( m_DataAdviseSink != NULL)
      return CONNECT_E_ADVISELIMIT;
    hr = pAdvSink->QueryInterface( IID_IAdviseSink, (LPVOID*) &m_DataAdviseSink);
    if(FAILED(hr))
      return hr;
    }
  else if( pformatetc->cfFormat == OPCSTMFORMATDATATIME )
    {
    if ( m_DataTimeAdviseSink != NULL)
      return CONNECT_E_ADVISELIMIT;
    hr = pAdvSink->QueryInterface( IID_IAdviseSink, (LPVOID*) &m_DataTimeAdviseSink);
    if(FAILED(hr))
      return hr;
    }
  else if( pformatetc->cfFormat == OPCSTMFORMATWRITECOMPLETE )
    {
    if ( m_AsyncAdviseSink != NULL)
      return CONNECT_E_ADVISELIMIT;
    hr = pAdvSink->QueryInterface( IID_IAdviseSink, (LPVOID*) &m_AsyncAdviseSink);
    if(FAILED(hr))
      return hr;
    }
  else
    return hr;

  *pdwConnection = pformatetc->cfFormat;

  return hr;
  }

//*******************************************************************
STDMETHODIMP OPCGroupBase::DUnadvise( DWORD dwConnection)
  {
  HRESULT hr = OLE_E_NOCONNECTION;
  if( dwConnection == OPCSTMFORMATDATA )
    {
    if( m_DataAdviseSink )
      {
      ASSERT(!m_DataAdviseSink->Release());
      m_DataAdviseSink = NULL;
      hr = S_OK;
      }
    }
  if( dwConnection == OPCSTMFORMATDATATIME )
    {
    if( m_DataTimeAdviseSink )
      {
      ASSERT(!m_DataTimeAdviseSink->Release());
      m_DataTimeAdviseSink = NULL;
      hr = S_OK;
      }
    }
  if( dwConnection == OPCSTMFORMATWRITECOMPLETE )
    {
    if( m_AsyncAdviseSink )
      {
      ASSERT(!m_AsyncAdviseSink->Release());
      m_AsyncAdviseSink = NULL;
      hr = S_OK;
      }
    }
  return hr;
  }

//*******************************************************************
STDMETHODIMP OPCGroupBase::EnumDAdvise( IEnumSTATDATA** ppenumAdvise)
  {
  return E_NOTIMPL;
  }

//*******************************************************************
// validate the format
STDMETHODIMP OPCGroupBase::QueryGetData(FORMATETC* pformatetc)
  {
  if ( pformatetc == NULL)
    return E_POINTER;

  if ( pformatetc->tymed != TYMED_HGLOBAL )
    return DV_E_TYMED;

  if ( pformatetc->cfFormat != OPCSTMFORMATDATA
    &&  pformatetc->cfFormat != OPCSTMFORMATDATATIME)
    return DV_E_FORMATETC;

  return NOERROR;
  }

//*******************************************************************
STDMETHODIMP OPCGroupBase::GetData( FORMATETC* pformatetcIn, STGMEDIUM* pmedium)
  {
  if ( pmedium == NULL || pformatetcIn == NULL)
    return E_INVALIDARG;

  HRESULT hr = QueryGetData( pformatetcIn);
  if ( hr != NOERROR )
    return hr;

  if( pformatetcIn->cfFormat == OPCSTMFORMATDATA )
    hr = CreateDataStream(pmedium, 0);
  else
    hr = CreateDataTimeStream( pmedium, 0 );

  return hr;
  }

//*******************************************************************
STDMETHODIMP OPCGroupBase::UpdateClients()
  {
  HRESULT hr = S_OK;
  if( !m_dataWaiting )
    return hr;

  //*******************************************************************
  // ConnectionPoint advise for OPC 2.0 clients
  if( m_bEnable && m_vec.begin() < m_vec.end() )
    {
    // find number of changed items to update
    EnterCriticalSection( &m_cs );   // protect data
    COPCItem* pItem = NULL;
    DWORD itemCount = 0;
    HRESULT masterError = S_OK;
    HRESULT masterQual = S_OK;
    CPtrArray changedItems;
    changedItems.SetSize( m_itemMap.GetCount() );
    POSITION pos = m_itemMap.GetStartPosition();
    while( pos )
      {
      LPVOID key;
      m_itemMap.GetNextAssoc( pos, key, pItem );
      if( pItem->changed && pItem->active )
        {
        changedItems[itemCount++] = key;
        }
      }
    if( itemCount == 0 )
      {
      LeaveCriticalSection( &m_cs );
      return hr;
      }

    OPCHANDLE*  pHandles    = (OPCHANDLE*)malloc(itemCount*sizeof(OPCHANDLE));
    VARIANT*    pValues     = (VARIANT*) malloc(itemCount*sizeof(VARIANT));
    WORD*       pQualities  = (WORD*)    malloc(itemCount*sizeof(WORD));
    FILETIME*   pTimes      = (FILETIME*)malloc(itemCount*sizeof(FILETIME));
    HRESULT*    pErrors     = (HRESULT*) malloc(itemCount*sizeof(HRESULT));
    if( pHandles && pValues && pQualities && pTimes && pErrors )
      {
      memset( pHandles, 0, itemCount*sizeof(OPCHANDLE) );
      memset( pQualities, 0, itemCount*sizeof(WORD) );
      memset( pTimes, 0, itemCount*sizeof(FILETIME) );
      memset( pErrors, 0, itemCount*sizeof(HRESULT) );

      // populate the arrays
      for( DWORD index=0; index<itemCount; index++ )
        {
        VERIFY( m_itemMap.Lookup( changedItems[index], pItem ) );
        pHandles[index] = pItem->clientHandle;
        pQualities[index] = pItem->quality;
        if( pQualities[index] != OPC_QUALITY_GOOD )
          masterQual = S_FALSE;
        pTimes[index] = pItem->timestamp;
        pErrors[index] = S_OK;
        VariantInit( &pValues[index] );
        if ( (pItem->value.vt & VT_ARRAY) != VT_ARRAY )
          {
          if( (pItem->quality & OPC_QUALITY_MASK) == OPC_QUALITY_GOOD )
            pErrors[index] = VariantChangeType( &pValues[index],
            (LPVARIANT)pItem->value,
            0, pItem->clientType );
          else
            pErrors[index] = VariantCopy( &pValues[index], pItem->value );
          }
        else
          {
          pErrors[index] = VariantCopy( &pValues[index], pItem->value );
          }
        if( pErrors[index] != S_OK )
          masterError = S_FALSE;
        }
      LeaveCriticalSection( &m_cs );   // protect data

      //*******************************************************************
      // advise the connection points
      Lock();
      IUnknown** pp = m_vec.begin();
      while (pp < m_vec.end() && hr == S_OK)
        {
        if (*pp != NULL)
          {
          try
            {
            IOPCDataCallback* pIOPCDataCallback = (IOPCDataCallback*)*pp;
            hr = pIOPCDataCallback->OnDataChange(0, m_clientHandle,
              masterQual, masterError, itemCount,
              pHandles, pValues, pQualities, pTimes, pErrors);
            if (FAILED(hr))
              {
              //_asm int 3;
              }
            }
          catch(...)              
            {
            //_asm int 3;
            }
          }
        pp++;
        }
      Unlock();
      //*******************************************************************
      // release the memory
      for( index=0; index<itemCount; index++ )
        {
        VariantClear( &pValues[index] );
        }
      }
    free(pHandles);
    free(pValues);
    free(pQualities);
    free(pTimes);
    free(pErrors);
    //*******************************************************************
    }

  // IDataObject advise for OPC 1.0 clients
#if !defined(_WIN32_WCE)
  FORMATETC formatetc;
  formatetc.cfFormat = OPCSTMFORMATDATA;
  formatetc.ptd = NULL;
  formatetc.dwAspect = DVASPECT_CONTENT;
  formatetc.lindex = -1;
  formatetc.tymed = TYMED_HGLOBAL;

  STGMEDIUM stm;
  stm.tymed = TYMED_HGLOBAL;
  stm.pUnkForRelease = NULL;

  if ( m_DataAdviseSink )
    {
    formatetc.cfFormat = OPCSTMFORMATDATA;
    hr = GetData(&formatetc, &stm);
    if( FAILED(hr) )
      return hr;
    if ( m_DataAdviseSink )
      m_DataAdviseSink->OnDataChange(&formatetc, &stm);
    ReleaseStgMedium( &stm );
    }
  if ( m_DataTimeAdviseSink )
    {
    formatetc.cfFormat = OPCSTMFORMATDATATIME;
    hr = GetData(&formatetc, &stm);
    if( FAILED(hr) )
      return hr;
    if ( m_DataTimeAdviseSink )
      m_DataTimeAdviseSink->OnDataChange(&formatetc, &stm);
    ReleaseStgMedium( &stm );
    }
#endif

  // Set all items changed flag to false
  COPCItem* pItem = NULL;
  POSITION pos = m_itemMap.GetStartPosition();
  while( pos )
    {
    LPVOID key;
    m_itemMap.GetNextAssoc( pos, key, pItem );
    pItem->changed = FALSE;
    }

  m_dataWaiting = FALSE;
  return hr;
  }

//*******************************************************************
BOOL OPCGroupBase::IsItemValid( const COPCItem* pItem )
  {
  if( pItem == NULL )
    return FALSE;
  COPCItem* pDummy;
  CSLock wait( &m_cs );
  if( !m_itemMap.Lookup( (LPVOID)pItem, pDummy ) )
    return FALSE;
  if( pItem != pDummy )
    return FALSE;
  return TRUE;
  }
//******************************************************************
void OPCGroupBase::ClearRemovedItems()
  {
  return;
  }

//*******************************************************************
//*******************************************************************
// Background data scan thread
//*******************************************************************
DWORD OPCGroupBase::ThreadStub(void* arg)
  {
  return ((OPCGroupBase*)arg)->DataThread();
  }

//*******************************************************************
DWORD OPCGroupBase::DataThread()
  {
  if(FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED)) )
    {
    TRACE(_T("CoInitializeEx failed\n"));
    return 0;
    }

  while( m_running && !m_removed )
    {
    DWORD start = GetTickCount();
    if( m_active )
      {
      EnterCriticalSection( &m_IOcs );
      BOOL changed = DoUpdateGroup();
      LeaveCriticalSection( &m_IOcs );

      if( m_running && !m_removed )
        {
        EnterCriticalSection( &m_cs );
        COPCItem* pItem = NULL;
        LPVOID key = 0;
        POSITION pos = m_itemMap.GetStartPosition();
        while( pos )
          {
          m_itemMap.GetNextAssoc( pos, key, (COPCItem*&)pItem );
          if( pItem->changed )
            {
            changed = TRUE;
            }
          }
        LeaveCriticalSection( &m_cs );
        }

      if( changed && m_running && !m_removed )
        {
        m_dataWaiting = TRUE;

        DoUpdateClients();
        m_parent->UpdateTime();
        }
      }
    if( m_running && !m_removed )
      {
      DWORD end = GetTickCount();
      DWORD elapsed = end - start;
      if( end < start )    // when it wraps (49 days)
        elapsed = 0;
      if( elapsed < m_updateRate )    // limit it
        {
        // wait for the rest of our period
        WaitForSingleObject( m_hTimer, m_updateRate - elapsed );
        }
      else  // if
        {
        while( m_cmdWaiting )
          Sleep( 5 );
        }
      }
    }
  CoUninitialize();
  return 1;
  }

BOOL OPCGroupBase::DoUpdateGroup()
  {
  return FALSE;
  }

//******************************************************************
// DoUpdateClients - These should be overwritten for Inprocess Servers
//                   that will be used with the Automation Interface.
//******************************************************************
void OPCGroupBase::DoUpdateClients()
  {
  UpdateClients();
  }

void OPCGroupBase::DoAsyncUpdate(ASyncRequest* pRequest)
  {
  AsyncUpdate(pRequest);
  }

void OPCGroupBase::DoAsyncUpdate2(ASyncRequest* pRequest)
  {
  AsyncUpdate2(pRequest);
  }

//*******************************************************************
// Async thread
//*******************************************************************
DWORD OPCGroupBase::ASyncThreadStub(void* arg)
  {
  return ((OPCGroupBase*)arg)->ASyncThread();
  }

//*******************************************************************
// This thread executes a single Async I/O request and exits
DWORD OPCGroupBase::ASyncThread()
  {
  if(FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED)) )
    {
    TRACE(_T("CoInitializeEx failed\n"));
    return 0;
    }

  while( m_running )
    {
    // When multiple refresh from device requests are adjacent in the queue,
    // only do I/O for the first one.
    BOOL lastDeviceRefresh = FALSE;
    while( m_running && !m_asyncRequests.IsEmpty() )
      {
      ASyncRequest* pRequest = NULL;
      EnterCriticalSection( &m_cs );
      if (!m_asyncRequests.IsEmpty())
        pRequest = m_asyncRequests.RemoveTail();
      LeaveCriticalSection( &m_cs );

      if( pRequest==NULL )
        continue;

      COPCItem* pItem = NULL;
      if( pRequest->type == ASyncRequest::REFRESH
        || pRequest->type == ASyncRequest::REFRESHCP )
        {
        if( pRequest->dwSource == OPC_DS_DEVICE && !lastDeviceRefresh )
          {
          // Scan tags
          EnterCriticalSection( &m_IOcs );
          DoUpdateGroup();
          LeaveCriticalSection( &m_IOcs );
          lastDeviceRefresh = TRUE;
          }

        // update all items from tags
        LPVOID key = 0;
        CSLock wait( &m_cs );
        POSITION pos = m_itemMap.GetStartPosition();
        while( pos )
          {
          m_itemMap.GetNextAssoc( pos, key, (COPCItem*&)pItem );
          if( pItem->active )
            {
            // mark all values to be sent to clients (not just changed ones)
            pItem->changed = TRUE;
            }
          }
        }
      else
        if( pRequest->type == ASyncRequest::READ
          || pRequest->type == ASyncRequest::READCP )
          {
          // read tags regardless of active state
          HRESULT* pErrors = (HRESULT*)malloc(pRequest->dwNumItems*sizeof(HRESULT));
          OPCITEMSTATE* pItemValues = (OPCITEMSTATE*)malloc(pRequest->dwNumItems*sizeof(OPCITEMSTATE));
          if( pErrors && pItemValues )
            {
            memset( pErrors, 0, pRequest->dwNumItems*sizeof(HRESULT));
            memset( pItemValues, 0, pRequest->dwNumItems*sizeof(OPCITEMSTATE) );
            DoRead(pRequest->dwSource, pRequest->dwNumItems,
              (COPCItem**)pRequest->handles, pItemValues, pErrors);

            CSLock wait( &m_cs );   // protect data
            // now put results into the items
            for( DWORD index=0; index<pRequest->dwNumItems; index++ )
              {
              // server handle is the address of its Item
              COPCItem* pItem = (COPCItem*)pRequest->handles[index];

              // Ver. 2.03 - All values and qualities are sent to the client regardless of active status
              if((m_active && pItem->active) || pRequest->type == ASyncRequest::READCP )
                pItem->quality = pItemValues[index].wQuality;
              else
                pItem->quality = OPC_QUALITY_BAD | OPC_QUALITY_OUT_OF_SERVICE;

              VariantCopy( &pItem->value, &pItemValues[index].vDataValue);
              pItem->changed = TRUE;
              }
            lastDeviceRefresh = FALSE;
            }
          free(pErrors);
          free(pItemValues);
          }

        else
          if( pRequest->type == ASyncRequest::WRITE
            || pRequest->type == ASyncRequest::WRITECP )
            {
            DoWrite(pRequest->dwNumItems,(COPCItem**)pRequest->handles, pRequest->values, pRequest->errors);
            lastDeviceRefresh = FALSE;
            }
          else
            ASSERT( FALSE );

      // Send notification
      if( pRequest->type == ASyncRequest::READ
        || pRequest->type == ASyncRequest::WRITE
        || pRequest->type == ASyncRequest::REFRESH )
        DoAsyncUpdate( pRequest );
      else
        DoAsyncUpdate2( pRequest );

      delete pRequest;
      }
    // Wait to be notified 
    WaitForSingleObject( m_hAsyncRequest, INFINITE );
    }
  CoUninitialize();
  return 1;
  }

//*******************************************************************
// Asynchronous advises are different from the regular ones.
// There is a unique transaction ID sent
// and it only goes to the specified connection rather than
// all advised clients.
STDMETHODIMP OPCGroupBase::AsyncUpdate(ASyncRequest* pRequest)
  {
  HRESULT hr = S_FALSE;

#if !defined(_WIN32_WCE)
  FORMATETC formatetc;
  formatetc.cfFormat = (USHORT)pRequest->dwConnection;
  formatetc.ptd = NULL;
  formatetc.dwAspect = DVASPECT_CONTENT;
  formatetc.lindex = -1;
  formatetc.tymed = TYMED_HGLOBAL;
  // Send notification
  STGMEDIUM   medium;
  if( pRequest->type == ASyncRequest::WRITE )
    {
    OPCGROUPHEADERWRITE header;
    header.dwItemCount = pRequest->dwNumItems;
    header.hClientGroup = m_clientHandle;
    header.dwTransactionID = pRequest->dwTransactionID;
    header.hrStatus = S_OK;
    for( DWORD index=0; index<header.dwItemCount; index++ )
      {
      if( FAILED(pRequest->errors[index]) )
        header.hrStatus = S_FALSE;
      }

    CSharedFile file;
    file.Write( &header, sizeof(header) );

    for( index=0; index<header.dwItemCount; index++ )
      {
      COPCItem* pItem = (COPCItem*)pRequest->handles[index];
      OPCITEMHEADERWRITE itemHeader;
      itemHeader.hClient = pItem->clientHandle;
      itemHeader.dwError = pRequest->errors[index];
      file.Write( &itemHeader, sizeof(itemHeader) );
      }
    medium.tymed = TYMED_HGLOBAL;
    medium.hGlobal = file.Detach();
    medium.pUnkForRelease = NULL;
    // The format is different from the advise format
    if( m_AsyncAdviseSink )
      m_AsyncAdviseSink->OnDataChange(&formatetc, &medium);
    }
  else  // either a read or refresh
    {
    if( formatetc.cfFormat == OPCSTMFORMATDATA )
      {
      hr = CreateDataStream( &medium, pRequest->dwTransactionID );
      if( FAILED(hr) )
        return hr;
      if( m_DataAdviseSink )
        m_DataAdviseSink->OnDataChange(&formatetc, &medium);
      }
    else
      {
      hr = CreateDataTimeStream( &medium, pRequest->dwTransactionID );
      if( FAILED(hr) )
        return hr;
      if( m_DataTimeAdviseSink )
        m_DataTimeAdviseSink->OnDataChange(&formatetc, &medium);
      }
    }

  ReleaseStgMedium( &medium );
#endif

  return hr;
  }

//*******************************************************************
// AsyncUpdate2 handles the IOPCAsyncIO2 calls corresponding to
// Connection Points.
STDMETHODIMP OPCGroupBase::AsyncUpdate2(ASyncRequest* pRequest)
  {
  HRESULT hr = S_OK;

  // Send notification
  if( pRequest->type == ASyncRequest::WRITECP )
    {
    COPCItem* pItem = NULL;
    DWORD itemCount = pRequest->dwNumItems;
    HRESULT masterError = S_OK;

    OPCHANDLE*  pHandles    = (OPCHANDLE*)malloc(itemCount*sizeof(OPCHANDLE));
    HRESULT*    pErrors     = (HRESULT*) malloc(itemCount*sizeof(HRESULT));
    if( pHandles && pErrors )
      {
      memset( pHandles, 0, itemCount*sizeof(OPCHANDLE) );
      memset( pErrors, 0, itemCount*sizeof(HRESULT) );

      // populate the arrays
      for( DWORD index=0; index<itemCount; index++ )
        {
        pItem = (COPCItem*)pRequest->handles[index];
        pHandles[index] = pItem->clientHandle;
        pErrors[index] = pRequest->errors[index];
        if( pErrors[index] != S_OK )
          masterError = S_FALSE;
        }
      //*******************************************************************
      // advise the connection points
      Lock();
      IUnknown** pp = m_vec.begin();
      while (pp < m_vec.end() && hr == S_OK)
        {
        if (*pp != NULL)
          {
          try
            {
            IOPCDataCallback* pIOPCDataCallback = (IOPCDataCallback*)*pp;
            hr = pIOPCDataCallback->OnWriteComplete(pRequest->dwTransactionID, m_clientHandle,
              masterError, itemCount,
              pHandles, pErrors);
            }
          catch(...) {;}
          }
        pp++;
        }
      Unlock();
      }
    free(pHandles);
    free(pErrors);
    //*******************************************************************
    }
  else if( pRequest->type == ASyncRequest::READCP )
    {
    COPCItem* pItem = NULL;
    DWORD itemCount = pRequest->dwNumItems;
    HRESULT masterError = S_OK;
    HRESULT masterQual = S_OK;

    OPCHANDLE*  pHandles    = (OPCHANDLE*)malloc(itemCount*sizeof(OPCHANDLE));
    VARIANT*    pValues     = (VARIANT*) malloc(itemCount*sizeof(VARIANT));
    WORD*       pQualities  = (WORD*)    malloc(itemCount*sizeof(WORD));
    FILETIME*   pTimes      = (FILETIME*)malloc(itemCount*sizeof(FILETIME));
    HRESULT*    pErrors     = (HRESULT*) malloc(itemCount*sizeof(HRESULT));

    if( pHandles && pValues && pQualities && pTimes && pErrors )
      {
      memset( pHandles, 0, itemCount*sizeof(OPCHANDLE) );
      memset( pQualities, 0, itemCount*sizeof(WORD) );
      memset( pTimes, 0, itemCount*sizeof(FILETIME) );
      memset( pErrors, 0, itemCount*sizeof(HRESULT) );

      // populate the arrays
      for( DWORD index=0; index<itemCount; index++ )
        {
        pItem = (COPCItem*)pRequest->handles[index];
        pHandles[index] = pItem->clientHandle;
        pQualities[index] = pItem->quality;
        if( pItem->quality != OPC_QUALITY_GOOD )
          masterQual = S_FALSE;
        pTimes[index] = pItem->timestamp;
        pErrors[index] = S_OK;
        VariantInit( &pValues[index] );
        if ( (pItem->value.vt & VT_ARRAY) != VT_ARRAY )
          {
          if( (pItem->quality & OPC_QUALITY_MASK) == OPC_QUALITY_GOOD )
            pErrors[index] = VariantChangeType( &pValues[index],
            (LPVARIANT)pItem->value,
            0, pItem->clientType );
          else
            pErrors[index] = VariantCopy( &pValues[index], pItem->value );
          }
        else
          {
          pErrors[index] = VariantCopy( &pValues[index], pItem->value );
          }
        if( pErrors[index] != S_OK )
          masterError = S_FALSE;
        }
      //*******************************************************************
      // advise the connection points
      Lock();
      IUnknown** pp = m_vec.begin();
      while (pp < m_vec.end() && hr == S_OK)
        {
        if (*pp != NULL)
          {
          try
            {
            IOPCDataCallback* pIOPCDataCallback = (IOPCDataCallback*)*pp;
            hr = pIOPCDataCallback->OnReadComplete(pRequest->dwTransactionID, m_clientHandle,
              masterQual, masterError, itemCount,
              pHandles, pValues, pQualities, pTimes, pErrors);
            }
          catch(...) {;}
          }
        pp++;
        }
      Unlock();
      //*******************************************************************
      // release the memory
      for( index=0; index<itemCount; index++ )
        {
        VariantClear( &pValues[index] );
        }
      }
    free(pHandles);
    free(pValues);
    free(pQualities);
    free(pTimes);
    free(pErrors);
    //*******************************************************************
    }
  else if( pRequest->type == ASyncRequest::REFRESHCP )
    {
    COPCItem* pItem = NULL;
    DWORD itemCount = 0;
    HRESULT masterError = S_OK;
    HRESULT masterQual = S_OK;
    CPtrArray changedItems;
    changedItems.SetSize( m_itemMap.GetCount() );
    POSITION pos = m_itemMap.GetStartPosition();
    while( pos )
      {
      LPVOID key;
      m_itemMap.GetNextAssoc( pos, key, pItem );
      ASSERT(pItem);
      // Ver. 2.03 - Modified to send only active items
      if(pItem->active)
        changedItems[itemCount++] = key;
      pItem = NULL;
      }
    OPCHANDLE*  pHandles    = (OPCHANDLE*)malloc(itemCount*sizeof(OPCHANDLE));
    VARIANT*    pValues     = (VARIANT*) malloc(itemCount*sizeof(VARIANT));
    WORD*       pQualities  = (WORD*)    malloc(itemCount*sizeof(WORD));
    FILETIME*   pTimes      = (FILETIME*)malloc(itemCount*sizeof(FILETIME));
    HRESULT*    pErrors     = (HRESULT*) malloc(itemCount*sizeof(HRESULT));
    if( pHandles && pValues && pQualities && pTimes && pErrors )
      {
      memset( pHandles, 0, itemCount*sizeof(OPCHANDLE) );
      memset( pQualities, 0, itemCount*sizeof(WORD) );
      memset( pTimes, 0, itemCount*sizeof(FILETIME) );
      memset( pErrors, 0, itemCount*sizeof(HRESULT) );

      // populate the arrays
      for( DWORD index=0; index<itemCount; index++ )
        {
        VERIFY( m_itemMap.Lookup( changedItems[index], pItem ) );
        pHandles[index] = pItem->clientHandle;
        pQualities[index] = pItem->quality;
        if( pItem->quality != OPC_QUALITY_GOOD )
          masterQual = S_FALSE;
        pTimes[index] = pItem->timestamp;
        pErrors[index] = S_OK;
        VariantInit( &pValues[index] );
        if ( (pItem->value.vt & VT_ARRAY) != VT_ARRAY )
          {
          if( (pItem->quality & OPC_QUALITY_MASK) == OPC_QUALITY_GOOD )
            pErrors[index] = VariantChangeType( &pValues[index],
            (LPVARIANT)pItem->value,
            0, pItem->clientType );
          }
        else
          {
          pErrors[index] = VariantCopy( &pValues[index], pItem->value );
          }
        if( pErrors[index] != S_OK )
          masterError = S_FALSE;
        }
      //*******************************************************************
      // advise the connection points
      Lock();
      IUnknown** pp = m_vec.begin();
      while (pp < m_vec.end() && hr == S_OK)
        {
        if (*pp != NULL)
          {
          try
            {
            IOPCDataCallback* pIOPCDataCallback = (IOPCDataCallback*)*pp;
            hr = pIOPCDataCallback->OnDataChange(pRequest->dwTransactionID, m_clientHandle,
              masterQual, masterError, itemCount,
              pHandles, pValues, pQualities, pTimes, pErrors);
            }
          catch(...) {;}
          }
        pp++;
        }
      Unlock();
      //*******************************************************************
      // release the memory
      for( index=0; index<itemCount; index++ )
        {
        VariantClear( &pValues[index] );
        }
      }
    free(pHandles);
    free(pValues);
    free(pQualities);
    free(pTimes);
    free(pErrors);
    //*******************************************************************
    }

  return hr;
  }

//*******************************************************************
STDMETHODIMP OPCGroupBase::CreateDataStream(STGMEDIUM* pmedium, DWORD transactionID)
  {
#if !defined(_WIN32_WCE)
  COPCItem* pItem = NULL;

  OPCGROUPHEADER header;
  header.dwSize = 0;
  header.hClientGroup = m_clientHandle;
  header.dwTransactionID = transactionID;
  header.hrStatus = S_OK;

  // find number of changed items to update
  CSLock wait( &m_cs );   // protect data
  header.dwItemCount = 0;
  CPtrArray changedItems;
  changedItems.SetSize( m_itemMap.GetCount() );
  POSITION pos = m_itemMap.GetStartPosition();
  while( pos )
    {
    LPVOID key;
    m_itemMap.GetNextAssoc( pos, key, pItem );
    if( pItem->changed && pItem->active )
      {
      changedItems[header.dwItemCount++] = key;
      }
    }
  CSharedFile file;
  file.Write( &header, sizeof(header) );

  DWORD dataOffset = sizeof(header) + sizeof(OPCITEMHEADER2)*header.dwItemCount;
  DWORD headerOffset = (DWORD)file.GetPosition();
  for( DWORD index=0; index<header.dwItemCount; index++ )
    {
    VERIFY( m_itemMap.Lookup( changedItems[index], pItem ) );
    // The header
    OPCITEMHEADER2 itemHeader;
    itemHeader.hClient = pItem->clientHandle;
    itemHeader.dwValueOffset = dataOffset;

    itemHeader.wQuality = pItem->quality;
    itemHeader.wReserved = 0;
    file.Seek( headerOffset, CFile::begin );
    file.Write( &itemHeader, sizeof(itemHeader) );
    headerOffset += sizeof(itemHeader);

    // The data
    COleVariant value( pItem->value );
    if ( (value.vt & VT_ARRAY) != VT_ARRAY )
      {
      if( (pItem->quality & OPC_QUALITY_MASK) == OPC_QUALITY_GOOD )
        VariantChangeType( (LPVARIANT)value,
        (LPVARIANT)value,
        0, pItem->clientType );
      }
    file.Seek( dataOffset, CFile::begin );
    file.Write( (LPVARIANT)value, sizeof(VARIANT) );
    dataOffset += sizeof(VARIANT);
    // add external data (BSTR or arrays)
    if( value.vt == VT_BSTR )
      {  // length is string + prepended length + NULL
      ULONG len = SysStringByteLen( value.bstrVal )+sizeof(DWORD)+sizeof(WCHAR);
      // write the DWORD length that preceeds the string, too
      file.Write( ((BYTE*)value.bstrVal - sizeof(DWORD)), len );
      dataOffset += len;
      }
    else if( value.vt & VT_ARRAY )
      {
      file.Write( value.parray, sizeof(SAFEARRAY) );
      dataOffset += sizeof(SAFEARRAY);
      ULONG dataSize = value.parray->rgsabound[0].cElements * value.parray->cbElements;
      file.Write( value.parray->pvData, dataSize );
      dataOffset += dataSize;
      if(  value.vt == (VT_BSTR|VT_ARRAY) )
        {
        LONG lBound=0;
        LONG uBound=0;
        SafeArrayGetLBound(value.parray, 1, &lBound);
        SafeArrayGetUBound(value.parray, 1, &uBound);
        for( LONG index=lBound; index<uBound; index++ )
          {
          BSTR bstr;
          SafeArrayGetElement(value.parray, &index, &bstr);
          ULONG len = SysStringByteLen( bstr )+sizeof(DWORD)+sizeof(WCHAR);
          // write the DWORD length that preceeds the string, too
          file.Write( ((BYTE*)bstr - sizeof(DWORD)), len );
          dataOffset += len;
          }
        }
      }
    }
  // update the header's stream size parameter
  header.dwSize = (DWORD)file.GetPosition();
  file.SeekToBegin();
  file.Write( &header, sizeof(header) );
  pmedium->tymed = TYMED_HGLOBAL;
  pmedium->hGlobal = file.Detach();
  pmedium->pUnkForRelease = NULL;
#endif
  return S_OK;
  }

//*******************************************************************
STDMETHODIMP OPCGroupBase::CreateDataTimeStream(STGMEDIUM* pmedium, DWORD transactionID)
  {
#if !defined(_WIN32_WCE)
  COPCItem* pItem = NULL;

  OPCGROUPHEADER header;
  header.dwSize = 0;
  header.hClientGroup = m_clientHandle;
  header.dwTransactionID = transactionID;
  header.hrStatus = S_OK;

  // find number of changed items to update
  CSLock wait( &m_cs );   // protect data
  header.dwItemCount = 0;
  CPtrArray changedItems;
  changedItems.SetSize( m_itemMap.GetCount() );
  POSITION pos = m_itemMap.GetStartPosition();
  while( pos )
    {
    LPVOID key;
    m_itemMap.GetNextAssoc( pos, key, pItem );
    if( pItem->changed && pItem->active )
      {
      changedItems[header.dwItemCount++] = key;
      }
    }
  CSharedFile file;
  file.Write( &header, sizeof(header) );

  DWORD dataOffset = sizeof(header) + sizeof(OPCITEMHEADER1)*header.dwItemCount;
  DWORD headerOffset = (DWORD)file.GetPosition();
  for( DWORD index=0; index<header.dwItemCount; index++ )
    {
    VERIFY( m_itemMap.Lookup( changedItems[index], pItem ) );
    // The header
    OPCITEMHEADER1 itemHeader;
    itemHeader.hClient = pItem->clientHandle;
    itemHeader.dwValueOffset = dataOffset;

    itemHeader.wQuality = pItem->quality;
    itemHeader.wReserved = 0;
    itemHeader.ftTimeStampItem = pItem->timestamp;
    file.Seek( headerOffset, CFile::begin );
    file.Write( &itemHeader, sizeof(itemHeader) );
    headerOffset += sizeof(itemHeader);

    // The data
    COleVariant value( pItem->value );
    if ( (value.vt & VT_ARRAY) != VT_ARRAY )
      {
      if( (pItem->quality & OPC_QUALITY_MASK) == OPC_QUALITY_GOOD )
        VariantChangeType( (LPVARIANT)value,
        (LPVARIANT)value,
        0, pItem->clientType );
      }
    file.Seek( dataOffset, CFile::begin );
    file.Write( (LPVARIANT)value, sizeof(VARIANT) );
    dataOffset += sizeof(VARIANT);
    // add external data (BSTR or arrays)
    if( value.vt == VT_BSTR )
      {  // length is string + prepended length + NULL
      ULONG len = SysStringByteLen( value.bstrVal )+sizeof(DWORD)+sizeof(WCHAR);
      // write the DWORD length that preceeds the string, too
      file.Write( ((BYTE*)value.bstrVal - sizeof(DWORD)), len );
      dataOffset += len;
      }
    else if( value.vt & VT_ARRAY )
      {
      file.Write( value.parray, sizeof(SAFEARRAY) );
      dataOffset += sizeof(SAFEARRAY);
      ULONG dataSize = value.parray->rgsabound[0].cElements * value.parray->cbElements;
      file.Write( value.parray->pvData, dataSize );
      dataOffset += dataSize;
      if(  value.vt == (VT_BSTR|VT_ARRAY) )
        {
        LONG lBound=0;
        LONG uBound=0;
        SafeArrayGetLBound(value.parray, 1, &lBound);
        SafeArrayGetUBound(value.parray, 1, &uBound);
        for( LONG index=lBound; index<=uBound; index++ )
          {
          BSTR bstr;
          SafeArrayGetElement(value.parray, &index, &bstr);
          ULONG len = SysStringByteLen( bstr )+sizeof(DWORD)+sizeof(WCHAR);
          // write the DWORD length that preceeds the string, too
          file.Write( ((BYTE*)bstr - sizeof(DWORD)), len );
          dataOffset += len;
          }
        }
      }
    }
  header.dwSize = (DWORD)file.GetPosition();
  file.SeekToBegin();
  file.Write( &header, sizeof(header) );
  pmedium->tymed = TYMED_HGLOBAL;
  pmedium->hGlobal = file.Detach();
  pmedium->pUnkForRelease = NULL;
#endif
  return S_OK;
  }

