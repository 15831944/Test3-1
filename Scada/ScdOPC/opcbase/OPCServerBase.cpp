//**************************************************************************
//
//        Copyright (c) FactorySoft, INC. 1997, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  OPCServerBase.cpp
//  $Author    :  Jim Hansen
//
//  Subsystem  :  OPC
//  Version    :  2.04
//
//  Description:  Implementation of the COM OPC Server interface.
//                Overrides provide application specific behavior.
//                Also: the OPC pattern matching function for browsing.
//
//**************************************************************************
#include "stdafx.h"
#include "OPCServerBase.h"
#if !defined(_WIN32_WCE)
   #include <process.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern FILETIME serverStartTime;
CRITICAL_SECTION g_cs;  // protect the following 2 lists
CSObject g_csobject(&g_cs);
ServerList servers;
CDWordArray LCIDs;

//*******************************************************************
OPCServerBase::OPCServerBase()
: m_localeID( LOCALE_SYSTEM_DEFAULT )
{
   InitializeCriticalSection( &m_cs );
   m_lastUpdateTime.dwLowDateTime=m_lastUpdateTime.dwHighDateTime=0;
   CSLock wait( &g_cs );
   servers.AddTail( this );
}

//*******************************************************************
// Destructor is only called when there are no more clients using the object.
OPCServerBase::~OPCServerBase()
{
   {
   CSLock wait( &g_cs );
   POSITION pos = servers.Find( this );
   if( pos )
      servers.RemoveAt( pos );
   }

   // Release all groups owned by this server
   OPCGroupObject* pGroup = NULL;
   LPVOID key = 0;
   ULONG left = 0;
   POSITION pos = m_groupMap.GetStartPosition();
   while( pos )
   {
      m_groupMap.GetNextAssoc( pos, key, pGroup );
      ASSERT( pGroup );
      pGroup->Remove();
#if !defined(_WIN32_WCE)
      IUnknown* pUnk=0;
      HRESULT hr = pGroup->QueryInterface( IID_IUnknown, (LPVOID*)&pUnk );
      if( SUCCEEDED(hr) )
      {
         CoDisconnectObject( pUnk, 0 );
         pUnk->Release();
//         pGroup->Release();
      }
//      else
#endif
	    delete pGroup;
   }
   m_groupMap.RemoveAll();
   DeleteCriticalSection( &m_cs );
}

//*******************************************************************
void OPCServerBase::UpdateTime()
{
    CSLock wait( &m_cs );
    CoFileTimeNow( &m_lastUpdateTime );
}

//*******************************************************************
// Server should send shutdown callback to its client
// This function can be called from any thread.
void OPCServerBase::ServerShutdown( LPTSTR reason )
{
   USES_CONVERSION;
   Lock();
   try
   {
       // notify the connection points
       IUnknown** pp = m_vec.begin();
       while (pp < m_vec.end())
       {
          if (*pp != NULL)
          {
             IOPCShutdown* pIOPCShutdown = (IOPCShutdown*)*pp;
             HRESULT hr = pIOPCShutdown->ShutdownRequest(T2OLE(reason));
          }
          pp++;
       }
   }
   catch(...)
   {
   }
   Unlock();
}

//********************************************************************
void OPCServerBase::GetUnkGroupList( OPCENUMSCOPE dwScope, LPUNKNOWN** UnkGroupList, int &GroupCount)
{
   int count = 0;
   *UnkGroupList = NULL;
   LPUNKNOWN* pUnkTemp = NULL;
   GroupCount = 0;

   OPCGroupObject* pGroupObject = NULL;
   LPVOID key = 0;
   POSITION pos = m_groupMap.GetStartPosition();
   while( pos )
   {
      m_groupMap.GetNextAssoc( pos, key, pGroupObject );
      ASSERT( pGroupObject );
      count++;
   }

   if(!count)
      return;

   pGroupObject = NULL;
   switch(dwScope)
   {
      case OPC_ENUM_PRIVATE_CONNECTIONS:
      case OPC_ENUM_PRIVATE:
      case OPC_ENUM_ALL_CONNECTIONS:
      case OPC_ENUM_ALL:
         *UnkGroupList = pUnkTemp = new LPUNKNOWN[count];
         if(pUnkTemp == NULL)
            return;
         pos = NULL;
         pos = m_groupMap.GetStartPosition();
         while( pos )
         {
            m_groupMap.GetNextAssoc( pos, key, pGroupObject );
            ASSERT( pGroupObject );
// Is this next mod for .Net OK ?
_asm int 3;
            pUnkTemp[GroupCount] = (LPUNKNOWN)(CComObjectRootEx<CComGlobalsThreadModel>*)pGroupObject;
            pUnkTemp[GroupCount]->AddRef();  // This is our reference
            GroupCount++;
         }
         break;
      case OPC_ENUM_PUBLIC:
      case OPC_ENUM_PUBLIC_CONNECTIONS:
      default:
         break;
   } 
}

