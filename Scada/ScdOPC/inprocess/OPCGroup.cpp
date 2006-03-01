//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  OPCGroup.cpp
//  $Author    :  Jim Hansen
//
//  Subsystem  :  FS OPC Server DLL
//  Version    :  2.04
//
//  Description:  The OPC Group object implementation.
//
//
//**************************************************************************

#include "stdafx.h"
#include "OPCServer.h"
#include <math.h>
const double pi = 3.1415926535;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//*******************************************************************
//    class COPCItem
//*******************************************************************
CFSItem::CFSItem(LPWSTR itemID)
: name(itemID), m_tick(0), m_type(SINE)
{
    for( int i=0; i<name.GetLength(); i++ )
        m_tick += (int)name[i];

    name.MakeLower();
    if( name.Find( _T("ramp") )>-1 )
    {
        m_type = RAMP;
        value.vt = VT_I4;
        value.lVal = m_tick;
    }
    else if( name.Find( _T("wave") )>-1 )
    {
        m_type = WAVE;
    }
    else if( name.Find( _T("random") )>-1 )
    {
        m_type = RANDOM;
    }
    else if( name.Find( _T("dig5") )>-1 )
    {
        m_type = DIG5;
    }
    else if( name.Find( _T("dig") )>-1 )
    {
        m_type = DIG1;
    }
    else if( name.Find( _T("toggle") )>-1 )
    {
        m_type = TOGGLE;
    }
}

CFSItem::~CFSItem()
{
}

BOOL CFSItem::Simulate()
{
  if( !active )
     return FALSE;   // skip the change detection code
  ULONG now = GetTickCount();
  double t = now/3130.0;
  t += (double)m_tick;

  switch( m_type )
  {
  case RAMP:
      value.vt = VT_I4;
      value.lVal = (value.lVal+7)%100;
      break;
  case SINE:
      value.vt = VT_R8;
      value.dblVal = sin(pi*t/10);
      break;
  case WAVE:
      value.vt = VT_I4;
      value.lVal = (long)(50*sin(pi*t/10)+50);
      break;
  case RANDOM:
      value.vt = VT_I4;
      value.lVal = rand()%100;
      break;
  case TOGGLE:
      value.vt = VT_BOOL;
      value.boolVal = value.boolVal ? VARIANT_FALSE : VARIANT_TRUE;
      break;
  case DIG1:
      value.vt = VT_BOOL;
      if( now-m_tick > 1000 )
      {
          m_tick = now;
          value.boolVal = value.boolVal ? VARIANT_FALSE : VARIANT_TRUE;
      }
      break;
  case DIG5:
      value.vt = VT_BOOL;
      if( now-m_tick > 5000 )
      {
          m_tick = now;
          value.boolVal = value.boolVal ? VARIANT_FALSE : VARIANT_TRUE;
      }
      break;
  }
  CoFileTimeNow( &timestamp );
  quality = OPC_QUALITY_GOOD;
  changed = TRUE;
  return changed;
}

//*******************************************************************
//    class OPCGroup
//*******************************************************************
// Initialize does the real work
OPCGroup::OPCGroup()
{
}

//*******************************************************************
OPCGroup::~OPCGroup()
{
   // Destroy the message window   VB6
   if(m_hWnd != NULL)
      DestroyWindow();
}

//*******************************************************************
// OPCGroup overrides
//*******************************************************************
void OPCGroup::DoSetUpdateRate( DWORD newUpdateRate )
{
   m_updateRate = newUpdateRate;
   if( m_updateRate < 10 )
      m_updateRate = 10;
}

