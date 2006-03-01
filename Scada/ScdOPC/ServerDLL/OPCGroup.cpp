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

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern COPCCallback* pCallback;

//*******************************************************************
//    class COPCItem
//*******************************************************************
CFSItem::CFSItem(CTag* pTarget)
: pTag( pTarget )
  {
  pTag->AddRef();
  }

CFSItem::~CFSItem()
  {
  pTag->Release();
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
  Shutdown();

  EnterCriticalSection( &m_cs );
  // Also delete any removed items still around
  DWORD count = m_removedItems.GetCount();
  int index=0;
  if( count > 0 )
    {
    CTag** ppTags = (CTag**)_alloca(count*sizeof(CTag*));
    while( !m_removedItems.IsEmpty() )
      {
      CFSItem* pItem = (CFSItem*)m_removedItems.RemoveHead();
      m_itemMap.RemoveKey( (LPVOID)pItem );
      CTag* pTag = pItem->pTag;
      delete pItem;  // decrement tag's reference count
      if( !pTag->InUse() )
        ppTags[index++] = pTag;

      }
    pCallback->Remove(index, ppTags);
    }

  // remove all items
  CSLock wait( &m_cs );
  index=0;
  if( m_itemMap.GetCount() > 0 )
    {
    CTag** ppTags = (CTag**)_alloca(m_itemMap.GetCount()*sizeof(CTag*));
    CFSItem* pItem = NULL;
    LPVOID key = 0;
    POSITION pos = m_itemMap.GetStartPosition();
    while( pos )
      {
      m_itemMap.GetNextAssoc( pos, key, (COPCItem*&)pItem );
      CTag* pTag = pItem->pTag;
      delete pItem;  // decrement tag's reference count
      if( !pTag->InUse() )
        ppTags[index++] = pTag;
      }
    pCallback->Remove(index, ppTags);
    m_itemMap.RemoveAll();
    }
  LeaveCriticalSection( &m_cs );
  }

//*******************************************************************
// OPCGroup overrides
//*******************************************************************
void OPCGroup::DoSetUpdateRate( DWORD newUpdateRate )
  {
  if( !running )
    return;
  ASSERT( pCallback );
  m_updateRate = pCallback->SetUpdateRate(newUpdateRate);
  }