//*******************************************************************
void OPCServerBase::GetStrGroupList( OPCENUMSCOPE dwScope, CStringList &StringList ) 
{
   OPCGroupObject* pGroupObject = NULL;
   LPVOID key = 0;
   POSITION pos = NULL;

   StringList.RemoveAll();

   pGroupObject = NULL;
   switch(dwScope)
   {
      case OPC_ENUM_PRIVATE_CONNECTIONS:
      case OPC_ENUM_PRIVATE:
      case OPC_ENUM_ALL_CONNECTIONS:
      case OPC_ENUM_ALL:
         pos = m_groupMap.GetStartPosition();
         while( pos )
         {
            m_groupMap.GetNextAssoc( pos, key, pGroupObject );
            ASSERT( pGroupObject );
            CString name(((OPCGroup*)pGroupObject)->GetGroupName());
            StringList.AddTail(name);
         }
         break;
      case OPC_ENUM_PUBLIC:
      case OPC_ENUM_PUBLIC_CONNECTIONS:
      default:
         break;
   } 

}

//*******************************************************************
OPCGroupObject* OPCServerBase::FindNamedGroup( LPCWSTR name )
{
   CSLock wait( &m_cs );
   OPCGroupObject* pGroup = NULL;
   LPVOID key = 0;
   POSITION pos = m_groupMap.GetStartPosition();
   while( pos )
   {
      m_groupMap.GetNextAssoc( pos, key, pGroup );
      ASSERT( pGroup );
      if( pGroup->CompareName( name ) )
         return pGroup;
   }

   return NULL;
}

//*******************************************************************
STDMETHODIMP OPCServerBase::AddGroup(
                            LPCWSTR     szName,
                            BOOL        bActive,
                            DWORD       dwRequestedUpdateRate,
                            OPCHANDLE   hClientGroup,
                            LONG      * pTimeBias,
                            FLOAT     * pPercentDeadband,
                            DWORD       dwLCID,
                            OPCHANDLE * phServerGroup,
                            DWORD     * pRevisedUpdateRate,
                            REFIID      riid,
                            LPUNKNOWN * ppUnk)
{
   USES_CONVERSION;
   *ppUnk = NULL;
   CSLock wait( &m_cs );
   OPCGroupObject* pGroup = NULL;
   CString groupName( szName );  // convert wide to native string
   if( groupName.IsEmpty() ) // if no name, create a unique one
   {
      for( int count=1; TRUE; count++ )
      {
         groupName.Format( _T("Group%d"), count);
         OPCGroupObject* pGroup = FindNamedGroup( T2OLE(groupName.GetBuffer(0)) );
         if( pGroup == NULL ) // keep looking until no match
            break;
      }
   }
   else  // check for unique name
   {
      OPCGroupObject* pGroup = FindNamedGroup( T2OLE(groupName.GetBuffer(0)) );
      if( pGroup )
         return OPC_E_DUPLICATENAME;
   }

   pGroup = DoAddGroup( T2OLE(groupName.GetBuffer(0)),
                        bActive,
                        dwRequestedUpdateRate,
                        hClientGroup,
                        pTimeBias,
                        pPercentDeadband,
                        dwLCID,
                        phServerGroup,
                        pRevisedUpdateRate);
   if( pGroup == NULL )
      return E_OUTOFMEMORY;


   HRESULT hr = pGroup->QueryInterface( riid, (LPVOID*)ppUnk );
   if( FAILED(hr) )
   {
      delete pGroup;
      return hr;
   }

   pGroup->AddRef();    // our reference

   if( phServerGroup != NULL )
      *phServerGroup = (OPCHANDLE)pGroup;
   if( pRevisedUpdateRate != NULL )
      *pRevisedUpdateRate = pGroup->GetUpdateRate();
   m_groupMap.SetAt( pGroup, pGroup );

   if( dwRequestedUpdateRate != pGroup->GetUpdateRate() )
      return OPC_S_UNSUPPORTEDRATE;

   return hr;
}

