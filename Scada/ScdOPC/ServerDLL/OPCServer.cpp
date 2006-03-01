//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-2000, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  OPCServer.cpp
//  $Author    :  Jim Hansen
//
//  Subsystem  :  FS OPC Server DLL
//  Version    :  2.04
//
//  Description:  Overrides to the OPC Server.
//
//
//**************************************************************************

#include "stdafx.h"
#include "OPCServer.h"
#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern FILETIME serverStartTime;
extern COPCCallback* pCallback;

const WORD MAJOR_VERSION = 2;
const WORD MINOR_VERSION = 0;
const WORD BUILD_NUMBER = 4;

//*******************************************************************
OPCServer::OPCServer()
{
  // AFX_MANAGE_STATE(AfxGetStaticModuleState())                                               
   m_pBrowser = NULL;
   pCallback->ConnectNotify(true); //kga 
}

//*******************************************************************
OPCServer::~OPCServer()
{
  // AFX_MANAGE_STATE(AfxGetStaticModuleState())                                             
   delete m_pBrowser;
   m_pBrowser = NULL;
   pCallback->ConnectNotify(false); //kga 
}

//*******************************************************************
// Return the number of properties for this ItemID (if it is valid)
// ppVoid will be passed back in DoQueryAvailableProperties,
// so save the tag pointer in it.
// Return OPC_E_UNKNOWNITEMID if the item ID is not valid.
HRESULT OPCServer::DoQueryNumProperties(
                              LPWSTR      szItemID,
                              DWORD     * pdwNumItems,
                              LPVOID    * ppVoid)
{
   CString itemName( szItemID );
   HRESULT hr = pCallback->QueryNumProperties(itemName, pdwNumItems, ppVoid);
   return hr;
}

//*******************************************************************
// Return the properties for this ItemID (if it is valid)
// pVoid contains the tag pointer if DoQueryNumProperties returned
// successfully (If it failed, this function is not called)
HRESULT OPCServer::DoQueryAvailableProperties(
                              LPWSTR      szItemID,
                              DWORD       dwNumItems,
                              LPVOID      pVoid,
                              DWORD     * pPropertyIDs,
                              LPWSTR    * pDescriptions,
                              VARTYPE   * pvtDataTypes)
{
   CString itemName( szItemID );
   HRESULT hr = pCallback->QueryAvailableProperties(itemName, dwNumItems, pVoid,
                                    pPropertyIDs, pDescriptions, pvtDataTypes );

   return hr;
}

//*******************************************************************
HRESULT OPCServer::DoGetItemProperties(
                              LPWSTR      szItemID,
                              DWORD       dwNumItems,
                              DWORD     * pPropertyIDs,
                              VARIANT   * pData,
                              HRESULT   * pErrors)
{
   CString itemName( szItemID );
   HRESULT hr = pCallback->GetItemProperties(itemName, dwNumItems,
                                    pPropertyIDs, pData, pErrors );

   return hr;
}

//*******************************************************************
HRESULT OPCServer::DoLookupItemIDs(
                              LPWSTR      szItemID,
                              DWORD       dwNumItems,
                              DWORD     * pPropertyIDs,
                              LPWSTR    * pszNewItemIDs,
                              HRESULT   * pErrors)
{
   CString itemName( szItemID );
   HRESULT hr = pCallback->LookupItemIDs(itemName, dwNumItems,
                                    pPropertyIDs, pszNewItemIDs, pErrors );

   return hr;
}

//*******************************************************************
HRESULT OPCServer::DoGetErrorString(  HRESULT  dwError,
                                      LCID     dwLocale,
                                      LPWSTR   pString)
{
   USES_CONVERSION;
   LPCTSTR pmsg = pCallback->GetErrorString( dwError, dwLocale );
   wcscpy( pString, T2OLE((LPTSTR)pmsg) );
   return (pString) ? S_OK : E_FAIL;
}

//*******************************************************************
HRESULT OPCServer::DoGetStatus( OPCSERVERSTATUS *pServerStatus)
{
   CSLock wait( &m_cs );
   USES_CONVERSION;
   pServerStatus->ftStartTime = serverStartTime;
   CoFileTimeNow( &pServerStatus->ftCurrentTime );
   pServerStatus->ftLastUpdateTime = m_lastUpdateTime;
   pServerStatus->dwServerState = pCallback->GetServerState();
   pServerStatus->dwGroupCount = m_groupMap.GetCount();
   pServerStatus->dwBandWidth = 0;
   pServerStatus->wMajorVersion = MAJOR_VERSION;
   pServerStatus->wMinorVersion = MINOR_VERSION;
   pServerStatus->wBuildNumber = BUILD_NUMBER;

   LPCTSTR vendor = pCallback->GetVendorString();
   DWORD chars = _tcslen(vendor)+1;
   pServerStatus->szVendorInfo = (LPWSTR)CoTaskMemAlloc(2*chars);
   wcscpy( pServerStatus->szVendorInfo, T2OLE((LPTSTR)vendor) );

   return S_OK;
}

