//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  OPCServer.cpp
//  $Author    :  Jim Hansen
//
//  Subsystem  :  FS OPC Server
//  Version    :  2.04
//
//  Description:  Overrides to the OPC Server.
//
//
//**************************************************************************

#include "stdafx.h"
#include "OPCServer.h"
#include "OPCProps.h"
#include <process.h>
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern FILETIME serverStartTime;

const WORD MAJOR_VERSION = 2;
const WORD MINOR_VERSION = 0;
const WORD BUILD_NUMBER = 3;

//*******************************************************************
OPCServer::OPCServer()
{
   m_pCurrentGroup = &theDoc->devices;
}

//*******************************************************************
OPCServer::~OPCServer()
{
}

//*******************************************************************
// Return the number of properties for this ItemID (if it is valid)
// ppVoid will be passed back in DoQueryAvailableProperties,
// so save the tag pointer in it.
HRESULT OPCServer::DoQueryNumProperties(
                              LPWSTR      szItemID,
                              DWORD     * pdwNumItems,
                              LPVOID    * ppVoid)
{
   CString itemName( szItemID );
   ODevice* pDevice = NULL;
   OModbusTag* pTag = (OModbusTag*)theDoc->FindTag( itemName, VT_NULL, &pDevice );
   if( pTag )
   {
      *ppVoid = (LPVOID)pTag;
      *pdwNumItems = 6;
      return S_OK;
   }
   return OPC_E_UNKNOWNITEMID;
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
   USES_CONVERSION;
   OModbusTag* pTag = (OModbusTag*)pVoid;
   CString description;
   DWORD index=0;

   description = _T("Item Canonical DataType");
   pPropertyIDs[index] = OPC_PROP_CDT;
   pDescriptions[index] = (LPWSTR)CoTaskMemAlloc(2*description.GetLength()+2);
   wcscpy( pDescriptions[index], T2OLE(description.GetBuffer(0)) );
   pvtDataTypes[index] = VT_I2;

   index++;
   if( index==dwNumItems ) return S_OK;
   description = _T("Item Value");
   pPropertyIDs[index] = OPC_PROP_VALUE;
   pDescriptions[index] = (LPWSTR)CoTaskMemAlloc(2*description.GetLength()+2);
   wcscpy( pDescriptions[index], T2OLE(description.GetBuffer(0)) );
   pvtDataTypes[index] = pTag->nativeType;

   index++;
   if( index==dwNumItems ) return S_OK;
   description = _T("Item Quality");
   pPropertyIDs[index] = OPC_PROP_QUALITY;
   pDescriptions[index] = (LPWSTR)CoTaskMemAlloc(2*description.GetLength()+2);
   wcscpy( pDescriptions[index], T2OLE(description.GetBuffer(0)) );
   pvtDataTypes[index] = VT_I2;

   index++;
   if( index==dwNumItems ) return S_OK;
   description = _T("Item Timestamp");
   pPropertyIDs[index] = OPC_PROP_TIME;
   pDescriptions[index] = (LPWSTR)CoTaskMemAlloc(2*description.GetLength()+2);
   wcscpy( pDescriptions[index], T2OLE(description.GetBuffer(0)) );
   pvtDataTypes[index] = VT_DATE;

   index++;
   if( index==dwNumItems ) return S_OK;
   description = _T("Item Access Rights");
   pPropertyIDs[index] = OPC_PROP_RIGHTS;
   pDescriptions[index] = (LPWSTR)CoTaskMemAlloc(2*description.GetLength()+2);
   wcscpy( pDescriptions[index], T2OLE(description.GetBuffer(0)) );
   pvtDataTypes[index] = VT_I4;

   index++;
   if( index==dwNumItems ) return S_OK;
   description = _T("Item Description");
   pPropertyIDs[index] = OPC_PROP_DESC;
   pDescriptions[index] = (LPWSTR)CoTaskMemAlloc(2*description.GetLength()+2);
   wcscpy( pDescriptions[index], T2OLE(description.GetBuffer(0)) );
   pvtDataTypes[index] = VT_BSTR;

   return S_OK;
}