//*******************************************************************
OPCGroupObject* OPCServerBase::DoAddGroup(
                            LPCWSTR     szName,
                            BOOL        bActive,
                            DWORD       dwRequestedUpdateRate,
                            OPCHANDLE   hClientGroup,
                            LONG      * pTimeBias,
                            FLOAT     * pPercentDeadband,
                            DWORD       dwLCID,
                            OPCHANDLE * phServerGroup,
                            DWORD     * pRevisedUpdateRate)
{
   OPCGroupObject* pGroup = new OPCGroupObject;
   if( pGroup == NULL )
      return NULL;

   pGroup->Initialize ( szName,
                        bActive,
                        dwRequestedUpdateRate,
                        hClientGroup,
                        pTimeBias,
                        pPercentDeadband,
                        dwLCID,
                        this);
   return pGroup;
}

//*******************************************************************
STDMETHODIMP OPCServerBase::SetLocaleID(LCID dwLcid)
{
    BOOL bFound = FALSE;

   // search LCIDs for valid lcid
   for(int index=0; index<LCIDs.GetSize(); index++)
   {
       if(LCIDs[index] == dwLcid || LOCALE_SYSTEM_DEFAULT == dwLcid)
       {
           bFound = TRUE;
           break;
       }
   }

   if(bFound != TRUE)
       return E_INVALIDARG;

   CSLock wait( &m_cs );
   m_localeID = dwLcid;
   return S_OK;
}

//*******************************************************************
STDMETHODIMP OPCServerBase::GetLocaleID(LCID * pdwLcid)
{
   if( pdwLcid==NULL )
      return E_POINTER;

   CSLock wait( &m_cs );
   *pdwLcid = m_localeID;
   return S_OK;
}

//*******************************************************************
STDMETHODIMP OPCServerBase::QueryAvailableLocaleIDs(
      DWORD          * pdwCount,
      LCID          ** pdwLcid)
{
   if( pdwCount==NULL || pdwLcid==NULL )
      return E_POINTER;

   *pdwCount = LCIDs.GetSize();
   *pdwLcid = (LCID*)CoTaskMemAlloc(LCIDs.GetSize()*sizeof(LCID));
   for(int index=0; index<LCIDs.GetSize(); index++)
   {
       (*pdwLcid)[index] = LCIDs[index];
   }

   return S_OK;
}

//*******************************************************************
STDMETHODIMP OPCServerBase::GetErrorString(
      HRESULT          dwError,
      LPWSTR         * ppString)
{
   return GetErrorString( dwError, m_localeID, ppString );
}

//*******************************************************************
STDMETHODIMP OPCServerBase::SetClientName(LPCWSTR szName)
{
   if( szName==NULL )
      return E_POINTER;

   CSLock wait( &m_cs );
   m_client = szName;
   return S_OK;
}

//*******************************************************************
STDMETHODIMP OPCServerBase::GetErrorString(
                           HRESULT  dwError,
                           LCID     dwLocale,
                           LPWSTR * ppString)
{
#ifdef USING_AFX_EXTENSION_MODULE
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif

   *ppString = NULL;
   CString message(_T("Unknown error."));
   switch(dwError)
   {
      case OPC_E_INVALIDHANDLE:
         message.LoadString( IDS_OPC_E_INVALIDHANDLE );
         break;
      case OPC_E_BADTYPE:
         message.LoadString( IDS_OPC_E_BADTYPE );
         break;
      case OPC_E_PUBLIC:
         message.LoadString( IDS_OPC_E_PUBLIC );
         break;
      case OPC_E_BADRIGHTS:
         message.LoadString( IDS_OPC_E_BADRIGHTS );
         break;
      case OPC_E_UNKNOWNITEMID:
         message.LoadString( IDS_OPC_E_UNKNOWNITEMID );
         break;
      case OPC_E_INVALIDITEMID:
         message.LoadString( IDS_OPC_E_INVALIDITEMID );
         break;
      case OPC_E_INVALIDFILTER:
         message.LoadString( IDS_OPC_E_INVALIDFILTER );
         break;
      case OPC_E_UNKNOWNPATH:
         message.LoadString( IDS_OPC_E_UNKNOWNPATH );
         break;
      case OPC_E_RANGE:
         message.LoadString( IDS_OPC_E_RANGE );
         break;
      case OPC_E_DUPLICATENAME:
         message.LoadString( IDS_OPC_E_DUPLICATENAME );
         break;
      case OPC_S_UNSUPPORTEDRATE:
         message.LoadString( IDS_OPC_S_UNSUPPORTEDRATE );
         break;
      case OPC_S_CLAMP:
         message.LoadString( IDS_OPC_S_CLAMP );
         break;
      case OPC_S_INUSE:
         message.LoadString( IDS_OPC_S_INUSE );
         break;
      case OPC_E_INVALIDCONFIGFILE:
         message.LoadString( IDS_OPC_E_INVALIDCONFIGFILE );
         break;
      case OPC_E_NOTFOUND:
         message.LoadString( IDS_OPC_E_NOTFOUND );
         break;
      default:
         {
         WCHAR buffer[256];
         HRESULT hr = DoGetErrorString( dwError, dwLocale, buffer );
         if( SUCCEEDED(hr) )
            message = buffer;
         }
         break;
   }
   *ppString = (LPWSTR)CoTaskMemAlloc(2+message.GetLength()*2);
   USES_CONVERSION;
   wcscpy( *ppString, T2OLE(message.GetBuffer(0)) );
   return S_OK;
}