//*******************************************************************
OPCNAMESPACETYPE OPCServer::DoQueryOrganization()
{
   if( !m_pBrowser )
      m_pBrowser = pCallback->CreateBrowser();
   if( m_pBrowser )
      return m_pBrowser->QueryOrganization();
   return OPC_NS_HIERARCHIAL;
}

//*******************************************************************
HRESULT OPCServer::DoChangeBrowsePosition(
                              OPCBROWSEDIRECTION dwBrowseDirection,
                              LPCWSTR           szString)
{
   if( !m_pBrowser )
      m_pBrowser = pCallback->CreateBrowser();
   if( m_pBrowser )
   {
      if( dwBrowseDirection == OPC_BROWSE_UP )
      {
         if( m_pBrowser->MoveUp() )
            return S_OK;
      }
      else if( dwBrowseDirection == OPC_BROWSE_TO )
      {
         // move to root
         while( m_pBrowser->MoveUp())
         {;}

         if(wcscmp(szString, L"") == 0)
         {
            return S_OK;
         }
         else
         {
            CString branch( szString );
            if( m_pBrowser->MoveDown( branch ) )
               return S_OK;
         }
      }
      else
      {
         CString branch( szString );
         if( m_pBrowser->MoveDown( branch ) )
            return S_OK;
      }
   }
   return E_FAIL;
}

//*******************************************************************
// Should just create an enumerator, not the rest...
HRESULT OPCServer::DoBrowseOPCItemIDs(
                              OPCBROWSETYPE     dwBrowseFilterType,
                              LPCWSTR           szFilterCriteria,
                              VARTYPE           vtDataTypeFilter,
                              DWORD             dwAccessRightsFilter,
                              LPENUMSTRING *    ppIEnumString)
{
   if( !m_pBrowser )
      m_pBrowser = pCallback->CreateBrowser();

   CComEnumItemIDs* pEnumString = new CComEnumItemIDs;
   if( pEnumString == NULL )
      return E_OUTOFMEMORY;
   pEnumString->Initialize( m_pBrowser, dwBrowseFilterType,
                   szFilterCriteria, vtDataTypeFilter, dwAccessRightsFilter);

   return pEnumString->QueryInterface( IID_IEnumString, (LPVOID*)ppIEnumString );
}

//*******************************************************************
// if szItemDataID is NULL, return the current location
HRESULT OPCServer::DoGetItemID(
                              LPWSTR      szItemDataID,
                              LPWSTR *    szItemID)
{
   USES_CONVERSION;
   if( !m_pBrowser )
      m_pBrowser = pCallback->CreateBrowser();

   CString path( szItemDataID );
   CString strTemp( szItemDataID );

   if(strTemp.IsEmpty() != 0) // null
   {
        *szItemID = (LPWSTR)CoTaskMemAlloc( 2*(path.GetLength()+1) );
        wcscpy( *szItemID, T2OLE(path.GetBuffer(0)) );
        return S_OK;
   }

   if( m_pBrowser )
      path = m_pBrowser->GetItemID(path);
   if((strTemp.IsEmpty() == 0) && (path.IsEmpty() != 0))
   {
      *szItemID = NULL;
      return E_INVALIDARG;
   }

   *szItemID = (LPWSTR)CoTaskMemAlloc( 2*(path.GetLength()+1) );
   wcscpy( *szItemID, T2OLE(path.GetBuffer(0)) );


   return S_OK;
}

//*******************************************************************
HRESULT OPCServer::DoBrowseAccessPaths(
                              LPCWSTR        szItemID,
                              LPENUMSTRING * ppIEnumString)
{
   if( !m_pBrowser )
      m_pBrowser = pCallback->CreateBrowser();

   CComEnumAccessPaths* pEnumString = new CComEnumAccessPaths;
   if( pEnumString == NULL )
      return E_OUTOFMEMORY;
   HRESULT hr = pEnumString->Initialize(m_pBrowser, szItemID);

   if( hr == S_OK)
	   return pEnumString->QueryInterface( IID_IEnumString, (LPVOID*)ppIEnumString );
   else
	   return hr;
}

//*******************************************************************
// CEnumItemIDs implementation
//*******************************************************************
CEnumItemIDs::CEnumItemIDs()
{
}

CEnumItemIDs::~CEnumItemIDs()
{
}