//*******************************************************************
// DoRead performs Sync and ASync read
//    Values are put into the OPCITEMSTATE structures,
//    not the COPCItems themselves
//    (there is no interaction with subscriptions)
//*******************************************************************
HRESULT OPCGroup::DoRead(
    OPCDATASOURCE   dwSource,
    DWORD           dwNumItems,
    COPCItem     ** ppItems,
    OPCITEMSTATE  * pItemValues,
    HRESULT       * pErrors)
{
   HRESULT hr = S_OK;
   // Base just initializes the arrays
   OPCGroupBase::DoRead(dwSource,dwNumItems,ppItems,pItemValues,pErrors);
   if( dwSource == OPC_DS_CACHE )
   {
      // for each item requested, read its value from the tag
      for( DWORD index=0; index<dwNumItems; index++ )
      {
         if( pErrors[index] == S_OK )
         {
            COPCItem* pItem = ppItems[index];

            CSLock wait( &m_cs );
            pItemValues[index].hClient = pItem->clientHandle;
            pItemValues[index].ftTimeStamp = pItem->timestamp;
            if( m_active && pItem->active )
               pItemValues[index].wQuality = pItem->quality;
            else
               pItemValues[index].wQuality = OPC_QUALITY_BAD | OPC_QUALITY_OUT_OF_SERVICE;
            pItemValues[index].wReserved = 0;
            VariantCopy( &pItemValues[index].vDataValue, &pItem->value );
         }
         else
         {
            hr = S_FALSE;
         }
      }
   }
   else  // OPC_DS_DEVICE
   {
      // for each item requested, return its data
      for( DWORD index=0; index<dwNumItems; index++ )
      {
         if( pErrors[index] == S_OK )
         {
            COPCItem* pItem = ppItems[index];

            CSLock wait( &m_cs );
            pItemValues[index].hClient = pItem->clientHandle;
            pItemValues[index].ftTimeStamp = pItem->timestamp;
            pItemValues[index].wQuality = pItem->quality;
            pItemValues[index].wReserved = 0;
            VariantCopy( &pItemValues[index].vDataValue, &pItem->value );
         }
         else
         {
            hr = S_FALSE;
         }
      }
   }
   return hr;
}

//*******************************************************************
// This is called from both Sync and ASync calls
HRESULT OPCGroup::DoWrite(
    DWORD        dwNumItems,
    COPCItem  ** ppItems,
    VARIANT    * pItemValues,
    HRESULT    * pErrors)
{
   HRESULT hr = S_OK;

   for( DWORD index=0; index<dwNumItems; index++ )
   {
      if( pErrors[index] == S_OK )
      {
         // server handle is the address of its Item
         COPCItem* pItem = ppItems[index];

         pItem->value = pItemValues[index];
         CoFileTimeNow( &pItem->timestamp );
      }
      else
      {
         hr = S_FALSE;
      }
   }
   return hr;
}

//*******************************************************************
BOOL OPCGroup::DoUpdateGroup()
{
   BOOL changed = FALSE;

   CSLock wait( &m_cs );
   // update items
   CFSItem* pItem = NULL;
   LPVOID key = 0;
   POSITION pos = m_itemMap.GetStartPosition();
   while( pos )
   {
      m_itemMap.GetNextAssoc( pos, key, (COPCItem*&)pItem );
      if( pItem )
          changed |= pItem->Simulate();
   }
   return changed;
}

//*******************************************************************
//  VB6
//	 Sends message to the main thread.  Visual Basic does not support
//  events fired directly from any thread other than the main thread
//  created by a VB project without marshalling.
//  
//*******************************************************************
void OPCGroup::DoUpdateClients()
{
	SendMessage(WM_UPDATECLIENTS, 0, 0);
}

void OPCGroup::DoAsyncUpdate(ASyncRequest* pRequest)
{
	SendMessage(WM_ASYNCUPDATE, (WPARAM)pRequest, 0);
}

void OPCGroup::DoAsyncUpdate2(ASyncRequest* pRequest)
{
	SendMessage(WM_ASYNCUPDATE2, (WPARAM)pRequest, 0);
}