//*******************************************************************
// DoRead performs the synchronous read
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
  if( !running )
    return E_FAIL;
  ASSERT( pCallback );
  HRESULT hr = S_OK;
  // Base just initializes the arrays
  OPCGroupBase::DoRead(dwSource,dwNumItems,ppItems,pItemValues,pErrors);

  if( dwSource == OPC_DS_CACHE )   // just read from the tags
    {
    CSLock wait( &m_cs );
    CSLock wait2( &pCallback->m_CS );   // protect data
    // for each item requested, read its value from the tag
    for( DWORD index=0; index<dwNumItems; index++ )
      {
      if( pErrors[index] == S_OK )
        {
        CFSItem* pItem = (CFSItem*)ppItems[index];

        pItemValues[index].hClient = pItem->clientHandle;
        pItemValues[index].ftTimeStamp = pItem->pTag->m_timestamp;
        if( m_active && pItem->active )
          pItemValues[index].wQuality = pItem->pTag->m_quality;
        else
          pItemValues[index].wQuality = OPC_QUALITY_BAD | OPC_QUALITY_OUT_OF_SERVICE;
        pItemValues[index].wReserved = 0;
        VariantCopy( &pItemValues[index].vDataValue, &pItem->pTag->m_value );
        }
      else
        {
        hr = S_FALSE;
        }
      }
    }
  else  // OPC_DS_DEVICE
    {
    // get the tag pointers
    EnterCriticalSection( &m_cs );
    CTag** ppTags = (CTag**)_alloca(dwNumItems*sizeof(CTag*));
    for( DWORD index=0; index<dwNumItems; index++ )
      {
      CFSItem* pItem = (CFSItem*)ppItems[index];
      pItemValues[index].hClient = pItem->clientHandle;
      pItemValues[index].wReserved = 0;
      ppTags[index] = pItem->pTag;
      }
    LeaveCriticalSection( &m_cs );
    // Read the tags
    pCallback->Read(dwNumItems, ppTags, pErrors);

    // for each item requested, return its data
    CSLock wait( &m_cs );
    CSLock wait2( &pCallback->m_CS );   // protect data
    for( index=0; index<dwNumItems; index++ )
      {
      if( pErrors[index] == S_OK )
        {
        CFSItem* pItem = (CFSItem*)ppItems[index];

        // also update the item (added 2.02)
        pItem->value = pItem->pTag->m_value;
        pItem->quality = pItem->pTag->m_quality;
        pItem->timestamp = pItem->pTag->m_timestamp;

        pItemValues[index].ftTimeStamp = ppTags[index]->m_timestamp;
        pItemValues[index].wQuality = ppTags[index]->m_quality;
        VariantCopy( &pItemValues[index].vDataValue, &ppTags[index]->m_value );
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
HRESULT OPCGroup::DoWrite(
                          DWORD        dwNumItems,
                          COPCItem  ** ppItems,
                          VARIANT    * pItemValues,
                          HRESULT    * pErrors)
  {
  if( !running )
    return E_FAIL;
  ASSERT( pCallback );
  HRESULT hr = S_OK;

  EnterCriticalSection( &m_cs );
  // get the tag pointers
  CTag** ppTags = (CTag**)_alloca(dwNumItems*sizeof(CTag*));
  for( DWORD index=0; index<dwNumItems; index++ )
    {
    if(pErrors[index] == S_OK)
      {
      CFSItem* pItem = (CFSItem*)ppItems[index];
      ppTags[index] = pItem->pTag;
      }
    else
      ppTags[index] = NULL;
    }
  LeaveCriticalSection( &m_cs );
  // Write the tags
  hr = pCallback->Write(dwNumItems, ppTags, pItemValues, pErrors);

  return hr;
  }

//*******************************************************************
BOOL OPCGroup::DoUpdateGroup()
  {
  if( !running )
    return E_FAIL;
  ASSERT( pCallback );

  // delete the removed items now that it is safe
  EnterCriticalSection( &m_cs );
  DWORD count = m_removedItems.GetCount();
  int index=0;
  if( count > 0 )
    {
    CTag** ppTags = (CTag**)_alloca(count*sizeof(CTag*));
    while( !m_removedItems.IsEmpty() )
      {
      CFSItem* pItem = (CFSItem*)m_removedItems.RemoveHead();
      m_itemMap.RemoveKey( (LPVOID)pItem );
      CTag* pTag = pItem->pTag;
      delete pItem;  // decrement tag's reference count
      if( !pTag->InUse() )
        ppTags[index++] = pTag;
      }
    if( index > 0 )
      pCallback->Remove(index, ppTags);
    }

  BOOL changed = FALSE;
  if( m_itemMap.GetCount() == 0 )
    {
    LeaveCriticalSection( &m_cs );
    return changed;
    }

  // get the tag pointers in the group
  HRESULT* pErrors = (HRESULT*)_alloca(m_itemMap.GetCount()*sizeof(HRESULT));
  memset( pErrors, 0, m_itemMap.GetCount()*sizeof(HRESULT));
  CTag** ppTags = (CTag**)_alloca(m_itemMap.GetCount()*sizeof(CTag*));
  index = 0;
  CFSItem* pItem = NULL;
  LPVOID key = 0;
  POSITION pos = m_itemMap.GetStartPosition();
  while( pos )
    {
    m_itemMap.GetNextAssoc( pos, key, (COPCItem*&)pItem );
    ASSERT( pItem->pTag );
    if( pItem->active )
      {
      ppTags[index] = pItem->pTag;
      index++;
      }
    }
  LeaveCriticalSection( &m_cs );
  // Scan the tags
  pCallback->Scan(index, ppTags, pErrors);


  // update items from tags
  CSLock wait( &m_cs );
  pos = m_itemMap.GetStartPosition();
  while( pos )
    {
    m_itemMap.GetNextAssoc( pos, key, (COPCItem*&)pItem );
    CSLock wait( &pCallback->m_CS );   // protect data
    if( !pItem->active )
      continue;   // skip the change detection code

    // mark changed values to be sent to clients
    if( (pItem->value == (LPCVARIANT)&pItem->pTag->m_value ) )
      ;  // only "operator==" works, not "operator !="
    else  // the values are different
      {
      double high, low;
      if( m_deadBand == 0.0 )
        {
        pItem->value = pItem->pTag->m_value;
        pItem->changed = changed = TRUE;
        }
      // deadband only applies to "analog" types

      else if( pCallback->GetTagLimits( pItem->pTag, &high, &low ) )
        {
        double engRange = high - low;
        double current, last;
        BOOL incompatible = FALSE;
        switch( pItem->pTag->m_value.vt )
          {
          case VT_UI1: current = pItem->pTag->m_value.iVal; break; 
          case VT_I2: current = pItem->pTag->m_value.iVal; break;
          case VT_I4: current = pItem->pTag->m_value.lVal; break;
          case VT_R4: current = pItem->pTag->m_value.fltVal; break;
          case VT_R8: current = pItem->pTag->m_value.dblVal; break;
          default: incompatible=TRUE;
          };
        switch( pItem->value.vt )
          {
          case VT_UI1: last = pItem->value.iVal; break;
          case VT_I2: last = pItem->value.iVal; break;
          case VT_I4: last = pItem->value.lVal; break;
          case VT_R4: last = pItem->value.fltVal; break;
          case VT_R8: last = pItem->value.dblVal; break;
          default: incompatible=TRUE;
          };
        if( incompatible )  // do the assign
          {
          pItem->value = pItem->pTag->m_value;
          pItem->changed = changed = TRUE;
          }
        // Now check if the deadband is exceeded
        else if( fabs(current - last)
    > m_deadBand*engRange/100.0 ) // deadband is in %
          {
          pItem->value = pItem->pTag->m_value;
          pItem->changed = changed = TRUE;
          }

        }

      else  // deadband doesn't apply to other types
        {
        pItem->value = pItem->pTag->m_value;
        pItem->changed = changed = TRUE;
        }
      }
    if( pItem->quality != pItem->pTag->m_quality )
      {
      pItem->quality = pItem->pTag->m_quality;
      pItem->changed = changed = TRUE;
      }
    if( pItem->changed )
      pItem->timestamp = pItem->pTag->m_timestamp;
    }

  return changed;
  }

//******************************************************************
void OPCGroup::ClearRemovedItems()
  {
  // delete the removed items now that it is safe
  EnterCriticalSection( &m_cs );
  DWORD count = m_removedItems.GetCount();
  int index=0;
  if( count > 0 )
    {
    CTag** ppTags = (CTag**)_alloca(count*sizeof(CTag*));
    while( !m_removedItems.IsEmpty() )
      {
      CFSItem* pItem = (CFSItem*)m_removedItems.RemoveHead();
      m_itemMap.RemoveKey( (LPVOID)pItem );
      CTag* pTag = pItem->pTag;
      delete pItem;  // decrement tag's reference count
      if( !pTag->InUse() )
        ppTags[index++] = pTag;
      }
    if( index > 0 )
      pCallback->Remove(index, ppTags);
    }
  LeaveCriticalSection( &m_cs );
  }

//*******************************************************************
HRESULT OPCGroup::DoAddItems(
                             DWORD            dwNumItems,
                             OPCITEMDEF     * pItemArray,
                             OPCITEMRESULT  * pAddResults,
                             HRESULT        * pErrors)
  {
  if( !running )
    return E_FAIL;
  ASSERT( pCallback );
  HRESULT hr = S_OK;
  CSLock wait( &m_cs );

  for(DWORD i=0; i<dwNumItems; i++ )
    {
    // search for this tag
    CString itemName( pItemArray[i].szItemID );
    CString accessPath( pItemArray[i].szAccessPath );
    CTag* pTag = pCallback->AddTag( itemName, accessPath, pItemArray[i].vtRequestedDataType );
    if( pTag == NULL )
      {
      pErrors[i] = OPC_E_INVALIDITEMID;
      hr = S_FALSE;
      }
    else
      {
      // verify valid requested data type
      if(pItemArray[i].vtRequestedDataType == VT_I1 ||
        pItemArray[i].vtRequestedDataType == VT_I2 ||
        pItemArray[i].vtRequestedDataType == VT_I4 ||
        pItemArray[i].vtRequestedDataType == VT_UI1 ||
        pItemArray[i].vtRequestedDataType == VT_UI2 ||
        pItemArray[i].vtRequestedDataType == VT_UI4 ||
        pItemArray[i].vtRequestedDataType == VT_R4 ||
        pItemArray[i].vtRequestedDataType == VT_R8 ||
        pItemArray[i].vtRequestedDataType == VT_CY ||
        pItemArray[i].vtRequestedDataType == VT_DATE ||
        pItemArray[i].vtRequestedDataType == VT_BSTR ||
        pItemArray[i].vtRequestedDataType == VT_BOOL ||
        pItemArray[i].vtRequestedDataType == VT_EMPTY )
        {
        // Create the OPC item
        CFSItem* pItem = new CFSItem( pTag );
        if( pItem )
          {
          pErrors[i] = S_OK;
          pItem->changed = TRUE;
          pItem->active = pItemArray[i].bActive;
          pItem->clientHandle = pItemArray[i].hClient;
          pItem->clientType = pItemArray[i].vtRequestedDataType;
          if( pItem->clientType == VT_EMPTY )
            pItem->clientType = pItem->pTag->m_nativeType;
          pAddResults[i].hServer = (OPCHANDLE)pItem;
          pAddResults[i].vtCanonicalDataType = pItem->pTag->m_nativeType;
          pAddResults[i].wReserved = 0;
          pAddResults[i].dwAccessRights = pItem->accessRights = pItem->pTag->m_accessRights;
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
      else
        {
        pErrors[i] = OPC_E_BADTYPE;
        hr = S_FALSE;
        }
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
  if( !running )
    return E_FAIL;
  ASSERT( pCallback );
  HRESULT hr = S_OK;
  CSLock wait( &m_cs );

  for(DWORD i=0; i<dwNumItems; i++ )
    {
    // search for this tag
    CString itemName( pItemArray[i].szItemID );
    CString accessPath( pItemArray[i].szAccessPath );
    CTag tag;
    pErrors[i] = pCallback->ValidateTag( &tag, itemName, accessPath, pItemArray[i].vtRequestedDataType );
    if( FAILED(pErrors[i]) )
      {
      hr = S_FALSE;
      }
    else
      {
      pValidationResults[i].hServer = NULL;
      pValidationResults[i].vtCanonicalDataType = tag.m_nativeType;
      pValidationResults[i].wReserved = 0;
      pValidationResults[i].dwAccessRights = tag.m_accessRights;
      pValidationResults[i].dwBlobSize = 0;
      pValidationResults[i].pBlob = NULL;
      }
    }
  return hr;
  }

//*******************************************************************
HRESULT OPCGroup::DoRemoveItems(
                                DWORD        dwNumItems,
                                COPCItem  ** ppItems,
                                HRESULT    * pErrors)
  {

  if( !running )
    return E_FAIL;
  ASSERT( pCallback );
  HRESULT hr = S_OK;

  CSLock wait( &m_cs );
  for( DWORD index=0; index<dwNumItems; index++ )
    {
    if( pErrors[index] == S_OK )
      {
      CFSItem* pItem = (CFSItem*)ppItems[index];
      // remove from map of all items
      m_removedItems.AddHead( pItem );
      }
    else
      hr = S_FALSE;
    }
  return hr;
  }

/* Don't need to override these
//*******************************************************************
HRESULT OPCGroup::DoSetActiveState(
DWORD        dwNumItems,
COPCItem  ** ppItems,
BOOL         bActive,
HRESULT    * pErrors)
{
if( !running )
return E_FAIL;
HRESULT hr = S_OK;

CSLock wait( &cs );
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
if( !running )
return E_FAIL;
HRESULT hr = S_OK;

CSLock wait( &cs );
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
*/

//*******************************************************************
HRESULT OPCGroup::DoSetDatatypes(
                                 DWORD        dwNumItems,
                                 COPCItem  ** ppItems,
                                 VARTYPE    * pRequestedDatatypes,
                                 HRESULT    * pErrors)
  {
  if( !running )
    return E_FAIL;
  HRESULT hr = S_OK;

  CSLock wait( &m_cs );
  for( DWORD index=0; index<dwNumItems; index++ )
    {
    if( pErrors[index] == S_OK )
      {
      CFSItem* pItem = (CFSItem*)ppItems[index];
      pItem->clientType = pRequestedDatatypes[index];
      if( pItem->clientType == VT_EMPTY )    // if none, use Double
        pItem->clientType = pItem->pTag->m_nativeType;
      }
    else
      hr = S_FALSE;
    }
  return hr;
  }

IUnknown* OPCGroup::DoCreateEnumerator()
  {
  if( !running )
    return NULL;
  CComEnumItemAttributes* pEnumerator = new CComEnumItemAttributes;
  if( pEnumerator )
    pEnumerator->Initialize(this);
  return pEnumerator;
  }

//*******************************************************************
HRESULT OPCGroup::DoCloneGroupItems(OPCGroupBase* pCloneGroup)
  {
  HRESULT hr = S_OK;   
  CFSItem* pItem = NULL;
  LPVOID key = 0;

  POSITION position = m_itemMap.GetStartPosition();
  while( position )
    {
    m_itemMap.GetNextAssoc( position, key, (COPCItem*&)pItem );
    if( pItem )
      {
      // Create the OPC item
      CFSItem* pNewItem = new CFSItem( pItem->pTag );
      if( pNewItem )
        {
        pNewItem->changed = TRUE;
        pNewItem->active = pItem->active;
        pNewItem->clientHandle = pItem->clientHandle;
        pNewItem->clientType = pItem->clientType;
        ((OPCGroup*)pCloneGroup)->m_itemMap.SetAt( (LPVOID)pNewItem, pNewItem );
        }
      }
    else
      hr = E_OUTOFMEMORY;
    }
  return hr;
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
  if( !running )
    return E_FAIL;
  ASSERT( pCallback );
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
    CString accessPath( pCallback->GetTagAccessPath( pItem->pTag ) );
    pItemArray[index].szAccessPath = (LPWSTR)CoTaskMemAlloc(2+accessPath.GetLength()*2);
    if( pItemArray[index].szAccessPath == NULL )
      {
      CoTaskMemFree( pItemArray );
      return E_OUTOFMEMORY;
      }
    wcscpy( pItemArray[index].szAccessPath, T2OLE(accessPath.GetBuffer(0)) );

    // Get the item name
    CString itemID( pCallback->GetTagName( pItem->pTag ) );
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
    pItemArray[index].dwAccessRights = pItem->pTag->m_accessRights;
    pItemArray[index].dwBlobSize = 0;
    pItemArray[index].pBlob = NULL;
    pItemArray[index].vtRequestedDataType = pItem->clientType;
    pItemArray[index].vtCanonicalDataType = pItem->pTag->m_nativeType;
    pItemArray[index].pBlob = NULL;
    pItemArray[index].dwEUType = OPC_NOENUM;
    VariantInit( &pItemArray[index].vEUInfo );
    double high, low;
    if( pCallback->GetTagLimits( pItem->pTag, &high, &low ) )
      {
      pItemArray[index].dwEUType = OPC_ANALOG;
      SAFEARRAYBOUND bound;
      bound.lLbound = 0;
      bound.cElements = 2;
      SAFEARRAY *pArray = SafeArrayCreate(VT_R8, 1, &bound);
      if(pArray == NULL)
        return E_OUTOFMEMORY;
      LONG eu = 0;
      SafeArrayPutElement(pArray, &eu, (void *)&low);
      eu++;
      SafeArrayPutElement(pArray, &eu, (void *)&high);
      pItemArray[index].vEUInfo.vt = VT_ARRAY | VT_R8;
      pItemArray[index].vEUInfo.parray = pArray;
      }
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