//*******************************************************************
HRESULT OPCServerBase::DoGetErrorString(  HRESULT  dwError,
                                          LCID     dwLocale,
                                          LPWSTR   pString)
{
    return E_FAIL;
}

//*******************************************************************
STDMETHODIMP OPCServerBase::GetGroupByName(
                           LPCWSTR     szName,
                           REFIID      riid,
                           LPUNKNOWN * ppUnk)
{
   *ppUnk = NULL;
   CSLock wait( &m_cs );
   OPCGroupObject* pGroup = FindNamedGroup( szName );
   if( pGroup )
      return pGroup->QueryInterface( riid, (LPVOID*)ppUnk );

   return E_INVALIDARG;
}

//*******************************************************************
STDMETHODIMP OPCServerBase::GetStatus(OPCSERVERSTATUS **ppServerStatus)
{
   if( ppServerStatus == NULL )
      return E_POINTER;
   *ppServerStatus = (OPCSERVERSTATUS*)CoTaskMemAlloc(sizeof(OPCSERVERSTATUS));
   if( *ppServerStatus == NULL )
      return E_OUTOFMEMORY;

   return DoGetStatus( *ppServerStatus );
}

//*******************************************************************
HRESULT OPCServerBase::DoGetStatus( OPCSERVERSTATUS *pServerStatus)
{
#ifdef USING_AFX_EXTENSION_MODULE
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif

   CSLock wait( &m_cs );
   pServerStatus->ftStartTime = serverStartTime;
   CoFileTimeNow( &pServerStatus->ftCurrentTime );
   pServerStatus->ftLastUpdateTime = m_lastUpdateTime;
   pServerStatus->dwServerState = OPC_STATUS_RUNNING;
   pServerStatus->dwGroupCount = m_groupMap.GetCount();
   pServerStatus->dwBandWidth = 0;
   pServerStatus->wMajorVersion = 0;
   pServerStatus->wMinorVersion = 0;
   pServerStatus->wBuildNumber = 0;
   CString vendor(_T(""));
   pServerStatus->szVendorInfo = (LPWSTR)CoTaskMemAlloc(2*vendor.GetLength()+2);
   USES_CONVERSION;
   wcscpy( pServerStatus->szVendorInfo, T2OLE(vendor.GetBuffer(0)) );

   return S_OK;
}

//*******************************************************************
STDMETHODIMP OPCServerBase::RemoveGroup(
                           OPCHANDLE   hServerGroup,
                           BOOL     bForce)
{
   if( hServerGroup == 0 )
      return E_INVALIDARG;

   OPCGroupObject* pGroup = (OPCGroupObject*)hServerGroup;
   _ASSERT( pGroup );

   pGroup->Remove();
   
   {// remove from list
   CSLock wait( &m_cs );
   BOOL ok = m_groupMap.RemoveKey( (LPVOID)pGroup );
   }

   LONG left = pGroup->Release();
   if( bForce && left > 0)
   {
#if !defined(_WIN32_WCE)
      IUnknown* pUnk=0;
      HRESULT hr = pGroup->QueryInterface( IID_IUnknown, (LPVOID*)&pUnk );
      if( SUCCEEDED(hr) )
      {
          CoDisconnectObject( pUnk, 0 );
          pUnk->Release();
      }
#endif
      delete pGroup;
      return S_OK;
   }

   if(left > 0)
       return OPC_S_INUSE;

   return S_OK;
}