//*******************************************************************
HRESULT OPCGroup::DoAddItems(
    DWORD            dwNumItems,
    OPCITEMDEF     * pItemArray,
    OPCITEMRESULT  * pAddResults,
    HRESULT        * pErrors)
{
   HRESULT hr = S_OK;
   CSLock wait( &m_cs );

   for(DWORD i=0; i<dwNumItems; i++ )
   {
      // Create the OPC item
      CFSItem* pItem = new CFSItem(pItemArray[i].szItemID);
      if( pItem )
      {
         pErrors[i] = S_OK;
         pItem->changed = TRUE;
         pItem->active = pItemArray[i].bActive;
         pItem->clientHandle = pItemArray[i].hClient;
         pItem->clientType = pItemArray[i].vtRequestedDataType;
         if( pItem->clientType == VT_EMPTY )
            pItem->clientType = VT_R8;
         pAddResults[i].hServer = (OPCHANDLE)pItem;
         pAddResults[i].vtCanonicalDataType = pItem->clientType;
         pAddResults[i].wReserved = 0;
         pAddResults[i].dwAccessRights = 3;
         pAddResults[i].dwBlobSize = 0;
         pAddResults[i].pBlob = NULL;
         m_itemMap.SetAt( (LPVOID)pItem, pItem );
      }
      else
      {
         pErrors[i] = E_OUTOFMEMORY;
         hr = S_FALSE;
      }
   }
   return hr;
}

//*******************************************************************
HRESULT OPCGroup::DoValidateItems(
    DWORD             dwNumItems,
    OPCITEMDEF      * pItemArray,
    OPCITEMRESULT   * pValidationResults,
    HRESULT         * pErrors)
{
   HRESULT hr = S_OK;
   CSLock wait( &m_cs );

   for(DWORD i=0; i<dwNumItems; i++ )
   {
      pValidationResults[i].hServer = NULL;
      pValidationResults[i].vtCanonicalDataType = pItemArray[i].vtRequestedDataType;
      pValidationResults[i].wReserved = 0;
      pValidationResults[i].dwAccessRights = 3;
      pValidationResults[i].dwBlobSize = 0;
      pValidationResults[i].pBlob = NULL;
   }
   return hr;
}

/* Don't need to override these
//*******************************************************************
HRESULT OPCGroup::DoRemoveItems(
    DWORD        dwNumItems,
    COPCItem  ** ppItems,
    HRESULT    * pErrors)
{
   HRESULT hr = S_OK;

   CTag** ppTags = (CTag**)_alloca(dwNumItems*sizeof(CTag*));
   CSLock wait( &m_cs );
   for( DWORD index=0; index<dwNumItems; index++ )
   {
      if( pErrors[index] == S_OK )
      {
         CFSItem* pItem = (CFSItem*)ppItems[index];
         // remove from map of all items
         m_itemMap.RemoveKey( (LPVOID)pItem );
         ppTags[index] = pItem->pTag;
         delete pItem;  // decrement tag's reference count
      }
      else
         hr = S_FALSE;
   }
   pCallback->Remove(index, ppTags);
   return hr;
}

//*******************************************************************
HRESULT OPCGroup::DoSetActiveState(
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
HRESULT OPCGroup::DoSetClientHandles(
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
HRESULT OPCGroup::DoSetDatatypes(
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
*/

IUnknown* OPCGroup::DoCreateEnumerator()
{
   CComEnumItemAttributes* pEnumerator = new CComEnumItemAttributes;
   if( pEnumerator )
      pEnumerator->Initialize(this);
   return pEnumerator;
}
//*******************************************************************
// CEnumItemAttributes implementation
//*******************************************************************
CEnumItemAttributes::CEnumItemAttributes()
{
   m_pos = NULL;
   m_parent = NULL;
}

CEnumItemAttributes::~CEnumItemAttributes()
{
   m_pos = NULL;
}

//*******************************************************************
void CEnumItemAttributes::Initialize(OPCGroup* pGroup)
{
   m_parent = pGroup;
   Reset();
}