//*******************************************************************
HRESULT OPCServer::DoGetItemProperties(
                              LPWSTR      szItemID,
                              DWORD       dwNumItems,
                              DWORD     * pPropertyIDs,
                              VARIANT   * ppvData,
                              HRESULT   * ppErrors)
{
   CString itemName( szItemID );
   ODevice* pDevice = NULL;
   OModbusTag* pTag = (OModbusTag*)theDoc->FindTag( itemName, VT_NULL, &pDevice );
   if( !pTag )
      return OPC_E_UNKNOWNITEMID;

   USES_CONVERSION;
   DATE date;
   WORD dosDate=0, dosTime=0;
   for( DWORD index=0; index<dwNumItems; index++ )
   {
      ppErrors[index] = S_OK;
      switch(pPropertyIDs[index])
      {
         case OPC_PROP_CDT:
            ppvData[index].vt = VT_I2;
            ppvData[index].iVal = pTag->nativeType;
            break;
         case OPC_PROP_VALUE:
            VariantCopy( &ppvData[index], &pTag->value );
            break;
         case OPC_PROP_QUALITY:
            ppvData[index].vt = VT_I2;
            ppvData[index].iVal = pTag->quality;
            break;
         case OPC_PROP_TIME:
            FILETIME filetimeLocal;
            if( FileTimeToLocalFileTime( &pTag->timestamp, &filetimeLocal) )
            {
               SYSTEMTIME systime;
               if( FileTimeToSystemTime(&filetimeLocal, &systime) )
               {
	               SystemTimeToVariantTime( &systime, &date);
	            }
            }
            ppvData[index].vt = VT_DATE;
            ppvData[index].date = date;
            break;
         case OPC_PROP_RIGHTS:
            ppvData[index].vt = VT_I4;
            ppvData[index].lVal = OPC_READABLE;
            // check if tag is writable (OUTPUT_XXX)
            if(TRUE /* CNM TEMP*/ || pTag->m_IsOutput)
            {
               ppvData[index].lVal |= OPC_WRITEABLE;
            }
            break;
         case OPC_PROP_DESC:
            ppvData[index].vt = VT_BSTR;
            ppvData[index].bstrVal = pTag->description.AllocSysString();
            break;
      }
   }
   return S_OK;
}

//*******************************************************************
HRESULT OPCServer::DoGetStatus( OPCSERVERSTATUS *pServerStatus)
{
   CSLock wait( &m_cs );
   pServerStatus->ftStartTime = serverStartTime;
   CoFileTimeNow( &pServerStatus->ftCurrentTime );
   pServerStatus->ftLastUpdateTime = m_lastUpdateTime;
   pServerStatus->dwServerState = OPC_STATUS_RUNNING;
   pServerStatus->dwGroupCount = m_groupMap.GetCount();
   pServerStatus->dwBandWidth = 0;
   pServerStatus->wMajorVersion = MAJOR_VERSION;
   pServerStatus->wMinorVersion = MINOR_VERSION;
   pServerStatus->wBuildNumber = BUILD_NUMBER;
   CString vendor;
   if( !vendor.LoadString(IDS_VENDOR_INFO) )
   {
      TRACE(_T("Cannot load vendor info string\n"));
   }
   pServerStatus->szVendorInfo = (LPWSTR)CoTaskMemAlloc(2*vendor.GetLength()+2);
   USES_CONVERSION;
   wcscpy( pServerStatus->szVendorInfo, T2OLE(vendor.GetBuffer(0)) );

   return S_OK;
}

//*******************************************************************
OPCNAMESPACETYPE OPCServer::DoQueryOrganization()
{
   return OPC_NS_HIERARCHIAL;
}