//*******************************************************************
STDMETHODIMP OPCServerBase::QueryAvailableProperties(
                           LPWSTR      szItemID,
                           DWORD     * pdwCount,
                           DWORD    ** ppPropertyIDs,
                           LPWSTR   ** ppDescriptions,
                           VARTYPE  ** ppvtDataTypes)
{
   // All args should be valid
   if( szItemID==NULL || pdwCount==NULL || ppPropertyIDs==NULL
    || ppDescriptions==NULL || ppvtDataTypes==NULL )
      return E_POINTER;

   *pdwCount = 0;
   *ppPropertyIDs = NULL;
   *ppDescriptions = NULL;
   *ppvtDataTypes = NULL;

   // Find this Item, and get the number of properties
   DWORD dwNumItems = 0;
   LPVOID pVoid = NULL;
   HRESULT hr = DoQueryNumProperties(szItemID, &dwNumItems, &pVoid);
   if( FAILED(hr) )
      return hr;

   // create return data
   *ppPropertyIDs = (DWORD*)CoTaskMemAlloc(dwNumItems*sizeof(DWORD));
   if( *ppPropertyIDs == NULL )
      return E_OUTOFMEMORY;
   memset( *ppPropertyIDs, 0, dwNumItems*sizeof(DWORD));

   *ppDescriptions = (LPWSTR*)CoTaskMemAlloc(dwNumItems*sizeof(LPWSTR));
   if( *ppDescriptions == NULL )
      {
      CoTaskMemFree( *ppPropertyIDs );
      *ppPropertyIDs = NULL;
      return E_OUTOFMEMORY;
      }
   memset( *ppDescriptions, 0, dwNumItems*sizeof(LPWSTR));

   *ppvtDataTypes = (VARTYPE*)CoTaskMemAlloc(dwNumItems*sizeof(VARTYPE));
   if( *ppvtDataTypes == NULL )
      {
      CoTaskMemFree( *ppPropertyIDs );
      *ppPropertyIDs = NULL;
      CoTaskMemFree( *ppDescriptions );
      *ppDescriptions = NULL;
      return E_OUTOFMEMORY;
      }
   memset( *ppvtDataTypes, 0, dwNumItems*sizeof(VARTYPE));

   *pdwCount = dwNumItems;
   // Now get the properties
   hr = DoQueryAvailableProperties(szItemID, dwNumItems, pVoid,
                        *ppPropertyIDs, *ppDescriptions, *ppvtDataTypes);
   return hr;
}

//*******************************************************************
// Return the number of properties for this ItemID (if it is valid)
// ppVoid will be passed back in DoQueryAvailableProperties,
// so save the tag pointer in it.
HRESULT OPCServerBase::DoQueryNumProperties(
                              LPWSTR      szItemID,
                              DWORD     * pdwNumItems,
                              LPVOID    * ppVoid)
{
   return OPC_E_UNKNOWNITEMID;
}

//*******************************************************************
// Return the properties for this ItemID (if it is valid)
// pVoid contains the tag pointer if DoQueryNumProperties returned
// successfully (If it failed, this function is not called)
HRESULT OPCServerBase::DoQueryAvailableProperties(
                              LPWSTR      szItemID,
                              DWORD       dwNumItems,
                              LPVOID      pVoid,
                              DWORD     * pPropertyIDs,
                              LPWSTR    * pDescriptions,
                              VARTYPE   * pvtDataTypes)
{
   return E_NOTIMPL;
}

//*******************************************************************
STDMETHODIMP OPCServerBase::GetItemProperties(
                           LPWSTR      szItemID,
                           DWORD       dwCount,
                           DWORD     * pdwPropertyIDs,
                           VARIANT  ** ppvData,
                           HRESULT  ** ppErrors)
{
   // All args should be valid
   if( szItemID==NULL || pdwPropertyIDs==NULL
    || ppvData==NULL || ppErrors==NULL )
      return E_POINTER;

   *ppvData = NULL;
   *ppErrors = NULL;

   // create return data
   VARIANT* pV = *ppvData = (VARIANT*)CoTaskMemAlloc(dwCount*sizeof(VARIANT));
   if( *ppvData == NULL )
      return E_OUTOFMEMORY;
   memset( *ppvData, 0, dwCount*sizeof(DWORD));

   *ppErrors = (HRESULT*)CoTaskMemAlloc(dwCount*sizeof(HRESULT));
   if( *ppErrors == NULL )
      {
      CoTaskMemFree( *ppvData );
      *ppvData = NULL;
      return E_OUTOFMEMORY;
      }
   memset( *ppErrors, 0, dwCount*sizeof(HRESULT));

   for( DWORD index=0; index<dwCount; index++ )
      VariantInit(&(pV[index]));

   // Now get the properties
   HRESULT hr = DoGetItemProperties(szItemID, dwCount,
                        pdwPropertyIDs, *ppvData, *ppErrors);
   if(FAILED(hr))
   {
       CoTaskMemFree(*ppvData);
       *ppvData = NULL;
       CoTaskMemFree(*ppErrors);
       *ppErrors = NULL;
   }
   return hr;
}

//*******************************************************************
HRESULT OPCServerBase::DoGetItemProperties(
                              LPWSTR      szItemID,
                              DWORD       dwNumItems,
                              DWORD     * pPropertyIDs,
                              VARIANT   * pData,
                              HRESULT   * pErrors)
{
   return E_NOTIMPL;
}