//*******************************************************************
STDMETHODIMP CEnumItemAttributes::Next(
                                     ULONG celt,
                                     OPCITEMATTRIBUTES ** ppItemArray,
                                     ULONG * pceltFetched )
{
USES_CONVERSION;
   // All args should be valid
   if( ppItemArray==NULL )
      return E_INVALIDARG;

   *ppItemArray = NULL;
   OPCITEMATTRIBUTES * pItemArray = (OPCITEMATTRIBUTES*)CoTaskMemAlloc(celt*sizeof(OPCITEMATTRIBUTES));
   if( pItemArray == NULL )
      return E_OUTOFMEMORY;
   for( ULONG index = 0; index < celt && m_pos; index++ )
   {
      CFSItem* pItem = NULL;
      LPVOID key = 0;
      m_parent->m_itemMap.GetNextAssoc( m_pos, key, (COPCItem*&)pItem );

      // Get the access path (optional, may be empty);
      CString accessPath( _T("") );
      pItemArray[index].szAccessPath = (LPWSTR)CoTaskMemAlloc(2+accessPath.GetLength()*2);
      if( pItemArray[index].szAccessPath == NULL )
      {
         CoTaskMemFree( pItemArray );
         return E_OUTOFMEMORY;
      }
      wcscpy( pItemArray[index].szAccessPath, T2OLE(accessPath.GetBuffer(0)) );

      // Get the item name
      CString itemID( pItem->name );
      pItemArray[index].szItemID = (LPWSTR)CoTaskMemAlloc(2+itemID.GetLength()*2);
      if( pItemArray[index].szItemID == NULL )
      {
         CoTaskMemFree( pItemArray[index].szAccessPath );
         CoTaskMemFree( pItemArray );
         return E_OUTOFMEMORY;
      }
      wcscpy( pItemArray[index].szItemID, T2OLE(itemID.GetBuffer(0)) );
      pItemArray[index].bActive = pItem->active;
      pItemArray[index].hClient = pItem->clientHandle;
      pItemArray[index].hServer = (OPCHANDLE)pItem;
      pItemArray[index].dwAccessRights = 3;
      pItemArray[index].dwBlobSize = 0;
      pItemArray[index].pBlob = NULL;
      pItemArray[index].vtRequestedDataType = pItem->clientType;
      pItemArray[index].vtCanonicalDataType = pItem->clientType;
      pItemArray[index].pBlob = NULL;
      pItemArray[index].dwEUType = OPC_NOENUM;
      VariantInit( &pItemArray[index].vEUInfo );
   }
   if( pceltFetched )
      *pceltFetched = index;
   *ppItemArray = pItemArray;
   return (celt==index) ? S_OK : S_FALSE;
}

//*******************************************************************
// just iterate celt times to skip those items
STDMETHODIMP CEnumItemAttributes::Skip( ULONG celt )
{
   for( ULONG i = 0; i < celt && m_pos; i++ )
   {
      COPCItem* pItem = NULL;
      LPVOID key = 0;
      m_parent->m_itemMap.GetNextAssoc( m_pos, key, pItem );
   }
   return (celt==i) ? S_OK : S_FALSE;
}

//*******************************************************************
STDMETHODIMP CEnumItemAttributes::Reset( void )
{
   m_pos = m_parent->m_itemMap.GetStartPosition();
   return S_OK;
}

//*******************************************************************
STDMETHODIMP CEnumItemAttributes::Clone( IEnumOPCItemAttributes ** ppEnumItemAttributes )
{
   *ppEnumItemAttributes = NULL;
   CComEnumItemAttributes* pEnumerator = new CComEnumItemAttributes;
   pEnumerator->Initialize(m_parent);
   pEnumerator->m_pos = m_pos;

   return pEnumerator->QueryInterface( IID_IEnumOPCItemAttributes, (LPVOID*)ppEnumItemAttributes );
}