void CEnumItemIDs::Initialize(COPCBrowser*      pBrowser,
                              OPCBROWSETYPE     dwFilterType,
                              LPCWSTR           szCriteria,
                              VARTYPE           vtTypeFilter,
                              DWORD             dwRightsFilter)
{
   m_dwBrowseFilterType = dwFilterType;
   m_szFilterCriteria = szCriteria;
   m_vtDataTypeFilter = vtTypeFilter;
   m_dwAccessRightsFilter = dwRightsFilter;

   if( pBrowser )    // get names from this callback object
   {
      CString criteria( szCriteria );
      pBrowser->GetNames(dwFilterType,criteria,vtTypeFilter,dwRightsFilter);
      CString itemID( pBrowser->Next() );
      while( !itemID.IsEmpty() )
      {
         m_strings.AddTail( itemID );
         itemID = pBrowser->Next();
      }
   }
   Reset();
}

// Add filtering here...
STDMETHODIMP CEnumItemIDs::Next(
                            ULONG celt,
                            LPOLESTR * ppStrings,
                            ULONG * pceltFetched )
{
   USES_CONVERSION;
   for( ULONG i = 0; i < celt && m_pos; i++ )
   {
      CString name(m_strings.GetNext(m_pos));
      ppStrings[i] = (LPWSTR)CoTaskMemAlloc(2*name.GetLength()+2);
      wcscpy( ppStrings[i], T2OLE(name.GetBuffer(0)) );
   }
   if( pceltFetched )
      *pceltFetched = i;
   return (celt==i) ? S_OK : S_FALSE;
}

// just iterate celt times to skip those items
STDMETHODIMP CEnumItemIDs::Skip( ULONG celt )
{
   for( ULONG i = 0; i < celt && m_pos; i++ )
   {
      m_strings.GetNext(m_pos);
   }
   return (celt==i) ? S_OK : S_FALSE;
}

STDMETHODIMP CEnumItemIDs::Reset( void )
{
   m_pos = m_strings.GetHeadPosition();
   return S_OK;
}

STDMETHODIMP CEnumItemIDs::Clone( IEnumString ** ppEnumString )
{
   *ppEnumString = NULL;
   CComEnumItemIDs* pEnumString = new CComEnumItemIDs;
   pEnumString->Initialize( NULL, m_dwBrowseFilterType,
                   m_szFilterCriteria, m_vtDataTypeFilter, m_dwAccessRightsFilter);

   return pEnumString->QueryInterface( IID_IEnumString, (LPVOID*)ppEnumString );
}


//*******************************************************************
// CEnumAccessPaths Enumerator for AccessPaths
//*******************************************************************
CEnumAccessPaths::CEnumAccessPaths()
{
}

CEnumAccessPaths::~CEnumAccessPaths()
{
}

HRESULT CEnumAccessPaths::Initialize(COPCBrowser* pBrowser,
                     LPCWSTR szItemID)
{
   HRESULT hr;
   if( pBrowser )    // get names from this callback object
   {
      CString itemID( szItemID );
      hr = pBrowser->GetAccessPaths(itemID);
      if( hr == S_OK )
      {
          CString accessPath( pBrowser->NextAccessPath() );
          while( !accessPath.IsEmpty() )
          {
             m_strings.AddTail( accessPath );
             accessPath = pBrowser->NextAccessPath();
          }
      }
	  else if(hr == E_INVALIDARG)
		  return hr;
   }
   Reset();
   return hr;
}

STDMETHODIMP CEnumAccessPaths::Next(
                            ULONG celt,
                            LPOLESTR * ppStrings,
                            ULONG * pceltFetched )
{
   USES_CONVERSION;
   for( ULONG i = 0; i < celt && m_pos; i++ )
   {
      CString name(m_strings.GetNext(m_pos));
      ppStrings[i] = (LPWSTR)CoTaskMemAlloc(2*name.GetLength()+2);
      wcscpy( ppStrings[i], T2OLE(name.GetBuffer(0)) );
   }
   if( pceltFetched )
      *pceltFetched = i;
   return (celt==i) ? S_OK : S_FALSE;
}

// just iterate celt times to skip those items
STDMETHODIMP CEnumAccessPaths::Skip( ULONG celt )
{
   for( ULONG i = 0; i < celt && m_pos; i++ )
   {
      m_strings.GetNext(m_pos);
   }
   return (celt==i) ? S_OK : S_FALSE;
}

STDMETHODIMP CEnumAccessPaths::Reset( void )
{
   m_pos = m_strings.GetHeadPosition();
   return S_OK;
}

STDMETHODIMP CEnumAccessPaths::Clone( IEnumString ** ppEnumString )
{
   *ppEnumString = NULL;
   CComEnumAccessPaths* pEnumString = new CComEnumAccessPaths;
   POSITION pos = m_strings.GetHeadPosition();
   while( pos )
      pEnumString->m_strings.AddTail( m_strings.GetNext(pos) );

   return pEnumString->QueryInterface( IID_IEnumString, (LPVOID*)ppEnumString );
}