//*******************************************************************
STDMETHODIMP OPCServerBase::LookupItemIDs(
                           LPWSTR      szItemID,
                           DWORD       dwCount,
                           DWORD     * pdwPropertyIDs,
                           LPWSTR   ** ppszNewItemIDs,
                           HRESULT  ** ppErrors)
{
   // All args should be valid
   if( szItemID==NULL || pdwPropertyIDs==NULL
    || ppszNewItemIDs==NULL || ppErrors==NULL )
      return E_POINTER;

   *ppszNewItemIDs = NULL;
   *ppErrors = NULL;

   // create return data
   *ppszNewItemIDs = (LPWSTR*)CoTaskMemAlloc(dwCount*sizeof(LPWSTR));
   if( *ppszNewItemIDs == NULL )
      return E_OUTOFMEMORY;
   memset( *ppszNewItemIDs, 0, dwCount*sizeof(LPWSTR));

   *ppErrors = (HRESULT*)CoTaskMemAlloc(dwCount*sizeof(HRESULT));
   if( *ppErrors == NULL )
      {
      CoTaskMemFree( *ppszNewItemIDs );
      *ppszNewItemIDs = NULL;
      return E_OUTOFMEMORY;
      }
   memset( *ppErrors, 0, dwCount*sizeof(HRESULT));

   // Now get the properties
   HRESULT hr = DoLookupItemIDs(szItemID, dwCount, pdwPropertyIDs,
                        *ppszNewItemIDs, *ppErrors);
   if(FAILED(hr))
   {
       CoTaskMemFree(*ppszNewItemIDs);
       *ppszNewItemIDs = NULL;
       CoTaskMemFree(*ppErrors);
       *ppErrors = NULL;
   }

   return hr;
}

//*******************************************************************
HRESULT OPCServerBase::DoLookupItemIDs(
                              LPWSTR      szItemID,
                              DWORD       dwNumItems,
                              DWORD     * pPropertyIDs,
                              LPWSTR    * pszNewItemIDs,
                              HRESULT   * pErrors)
{
   return E_NOTIMPL;
}

//*******************************************************************
STDMETHODIMP OPCServerBase::CreateGroupEnumerator(
                           OPCENUMSCOPE dwScope,
                           REFIID      riid,
                           LPUNKNOWN * ppUnk)
{
   if( riid != IID_IEnumUnknown && riid != IID_IEnumString)
      return E_NOINTERFACE;

   return DoCreateGroupEnumerator(dwScope, riid, &(*ppUnk));
}

//*******************************************************************
HRESULT OPCServerBase::DoCreateGroupEnumerator(OPCENUMSCOPE dwScope, REFIID riid, LPUNKNOWN* ppUnk)
{
   int iCount = 0;

   if( riid == IID_IEnumUnknown )
   {
      LPUNKNOWN* ppUnkGroupList = NULL;
      GetUnkGroupList(dwScope, &ppUnkGroupList, iCount);
      if(ppUnkGroupList)
      {
         CComEnumUnkGroups* pEnumerator = NULL;
         pEnumerator = new CComEnumUnkGroups();
         if(pEnumerator)
         {
            pEnumerator->Initialize(ppUnkGroupList, iCount);
            return pEnumerator->QueryInterface( riid, (LPVOID*)ppUnk );
         }
         else
            return E_OUTOFMEMORY;
      }
      else
         return S_FALSE;
   }
   else
   {
      CStringList StringList;
      GetStrGroupList(dwScope, StringList);
      if(StringList.IsEmpty() == 0)
      {
         CComEnumStrGroups* pEnumerator = NULL;
         pEnumerator = new CComEnumStrGroups;
         if(pEnumerator)
         {
            pEnumerator->Initialize(StringList);
            return pEnumerator->QueryInterface( riid, (LPVOID*)ppUnk );
         }
         else
            return E_OUTOFMEMORY;
      }
      else
         return S_FALSE;
   }
}

//*******************************************************************
// IOPCBrowseServerAddressSpace
//*******************************************************************
STDMETHODIMP OPCServerBase::QueryOrganization( OPCNAMESPACETYPE * pNameSpaceType)
{
   if( pNameSpaceType == NULL )
      return E_POINTER;

   *pNameSpaceType = DoQueryOrganization();

   return S_OK;
}

//*******************************************************************
OPCNAMESPACETYPE OPCServerBase::DoQueryOrganization()
{
   return OPC_NS_HIERARCHIAL;
}