//*******************************************************************
HRESULT OPCServer::DoChangeBrowsePosition(
                              OPCBROWSEDIRECTION dwBrowseDirection,
                              LPCWSTR           szString)
{
   USES_CONVERSION;
   if( dwBrowseDirection == OPC_BROWSE_UP )
      if( m_pCurrentGroup->parent )
      {
         m_pCurrentGroup = m_pCurrentGroup->parent;
         return S_OK;
      }
      else
         return E_FAIL;    // cannot go up any more
   else if( dwBrowseDirection == OPC_BROWSE_DOWN )
   {
      CString name( szString );
      POSITION pos = m_pCurrentGroup->groups.GetHeadPosition();
      while( pos )
      {
         OTagGroup* pGroup = m_pCurrentGroup->groups.GetNext( pos );
         if( pGroup->name.CompareNoCase(name)==0 )
         {
            m_pCurrentGroup = pGroup;
            return S_OK;
         }
      }
   }
   else if( dwBrowseDirection == OPC_BROWSE_TO )
   {
      CString name( szString );
      if( name.IsEmpty() )    // the Root
      {
         m_pCurrentGroup = &theDoc->devices;
         return S_OK;
      }
      while( !name.IsEmpty() )
      {
         int delimiter = name.Find( _T('.') );
         if( delimiter > -1 )
         {
            CString branch( name.Left( delimiter ) );
            HRESULT hr = DoChangeBrowsePosition(OPC_BROWSE_DOWN, T2OLE(branch.GetBuffer(0)));
            if( FAILED(hr) )
               return hr;
            name = name.Mid( delimiter+1 );
         }
         else  // no more delimiters
         {
            return DoChangeBrowsePosition(OPC_BROWSE_DOWN, T2OLE(name.GetBuffer(0)));
         }
      }
   }
   return E_INVALIDARG;
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
   CComEnumItemIDs* pEnumString = new CComEnumItemIDs;
   if( pEnumString == NULL )
      return E_OUTOFMEMORY;
   if( dwBrowseFilterType == OPC_FLAT )
       m_pCurrentGroup = &theDoc->devices;
   pEnumString->Initialize( m_pCurrentGroup, dwBrowseFilterType,
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
   BOOL found = FALSE;
   CString name( szItemDataID );

   if(name.IsEmpty() != 0) // null
   {
        *szItemID = (LPWSTR)CoTaskMemAlloc( 2*(name.GetLength()+1) );
        wcscpy( *szItemID, T2OLE(name.GetBuffer(0)) );
        return S_OK;
   }
   
   CString path( GetItemPath(name) );
   if((name.IsEmpty() == 0) && (path.IsEmpty() != 0))
   {
      *szItemID = NULL;
      return E_INVALIDARG;
   }

   *szItemID = (LPWSTR)CoTaskMemAlloc( 2*(path.GetLength()+1) );
   wcscpy( *szItemID, T2OLE(path.GetBuffer(0)) );

   return S_OK;
}

CString OPCServer::GetItemPath(CString path)
{
   // Search for the name
   CString m_name;
   CString tagName = m_pCurrentGroup->FindTagName(path);
   
   // The name is valid
   m_name = tagName;
   //_tcscpy(m_name,tagName.GetBuffer(_MAX_PATH));
     return m_name;
}
HRESULT OPCServer::DoLookupItemIDs(
                              LPWSTR      szItemID,
                              DWORD       dwNumItems,
                              DWORD     * pPropertyIDs,
                              LPWSTR    * pszNewItemIDs,
                              HRESULT   * pErrors)
{
	//Chandra add functionality to get itemID
	USES_CONVERSION;
	CString itemName( szItemID );
	DWORD index = 0;
	CString description;
	OTag* pTag = m_pCurrentGroup->FindTag(itemName);
    if( !pTag )
      return OPC_E_UNKNOWNITEMID;

   // The name is valid
   //_tcscpy( description.GetBuffer(_MAX_PATH),m_pCurrentGroup->GetPathName() + itemName);
   description = m_pCurrentGroup->GetPathName();
   pszNewItemIDs[index] = (LPWSTR)CoTaskMemAlloc(2*description.GetLength()+2);
   wcscpy( pszNewItemIDs[index], T2OLE(description.GetBuffer(0)) );
   //pszNewItemIDs[index] = AllocString(description);
   return S_OK;

}

//*******************************************************************
// CEnumItemIDs implementation
//*******************************************************************
CEnumItemIDs::CEnumItemIDs()
{
   m_pos = NULL;
}

CEnumItemIDs::~CEnumItemIDs()
{
   m_pos = NULL;
}

//*******************************************************************
void CEnumItemIDs::Initialize( OTagGroup*        pGroup,
                              OPCBROWSETYPE     dwFilterType,
                              LPCWSTR           szCriteria,
                              VARTYPE           vtTypeFilter,
                              DWORD             dwRightsFilter)
{
   m_pCurrentGroup = pGroup;

   m_BrowseFilterType = dwFilterType;
   m_FilterCriteria = szCriteria;
   m_DataTypeFilter = vtTypeFilter;
   m_AccessRightsFilter = dwRightsFilter;
   switch( m_BrowseFilterType )
   {
      case OPC_BRANCH:
         m_pos = m_pCurrentGroup->groups.GetHeadPosition();
         break;
      case OPC_LEAF:
         m_pos = m_pCurrentGroup->tags.GetHeadPosition();
         break;
      case OPC_FLAT:
         // Added 2.0
         AddTags( pGroup );
         m_pos = m_paths.GetHeadPosition();
         break;
   }
}

//*******************************************************************
// Recursive function to add tag names from all groups to a list.
// This is only called when browsing OPC_FLAT
// Added 2.0
void  CEnumItemIDs::AddTags( OTagGroup* pGroup )
{
   // First add full path names for this group's tags
   CString path(pGroup->GetPathName());
   POSITION pos = pGroup->tags.GetHeadPosition();
   while( pos )
   {
      OTag* pTag = pGroup->tags.GetNext( pos );
      CString name( path + pTag->name );
      m_paths.AddTail( name );
   }

   // And recurse into the child groups
   pos = pGroup->groups.GetHeadPosition();
   while( pos )
   {
      AddTags( pGroup->groups.GetNext( pos ) );
   }
}
//*******************************************************************
STDMETHODIMP CEnumItemIDs::Next(
                            ULONG celt,
                            LPOLESTR * ppStrings,
                            ULONG * pceltFetched )
{
   for( ULONG i = 0; i < celt && m_pos; )
   {
      CString name;
      switch( m_BrowseFilterType )
      {
         case OPC_BRANCH:
            {
            OTagGroup* pGroup = m_pCurrentGroup->groups.GetNext( m_pos );
            name = pGroup->name;
            }
            break;
         case OPC_LEAF:
            {
            OTag* pTag = m_pCurrentGroup->tags.GetNext( m_pos );
            name = pTag->name;
            }
            break;
         case OPC_FLAT:
            {
            name = m_paths.GetNext( m_pos );
            }
            break;
      }
      if( m_FilterCriteria.IsEmpty() || MatchPattern( name, m_FilterCriteria, FALSE) )
      {
         ppStrings[i] = (LPWSTR)CoTaskMemAlloc(2*name.GetLength()+2);
         USES_CONVERSION;
         wcscpy( ppStrings[i], T2OLE(name.GetBuffer(0)) );
         i++;
      }
   }
   if( pceltFetched )
      *pceltFetched = i;
   return (celt==i) ? S_OK : S_FALSE;
}

//*******************************************************************
// just iterate celt times to skip those items
STDMETHODIMP CEnumItemIDs::Skip( ULONG celt )
{
   for( ULONG i = 0; i < celt && m_pos; )
   {
      CString name;
      switch( m_BrowseFilterType )
      {
         case OPC_BRANCH:
            {
            OTagGroup* pGroup = m_pCurrentGroup->groups.GetNext( m_pos );
            name = pGroup->name;
            }
            break;
         case OPC_LEAF:
            {
            OTag* pTag = m_pCurrentGroup->tags.GetNext( m_pos );
            name = pTag->name;
            }
            break;
         case OPC_FLAT:
            {
            name = m_paths.GetNext( m_pos );
            }
            break;
      }
      if( m_FilterCriteria.IsEmpty() || MatchPattern( name, m_FilterCriteria, FALSE) )
      {
         i++;
      }
   }
   return (celt==i) ? S_OK : S_FALSE;
}

//*******************************************************************
STDMETHODIMP CEnumItemIDs::Reset( void )
{
   switch( m_BrowseFilterType )
   {
      case OPC_BRANCH:
         m_pos = m_pCurrentGroup->groups.GetHeadPosition();
         break;
      case OPC_LEAF:
         m_pos = m_pCurrentGroup->tags.GetHeadPosition();
         break;
      case OPC_FLAT:
         m_pos = m_paths.GetHeadPosition();
         break;
   }
   return S_OK;
}

//*******************************************************************
STDMETHODIMP CEnumItemIDs::Clone( IEnumString ** ppEnumString )
{
   USES_CONVERSION;
   *ppEnumString = NULL;
   CComEnumItemIDs* pEnumString = new CComEnumItemIDs;
   pEnumString->Initialize( m_pCurrentGroup,
                            m_BrowseFilterType,
                            T2OLE(m_FilterCriteria.GetBuffer(0)),
                            m_DataTypeFilter,
                            m_AccessRightsFilter);

   return pEnumString->QueryInterface( IID_IEnumString, (LPVOID*)ppEnumString );
}
