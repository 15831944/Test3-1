//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  OPCServer.cpp
//  $Author    :  Jim Hansen
//
//  Subsystem  :  FS OPC Server DLL
//  Version   :  2.04
//
//  Description:  Overrides to the OPC Server.
//
//
//**************************************************************************

#include "stdafx.h"
#include "OPCServer.h"
#include "OPCProps.h"
// #include<process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern FILETIME serverStartTime;

const WORD MAJOR_VERSION = 2;
const WORD MINOR_VERSION = 0;
const WORD BUILD_NUMBER = 4;

//*******************************************************************
OPCServer::OPCServer()
{
}

//*******************************************************************
OPCServer::~OPCServer()
{
}

//*******************************************************************
OPCGroupObject* OPCServer::DoAddGroup(
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

   RECT rect;
   rect.left=0;
   rect.right=100;
   rect.top=0;
   rect.bottom=100;

   // VB6 - Create a window that will be used to handle messages needed for firing events.
   HWND hwnd = pGroup->Create( NULL, rect, _T("UpdateClientWnd"), WS_POPUP);

   if(!hwnd)
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
// Return the number of properties for this ItemID (if it is valid)
// ppVoid will be passed back in DoQueryAvailableProperties,
// so save the tag pointer in it.
HRESULT OPCServer::DoQueryNumProperties(
                              LPWSTR      szItemID,
                              DWORD     * pdwNumItems,
                              LPVOID    * ppVoid)
{
   if( TRUE )  // if the ItemID is valid...
   {
      *ppVoid = 0;
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
   DWORD index=0;

   CString description;
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
   pvtDataTypes[index] = VT_R8;

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
   // any name is ok
   // A server should search for the matching item here...
   CFSItem* pItem = new CFSItem(szItemID);

	if(!pItem) // check for valid szItemID
		return OPC_E_UNKNOWNITEMID;

   // Initialize the item
   pItem->value.vt = VT_R8;
   pItem->value.dblVal = 0.24;
   pItem->quality = OPC_QUALITY_GOOD;
   CoFileTimeNow( &pItem->timestamp );

   USES_CONVERSION;
   DATE date = NULL;
   WORD dosDate=0, dosTime=0;
   for( DWORD index=0; index<dwNumItems; index++ )
   {
      ppErrors[index] = S_OK;
      switch(pPropertyIDs[index])
      {
         case OPC_PROP_CDT:
            ppvData[index].vt = VT_I2;
            ppvData[index].iVal = pItem->value.vt;  // the canonical (native) data type
            break;
         case OPC_PROP_VALUE:
            VariantCopy( &ppvData[index], &pItem->value );
            break;
         case OPC_PROP_QUALITY:
            ppvData[index].vt = VT_I2;
            ppvData[index].iVal = pItem->quality;
            break;
         case OPC_PROP_TIME:
            FILETIME filetimeLocal;
            if( FileTimeToLocalFileTime( &pItem->timestamp, &filetimeLocal) )
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
            ppvData[index].lVal = OPC_READABLE | OPC_WRITEABLE;
            break;
         case OPC_PROP_DESC:
            ppvData[index].vt = VT_BSTR;
            ppvData[index].bstrVal = SysAllocString(L"Item Description");
            break;
      }
   }
   delete pItem;
   return S_OK;
}

//*******************************************************************
HRESULT OPCServer::DoGetStatus( OPCSERVERSTATUS *pServerStatus)
{
   CSLock wait( &m_cs );
   pServerStatus->ftStartTime = serverStartTime;
   CoFileTimeNow( &pServerStatus->ftCurrentTime );
   pServerStatus->ftLastUpdateTime = m_lastUpdateTime;
   pServerStatus->dwServerState = OPC_STATUS_RUNNING;   // More here
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
   return OPC_NS_FLAT;
}

//*******************************************************************
HRESULT OPCServer::DoChangeBrowsePosition(
                              OPCBROWSEDIRECTION dwBrowseDirection,
                              LPCWSTR           szString)
{
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
   CComEnumItemIDs* pEnumString = new CComEnumItemIDs;
   if( pEnumString == NULL )
      return E_OUTOFMEMORY;
   pEnumString->Initialize( dwBrowseFilterType,
                   szFilterCriteria, vtDataTypeFilter, dwAccessRightsFilter);

   return pEnumString->QueryInterface( IID_IEnumString, (LPVOID*)ppIEnumString );
}

//*******************************************************************
// if szItemDataID is NULL, return the current location
HRESULT OPCServer::DoGetItemID(
                              LPWSTR      szItemDataID,
                              LPWSTR *    szItemID)
{
   CString path( szItemDataID );
   *szItemID = (LPWSTR)CoTaskMemAlloc( 2*(path.GetLength()+1) );
   USES_CONVERSION;
   wcscpy( *szItemID, T2OLE(path.GetBuffer(0)) );

   return S_OK;
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

void CEnumItemIDs::Initialize(OPCBROWSETYPE     dwFilterType,
                              LPCWSTR           szCriteria,
                              VARTYPE           vtTypeFilter,
                              DWORD             dwRightsFilter)
{
   m_BrowseFilterType = dwFilterType;
   m_FilterCriteria = szCriteria;
   m_DataTypeFilter = vtTypeFilter;
   m_AccessRightsFilter = dwRightsFilter;

   // Fill in the strings here or get them in Next()
   m_strings.AddTail( _T("Server accepts any string") );
   Reset();
}

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
   USES_CONVERSION;
   *ppEnumString = NULL;
   CComEnumItemIDs* pEnumString = new CComEnumItemIDs;
   pEnumString->Initialize( m_BrowseFilterType,
                            T2OLE(m_FilterCriteria.GetBuffer(0)),
                            m_DataTypeFilter,
                            m_AccessRightsFilter);

   return pEnumString->QueryInterface( IID_IEnumString, (LPVOID*)ppEnumString );
}