//*******************************************************************
STDMETHODIMP OPCServerBase::ChangeBrowsePosition(
                              OPCBROWSEDIRECTION dwBrowseDirection,
                              LPCWSTR           szString)
{
   return DoChangeBrowsePosition(dwBrowseDirection,szString);
}

//*******************************************************************
HRESULT OPCServerBase::DoChangeBrowsePosition(
                              OPCBROWSEDIRECTION dwBrowseDirection,
                              LPCWSTR           szString)
{
   return E_NOTIMPL;
}

//*******************************************************************
STDMETHODIMP OPCServerBase::BrowseOPCItemIDs(
                              OPCBROWSETYPE     dwBrowseFilterType,
                              LPCWSTR           szFilterCriteria,
                              VARTYPE           vtDataTypeFilter,
                              DWORD             dwAccessRightsFilter,
                              LPENUMSTRING *    ppIEnumString)
{
   *ppIEnumString = NULL;
   return DoBrowseOPCItemIDs( dwBrowseFilterType,
                              szFilterCriteria,
                              vtDataTypeFilter,
                              dwAccessRightsFilter,
                              ppIEnumString);
}

//*******************************************************************
// Should just create an enumerator, not the rest...
HRESULT OPCServerBase::DoBrowseOPCItemIDs(
                              OPCBROWSETYPE     dwBrowseFilterType,
                              LPCWSTR           szFilterCriteria,
                              VARTYPE           vtDataTypeFilter,
                              DWORD             dwAccessRightsFilter,
                              LPENUMSTRING *    ppIEnumString)
{
   return E_NOTIMPL;
}

//*******************************************************************
// if szItemDataID is NULL, return the current location
STDMETHODIMP OPCServerBase::GetItemID(
                              LPWSTR      szItemDataID,
                              LPWSTR *    szItemID)
{
   if( szItemID == NULL )
      return E_POINTER;

   return DoGetItemID(szItemDataID,szItemID);
}

HRESULT OPCServerBase::DoGetItemID(
                              LPWSTR      szItemDataID,
                              LPWSTR *    szItemID)
{
   return E_NOTIMPL;
}

//*******************************************************************
STDMETHODIMP OPCServerBase::BrowseAccessPaths(
                              LPCWSTR        szItemID,
                              LPENUMSTRING * ppIEnumString)
{
   return DoBrowseAccessPaths(szItemID,ppIEnumString);
}

//*******************************************************************
HRESULT OPCServerBase::DoBrowseAccessPaths(
                              LPCWSTR        szItemID,
                              LPENUMSTRING * ppIEnumString)
{
   return E_NOTIMPL;
}

//*******************************************************************
// CEnumXXXGroups implementation
//*******************************************************************
CEnumUnkGroups::CEnumUnkGroups()
{
}

CEnumUnkGroups::~CEnumUnkGroups()
{
   if(m_Groups != NULL)
   {
      for( ULONG i=0; i<m_totalCount; i++)
      {
         // Need to release our reference
         m_Groups[i]->Release();
      }

      delete [] m_Groups;
   }
}

void CEnumUnkGroups::Initialize( LPUNKNOWN* ppUnkGroupList, ULONG totalCount )
{
   ASSERT(ppUnkGroupList);
   ASSERT(totalCount > 0);
   m_Groups = ppUnkGroupList;
   m_totalCount = totalCount;
   Reset();
}

STDMETHODIMP CEnumUnkGroups::Next(
                            ULONG celt,
                            IUnknown ** ppUnk,
                            ULONG * pceltFetched )
{
   for( ULONG i = 0; (i < celt) && (m_current < m_totalCount); i++ )
   {
      ppUnk[i] = m_Groups[m_current];

      if(ppUnk[i] != NULL)
         ppUnk[i]->AddRef(); // The client must release these

      m_current++;
   }
   if( pceltFetched )
      *pceltFetched = i;
   return (celt==i) ? S_OK : S_FALSE;
}

// just iterate celt times to skip those items
STDMETHODIMP CEnumUnkGroups::Skip( ULONG celt )
{
   if((m_current + celt) >= m_totalCount )
      return S_FALSE;

   m_current = m_current + celt;
   return S_OK;
}

STDMETHODIMP CEnumUnkGroups::Reset( void )
{
   m_current = 0;
   return S_OK;
}

STDMETHODIMP CEnumUnkGroups::Clone( IEnumUnknown ** ppEnumUnknown )
{
   *ppEnumUnknown = NULL;
   CComEnumUnkGroups* pEnumUnknown = new CComEnumUnkGroups;
   if(pEnumUnknown == NULL)
      return E_OUTOFMEMORY;

   pEnumUnknown->Initialize(m_Groups, m_totalCount);
   pEnumUnknown->m_current = m_current;
   return pEnumUnknown->QueryInterface( IID_IEnumUnknown, (LPVOID*)ppEnumUnknown );
}

//**************************************************************************
CEnumStrGroups::CEnumStrGroups()
{
}

CEnumStrGroups::~CEnumStrGroups()
{
}

void CEnumStrGroups::Initialize( CStringList &StringList )
{
   m_strings.RemoveAll();
   POSITION pos = StringList.GetHeadPosition();
   while(pos)
   {
      m_strings.AddTail(StringList.GetNext(pos));
   }

   Reset();
}

STDMETHODIMP CEnumStrGroups::Next(
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
STDMETHODIMP CEnumStrGroups::Skip( ULONG celt )
{
   for( ULONG i = 0; i < celt && m_pos; i++ )
   {
      m_strings.GetNext(m_pos);
   }
   return (celt==i) ? S_OK : S_FALSE;
}

STDMETHODIMP CEnumStrGroups::Reset( void )
{
   m_pos = m_strings.GetHeadPosition();
   return S_OK;
}

STDMETHODIMP CEnumStrGroups::Clone( IEnumString ** ppEnumString )
{
   USES_CONVERSION;
   *ppEnumString = NULL;
   CComEnumStrGroups* pEnumString = new CComEnumStrGroups;
   pEnumString->Initialize( m_strings );

   return pEnumString->QueryInterface( IID_IEnumString, (LPVOID*)ppEnumString );
}

//*******************************************************************
int ConvertCase( int c, BOOL bCaseSensitive )
{
#ifdef _UNICODE 
   return bCaseSensitive ? c : towupper(c);
#else
   return bCaseSensitive ? c : toupper(c);
#endif
}


//*************************************************************************
// return TRUE if String Matches Pattern --
// -- uses Visual Basic LIKE operator syntax
// CAUTION: Function is recursive
//*************************************************************************
BOOL MatchPattern( LPCTSTR String, LPCTSTR Pattern, BOOL bCaseSensitive )
{
    TCHAR   c, p, l;
    for (; ;)
    {
        switch (p = ConvertCase( *Pattern++, bCaseSensitive ) )
        {
        case 0:                             // end of pattern
            return *String ? FALSE : TRUE;  // if end of string TRUE

        case _T('*'):
            while (*String)
            {               // match zero or more char
                if (MatchPattern (String++, Pattern, bCaseSensitive))
                    return TRUE;
            }
            return MatchPattern (String, Pattern, bCaseSensitive );

        case _T('?'):
            if (*String++ == 0)             // match any one char
                return FALSE;               // not end of string
            break;

        case _T('['):
            // match char set
            if ( (c = ConvertCase( *String++, bCaseSensitive) ) == 0)
                return FALSE;                // syntax
            l = 0;
            if( *Pattern == _T('!') )  // match a char if NOT in set []
            {
                ++Pattern;

                while( (p = ConvertCase( *Pattern++, bCaseSensitive) )
                         != _T('\0') )
                {
                    if (p == _T(']'))     // if end of char set, then
                        break;            // no match found

                    if (p == _T('-'))
                    {   // check a range of chars?
                        p = ConvertCase( *Pattern, bCaseSensitive );
                        // get high limit of range
                        if (p == 0  ||  p == _T(']'))
                            return FALSE;     // syntax

                        if (c >= l  &&  c <= p)
                            return FALSE;     // if in range, return FALSE
                    }
                    l = p;
                    if (c == p)               // if char matches this element
                        return FALSE;         // return false
                }
            }
            else    // match if char is in set []
            {
                while( (p = ConvertCase( *Pattern++, bCaseSensitive) )
                         != _T('\0') )
                {
                    if (p == _T(']'))         // if end of char set, then
                        return FALSE;         // no match found

                    if (p == _T('-'))
                    {   // check a range of chars?
                        p = ConvertCase( *Pattern, bCaseSensitive );
                        // get high limit of range
                        if (p == 0  ||  p == _T(']'))
                            return FALSE;       // syntax

                        if (c >= l  &&  c <= p)
                            break;              // if in range, move on
                    }
                    l = p;
                    if (c == p)                 // if char matches this element
                        break;                  // move on
                }

                while (p  &&  p != _T(']'))     // got a match in char set
                    p = *Pattern++;             // skip to end of set
            }

            break;

        case _T('#'):
            c = *String++;
            if( !_istdigit( c ) )
                return FALSE;        // not a digit

            break;

        default:
            c = ConvertCase( *String++, bCaseSensitive );
            if( c != p )            // check for exact char
                return FALSE;                   // not a match

            break;
        }
    }
}
