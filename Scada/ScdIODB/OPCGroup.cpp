//$T opcgroup.cpp GC 1.136 01/17/03 08:27:24
//$T opcgroup.cpp GC 1.136 01/17/03 08:25:38
//$T opcgroup.cpp GC 1.136 01/17/03 08:23:00
//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  OPCGroup.cpp
//  $Author    :  Jim Hansen
//
//  Subsystem  :  OPC
//  Version    :  2.04
//
//  Description:  The OPC Group object implementation.
//
//
//**************************************************************************
#include "stdafx.h"
#include "ListView.h"
#include "OPCServer.h"

//#include <process.h>
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
IMPLEMENT_DYNAMIC(OTopic, CObject)
//*******************************************************************
//    class COPCItem

//*******************************************************************
CModbusItem::CModbusItem(OModbusTag *pTarget) :
  pTag(pTarget)
  {
  pTag->AddRef();
  }

CModbusItem::~CModbusItem(void)
  {
  pTag->Release();
  }

//*******************************************************************
//    class OPCGroup
//*******************************************************************

// Initialize does the real work
OPCGroup::OPCGroup(void)
  {
  }

//*******************************************************************
OPCGroup::~OPCGroup(void)
  {
  // End threads, etc.
  Shutdown();

  // remove all topics (devices)
  CSLock  wait(&m_cs);  // protect data
  while(!m_topics.IsEmpty()) delete m_topics.RemoveHead();

  //
  //   // remove all items
  //   COPCItem* pItem = NULL;
  //   LPVOID key = 0;
  //   POSITION pos = m_itemMap.GetStartPosition();
  //   while( pos )
  //   {
  //      m_itemMap.GetNextAssoc( pos, key, pItem );
  //      delete pItem;
  //   }
  //   m_itemMap.RemoveAll();
  //
  // and any items previously removed
  while(!m_removedItems.IsEmpty())
    {
    CModbusItem *pItem = (CModbusItem *) m_removedItems.RemoveHead();
    delete pItem;       // decrement tag's reference count
    }
  }

//*******************************************************************
// OPCGroup overrides

//*******************************************************************
void OPCGroup::DoSetUpdateRate(DWORD newUpdateRate)
  {
  m_updateRate = newUpdateRate;
  if(m_updateRate < 10) m_updateRate = 10;
  }

//*******************************************************************
// DoRead performs Sync and ASync read
//    Values are put into the OPCITEMSTATE structures,
//    not the COPCItems themselves
//    (there is no interaction with subscriptions)

//*******************************************************************
HRESULT OPCGroup::DoRead
                  (
                    OPCDATASOURCE dwSource,
                    DWORD         dwNumItems,
                    COPCItem      **ppItems,
                    OPCITEMSTATE  *pItemValues,
                    HRESULT       *pErrors
                  )
  {
  HRESULT hr = S_OK;

  // Base just initializes the arrays
  OPCGroupBase::DoRead(dwSource, dwNumItems, ppItems, pItemValues, pErrors);
  if(dwSource == OPC_DS_CACHE)
    {
    // for each item requested, read its value from the tag
    for(DWORD index = 0; index < dwNumItems; index++)
      {
      if(pErrors[index] == S_OK)
        {
        // server handle is the address of its Item
        CModbusItem *pItem = (CModbusItem *) ppItems[index];
        ASSERT(pItem);

        CSLock  wait(&m_cs);
        CSLock  wait2(&tagCS);
        pItemValues[index].hClient = pItem->clientHandle;
        pItemValues[index].ftTimeStamp = pItem->pTag->timestamp;
        if(m_active && pItem->active)
          pItemValues[index].wQuality = pItem->pTag->quality;
        else
          pItemValues[index].wQuality = OPC_QUALITY_BAD | OPC_QUALITY_OUT_OF_SERVICE;
        pItemValues[index].wReserved = 0;
        VariantCopy(&pItemValues[index].vDataValue, &pItem->pTag->value);
        }
      else
        {
        hr = S_FALSE;
        }
      }
    }
  else  // OPC_DS_DEVICE
    {
    // for each item requested, read its value from the device
    for(DWORD index = 0; index < dwNumItems; index++)
      {
      if(pErrors[index] == S_OK)
        {
        // server handle is the address of its Item
        CModbusItem *pItem = (CModbusItem *) ppItems[index];
        ASSERT(pItem);

        OTagList  tags;
        tags.AddTail(pItem->pTag);
        pItem->pTag->pGroup->GetDevice()->UpdateTags(tags, 0);  // rate of 0 forces read
        CSLock  wait(&m_cs);
        CSLock  wait2(&tagCS);

        // also update the item (added 2.02)
        pItem->value = pItem->pTag->value;
        pItem->quality = pItem->pTag->quality;
        pItem->timestamp = pItem->pTag->timestamp;

        pItemValues[index].hClient = pItem->clientHandle;
        pItemValues[index].ftTimeStamp = pItem->pTag->timestamp;
        pItemValues[index].wQuality = pItem->pTag->quality;
        pItemValues[index].wReserved = 0;
        VariantCopy(&pItemValues[index].vDataValue, &pItem->pTag->value);
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
HRESULT OPCGroup::DoWrite
                  (
                    DWORD     dwNumItems,
                    COPCItem  **ppItems,
                    VARIANT   *pItemValues,
                    HRESULT   *pErrors
                  )
  {
  HRESULT hr = S_OK;
  CSLock  wait(&tagCS); // protect data from reads overwriting it!
  for(DWORD index = 0; index < dwNumItems; index++)
    {
    if(pErrors[index] == S_OK)
      {
      // server handle is the address of its Item
      CModbusItem *pItem = (CModbusItem *) ppItems[index];
      ASSERT(pItem);

      // check if tag is writable (OUTPUT_XXX)
      if(TRUE /* CNM Temp*/
        || pItem->pTag->m_IsOutput)        {
        pItem->pTag->value = pItemValues[index];
        CoFileTimeNow(&pItem->pTag->timestamp);

        // write the tag to the device
        BOOL ok = pItem->pTag->pGroup->GetDevice()->WriteTag( pItem->pTag );
        if( ok )
           pItem->changed = TRUE;
        pErrors[index] = S_OK;
        }
      else
        {
        pErrors[index] = OPC_E_BADRIGHTS;
        hr = S_FALSE;
        }
      }
    else
      {
      hr = S_FALSE;
      }
    }

  return hr;
  }

//*******************************************************************

// This is called from the group's scan thread (at its update rate)
BOOL OPCGroup::DoUpdateGroup(void)
  {
  BOOL  changed = FALSE;

  // delete any removed items now that it is safe
  EnterCriticalSection(&m_cs);
  while(!m_removedItems.IsEmpty())
    {
    CModbusItem *pItem = (CModbusItem *) m_removedItems.RemoveHead();

    // remove from scan list
    BOOL        found = FALSE;
    POSITION    pos = m_topics.GetHeadPosition();
    while(pos)
      {
      POSITION  savePos = pos;
      OTopic    *pTopic = m_topics.GetNext(pos);
      if(pTopic->pDevice == pItem->pTag->pGroup->GetDevice())
        {
        POSITION  itemPos = pTopic->tags.Find(pItem->pTag);

        //            ASSERT( itemPos != NULL );
        if(itemPos) pTopic->tags.RemoveAt(itemPos);

        // No longer delete topics. They will be reused if needed.
        // The group's destructor will delete all topics.
        }
      }

    delete pItem;           // decrement tag's reference count
    }

  LeaveCriticalSection(&m_cs);

  if(m_itemMap.GetCount() == 0) return changed;

  POSITION  topicPos = m_topics.GetHeadPosition();
  while(topicPos)
    {
    EnterCriticalSection(&m_cs);

    OTopic  *pTopic = m_topics.GetNext(topicPos);
    LeaveCriticalSection(&m_cs);

    // This does the device I/O (slow)
    pTopic->pDevice->UpdateTags(pTopic->tags, m_updateRate);
    }

  // update items from tags
  CSLock      wait(&m_cs);
  CModbusItem *pItem = NULL;
  LPVOID      key = 0;
  POSITION    pos = m_itemMap.GetStartPosition();
  while(pos)
    {
    m_itemMap.GetNextAssoc(pos, key, (COPCItem * &) pItem);

    //      if( !pItem->active )
    //         continue;
    CSLock  wait2(&tagCS);  // protect data

    // mark changed values to be sent to clients
    // MFC's COleVariant doesn't handle the VT_UI2 case...
    if(((pItem->pTag->value.vt != VT_UI2) && (pItem->value == (LPCVARIANT) pItem->pTag->value)) || 
       ((pItem->pTag->value.vt == VT_UI2) && (pItem->value.uiVal == pItem->pTag->value.uiVal)))
      {
      int xxx=0;
      }

    // only "operator==" works, not "operator !="
    else                    // the values are different
      {
      if(m_deadBand == 0.0)
        {
        pItem->value = pItem->pTag->value;
        pItem->changed = changed = TRUE;
        }

      // deadband only applies to "analog" types
      // which means a real type with scaling limits
      else if(pItem->pTag->value.vt == VT_R8 && pItem->value.vt == VT_R8
                  && pItem->pTag->enableProcessing)
        {
        double  engRange = pItem->pTag->pProcess->MaxEngRange -
          pItem->pTag->pProcess->MinEngRange;

        // Now check if the deadband is exceeded
        if(fabs(pItem->value.dblVal - pItem->pTag->value.dblVal
                 ) > m_deadBand * engRange / 100.0) // deadband is in %
          {
          pItem->value = pItem->pTag->value;
          pItem->changed = changed = TRUE;
          }
        }

      // also do floats because simulation doesn't convert to R8
      else if(pItem->pTag->value.vt == VT_R4 && pItem->value.vt == VT_R4
                  && pItem->pTag->enableProcessing)
        {
        double  engRange = pItem->pTag->pProcess->MaxEngRange -
          pItem->pTag->pProcess->MinEngRange;

        // Now check if the deadband is exceeded
        if(fabs(pItem->value.fltVal - pItem->pTag->value.fltVal
                 ) > m_deadBand * engRange / 100.0) // deadband is in %
          {
          pItem->value = pItem->pTag->value;
          pItem->changed = changed = TRUE;
          }
        }
      else  // deadband doesn't apply to other types
        {
        pItem->value = pItem->pTag->value;
        pItem->changed = changed = TRUE;
        }
      }

    if(pItem->quality != pItem->pTag->quality)
      {
      pItem->quality = pItem->pTag->quality;
      pItem->changed = changed = TRUE;
      }

    if(pItem->changed)
      {
      pItem->timestamp = pItem->pTag->timestamp;
      changed = TRUE;
      }
    }

  return changed;
  }

//*******************************************************************
HRESULT OPCGroup::DoAddItems
                  (
                    DWORD         dwNumItems,
                    OPCITEMDEF    *pItemArray,
                    OPCITEMRESULT *pAddResults,
                    HRESULT       *pErrors
                  )
  {
  HRESULT hr = S_OK;

  for(DWORD i = 0; i < dwNumItems; i++)
    {
    // search for this tag
    CString     itemName(pItemArray[i].szItemID);
    ODevice     *pDevice = NULL;
    OModbusTag  *pTag = (OModbusTag *) theDoc->FindTag
      (
        itemName,
        pItemArray[i].vtRequestedDataType,
        &pDevice
      );
    if(pTag == NULL || pDevice == NULL)
      {
      pErrors[i] = OPC_E_UNKNOWNITEMID;
      hr = S_FALSE;
      }
    else
      {
      // verify valid requested data type
      if(       pItemArray[i].vtRequestedDataType == VT_I1
             || pItemArray[i].vtRequestedDataType == VT_I2
             || pItemArray[i].vtRequestedDataType == VT_I4
             || pItemArray[i].vtRequestedDataType == VT_UI1
             || pItemArray[i].vtRequestedDataType == VT_UI2
             || pItemArray[i].vtRequestedDataType == VT_UI4
             || pItemArray[i].vtRequestedDataType == VT_R4
             || pItemArray[i].vtRequestedDataType == VT_R8
             || pItemArray[i].vtRequestedDataType == VT_CY
             || pItemArray[i].vtRequestedDataType == VT_DATE
             || pItemArray[i].vtRequestedDataType == VT_BSTR
             || pItemArray[i].vtRequestedDataType == VT_BOOL
             || pItemArray[i].vtRequestedDataType == VT_EMPTY)
        {
        // Create the OPC item
        CModbusItem *pItem = new CModbusItem(pTag);
        if(pItem)
          {
          pErrors[i] = S_OK;
          pItem->active = pItemArray[i].bActive;
          pItem->clientHandle = pItemArray[i].hClient;
          pItem->clientType = pItemArray[i].vtRequestedDataType;
          if(pItem->clientType == VT_EMPTY)
            pItem->clientType = pItem->pTag->nativeType;
          pAddResults[i].hServer = (OPCHANDLE) pItem;
          pAddResults[i].vtCanonicalDataType = pItem->pTag->nativeType;
          pAddResults[i].wReserved = 0;
          pAddResults[i].dwAccessRights = pItem->accessRights = OPC_READABLE;

          // check if tag is writable (OUTPUT_XXX)
          if(TRUE /* CNM TEMP*/ || pItem->pTag->m_IsOutput)
            {
            pAddResults[i].dwAccessRights |= OPC_WRITEABLE;
            pItem->accessRights |= OPC_WRITEABLE;
            }

          pAddResults[i].dwBlobSize = 0;
          pAddResults[i].pBlob = NULL;

          CSLock  wait(&m_cs);
          m_itemMap.SetAt((LPVOID) pItem, pItem);
          }
        else
          {
          pErrors[i] = E_OUTOFMEMORY;
          hr = S_FALSE;
          }

        if(pItem->active)
          {
          CSLock    wait(&m_cs);

          // Insert into lists for scanning
          BOOL      found = FALSE;
          POSITION  pos = m_topics.GetHeadPosition();
          while(pos)
            {
            OTopic  *pTopic = m_topics.GetNext(pos);
            if(pTopic->pDevice == pDevice)
              {
              InsertByAddress(pTag, pTopic->tags);
              found = TRUE;
              }
            }

          if(!found)
            {
            OTopic  *pTopic = new OTopic(pDevice, 0);
            m_topics.AddTail(pTopic);
            InsertByAddress(pTag, pTopic->tags);
            }
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
HRESULT OPCGroup::DoValidateItems
                  (
                    DWORD         dwNumItems,
                    OPCITEMDEF    *pItemArray,
                    OPCITEMRESULT *pValidationResults,
                    HRESULT       *pErrors
                  )
  {
  HRESULT hr = S_OK;

  for(DWORD i = 0; i < dwNumItems; i++)
    {
    // search for this tag
    CString     itemName(pItemArray[i].szItemID);
    ODevice     *pDevice = NULL;
    OModbusTag  *pTag = (OModbusTag *) theDoc->FindTag(itemName, VT_NULL, &pDevice);
    if(pTag == NULL)
      {
      pErrors[i] = OPC_E_UNKNOWNITEMID;
      hr = S_FALSE;
      }
    else
      {
      pTag->AddRef();
      pValidationResults[i].hServer = NULL;
      pValidationResults[i].vtCanonicalDataType = pTag->nativeType;
      pValidationResults[i].wReserved = 0;
      pValidationResults[i].dwAccessRights = OPC_READABLE;

      // check if tag is writable (OUTPUT_XXX)
      if ( TRUE /* CNM TEMP*/ || pTag->m_IsOutput)
        {
        pValidationResults[i].dwAccessRights |= OPC_WRITEABLE;
        }

      pValidationResults[i].dwBlobSize = 0;
      pValidationResults[i].pBlob = NULL;

      // Release also removes temporary tags
      pTag->Release();
      }
    }

  return hr;
  }

//*******************************************************************
HRESULT OPCGroup::DoRemoveItems
                  (
                    DWORD     dwNumItems,
                    COPCItem  **ppItems,
                    HRESULT   *pErrors
                  )
  {
  HRESULT hr = S_OK;
  CSLock  wait(&m_cs);
  for(DWORD index = 0; index < dwNumItems; index++)
    {
    if(pErrors[index] == S_OK)
      {
      // server handle is the address of its Item
      CModbusItem *pItem = (CModbusItem *) ppItems[index];
      ASSERT(pItem);

      // remove from map of all items
      VERIFY(m_itemMap.RemoveKey((LPVOID) pItem));
      m_removedItems.AddHead(pItem);
      }
    else
      hr = S_FALSE;
    }

  return hr;
  }

//*******************************************************************
HRESULT OPCGroup::DoSetActiveState
                  (
                    DWORD     dwNumItems,
                    COPCItem  **ppItems,
                    BOOL      bActive,
                    HRESULT   *pErrors
                  )
  {
  HRESULT hr = S_OK;
  CSLock  wait(&m_cs);
  for(DWORD index = 0; index < dwNumItems; index++)
    {
    if(pErrors[index] == S_OK)
      {
      // server handle is the address of its Item
      CModbusItem *pItem = (CModbusItem *) ppItems[index];
      ASSERT(pItem);

      BOOL  change = (pItem->active != bActive);
      pItem->active = bActive;
      if(pItem->active && change)
        {
        // Insert into lists for scanning
        BOOL      found = FALSE;
        POSITION  pos = m_topics.GetHeadPosition();
        while(pos)
          {
          OTopic  *pTopic = m_topics.GetNext(pos);
          if(pTopic->pDevice == pItem->pTag->pGroup->GetDevice())
            {
            InsertByAddress(pItem->pTag, pTopic->tags);
            found = TRUE;
            }
          }

        if(!found)
          {
          OTopic  *pTopic = new OTopic(pItem->pTag->pGroup->GetDevice(), 0);
          m_topics.AddTail(pTopic);
          InsertByAddress(pItem->pTag, pTopic->tags);
          }
        }
      else if(change)
        {
        // remove from scan list
        BOOL      found = FALSE;
        POSITION  pos = m_topics.GetHeadPosition();
        while(pos)
          {
          POSITION  savePos = pos;
          OTopic    *pTopic = m_topics.GetNext(pos);
          if(pTopic->pDevice == pItem->pTag->pGroup->GetDevice())
            {
            POSITION  itemPos = pTopic->tags.Find(pItem->pTag);
            pTopic->tags.RemoveAt(itemPos);
            if(pTopic->tags.IsEmpty())
              {
              //                     m_topics.RemoveAt( savePos );
              //                     delete pTopic;
              }
            }
          }
        }
      }
    else
      hr = S_FALSE;
    }

  return hr;
  }

//*******************************************************************
HRESULT OPCGroup::DoSetClientHandles
                  (
                    DWORD     dwNumItems,
                    COPCItem  **ppItems,
                    OPCHANDLE *phClient,
                    HRESULT   *pErrors
                  )
  {
  HRESULT hr = S_OK;

  CSLock  wait(&m_cs);
  for(DWORD index = 0; index < dwNumItems; index++)
    {
    if(pErrors[index] == S_OK)
      {
      // server handle is the address of its Item
      CModbusItem *pItem = (CModbusItem *) ppItems[index];
      ASSERT(pItem);
      pItem->clientHandle = phClient[index];
      }
    else
      hr = S_FALSE;
    }

  return hr;
  }

//*******************************************************************
HRESULT OPCGroup::DoSetDatatypes
                  (
                    DWORD     dwNumItems,
                    COPCItem  **ppItems,
                    VARTYPE   *pRequestedDatatypes,
                    HRESULT   *pErrors
                  )
  {
  HRESULT hr = S_OK;

  CSLock  wait(&m_cs);
  for(DWORD index = 0; index < dwNumItems; index++)
    {
    if(pErrors[index] == S_OK)
      {
      // server handle is the address of its Item
      CModbusItem *pItem = (CModbusItem *) ppItems[index];
      ASSERT(pItem);
      pItem->clientType = pRequestedDatatypes[index];
      if(pItem->clientType == VT_EMPTY) // if none, use Double
        pItem->clientType = VT_R8;
      }
    else
      hr = S_FALSE;
    }

  return hr;
  }

//*******************************************************************
IUnknown *OPCGroup::DoCreateEnumerator(void)
  {
  CComEnumItemAttributes  *pEnumerator = new CComEnumItemAttributes;
  if(pEnumerator) pEnumerator->Initialize(this);
  return pEnumerator;
  }

//*******************************************************************
HRESULT OPCGroup::DoCloneGroupItems(OPCGroupBase *pCloneGroup)
  {
  HRESULT     hr = S_OK;
  CModbusItem *pItem = NULL;
  LPVOID      key = 0;

  POSITION    position = m_itemMap.GetStartPosition();
  while(position)
    {
    m_itemMap.GetNextAssoc(position, key, (COPCItem * &) pItem);
    if(pItem)
      {
      // Create the OPC item
      CModbusItem *pNewItem = new CModbusItem(pItem->pTag);
      if(pNewItem)
        {
        pNewItem->changed = TRUE;
        pNewItem->active = pItem->active;
        pNewItem->clientHandle = pItem->clientHandle;
        pNewItem->clientType = pItem->clientType;
        ((OPCGroup *) pCloneGroup)->m_itemMap.SetAt((LPVOID) pNewItem, pNewItem);
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
CEnumItemAttributes::CEnumItemAttributes(void)
  {
  m_pos = NULL;
  m_parent = NULL;
  }

CEnumItemAttributes::~CEnumItemAttributes(void)
  {
  m_pos = NULL;
  }

//*******************************************************************
void CEnumItemAttributes::Initialize(OPCGroup *pGroup)
  {
  m_parent = pGroup;
  Reset();
  }

//*******************************************************************
STDMETHODIMP CEnumItemAttributes::Next
                                  (
                                    ULONG             celt,
                                    OPCITEMATTRIBUTES **ppItemArray,
                                    ULONG             *pceltFetched
                                  )
  {
  USES_CONVERSION;

  // All args should be valid
  if(ppItemArray == NULL) return E_INVALIDARG;

  *ppItemArray = NULL;

  OPCITEMATTRIBUTES *pItemArray = (OPCITEMATTRIBUTES *) CoTaskMemAlloc(celt * sizeof(OPCITEMATTRIBUTES));
  for(ULONG index = 0; index < celt && m_pos; index++)
    {
    CModbusItem *pItem = NULL;
    LPVOID      key = 0;
    CSLock      wait(&m_parent->m_cs);
    m_parent->m_itemMap.GetNextAssoc(m_pos, key, (COPCItem * &) pItem);

    pItemArray[index].szAccessPath = (LPWSTR) CoTaskMemAlloc(4);
    wcscpy(pItemArray[index].szAccessPath, L"");

    CString path(pItem->pTag->pGroup->GetPathName());
    path += pItem->pTag->name;

    pItemArray[index].szItemID = (LPWSTR) CoTaskMemAlloc(2 + path.GetLength() * 2);
    wcscpy(pItemArray[index].szItemID, T2OLE(path.GetBuffer(0)));
    pItemArray[index].bActive = pItem->active;
    pItemArray[index].hClient = pItem->clientHandle;
    pItemArray[index].hServer = (OPCHANDLE) pItem;
    pItemArray[index].dwAccessRights = OPC_READABLE;

    // check if tag is writable (OUTPUT_XXX)
    if(TRUE /* CNM TEMP*/ || pItem->pTag->m_IsOutput)
      {
      pItemArray[index].dwAccessRights |= OPC_WRITEABLE;
      }

    pItemArray[index].dwBlobSize = 0;
    pItemArray[index].pBlob = NULL;
    pItemArray[index].vtRequestedDataType = pItem->clientType;
    pItemArray[index].vtCanonicalDataType = pItem->pTag->value.vt;
    pItemArray[index].pBlob = NULL;
    pItemArray[index].dwEUType = OPC_NOENUM;
    VariantInit(&pItemArray[index].vEUInfo);
    if(pItem->pTag->enableProcessing && pItem->pTag->pProcess)
      {
      pItemArray[index].dwEUType = OPC_ANALOG;

      SAFEARRAYBOUND  bound;
      bound.lLbound = 0;
      bound.cElements = 2;

      SAFEARRAY *pArray = SafeArrayCreate(VT_R8, 1, &bound);
      if(pArray == NULL) return E_OUTOFMEMORY;

      LONG  eu = 0;
      SafeArrayPutElement(pArray, &eu, (void *) &pItem->pTag->pProcess->MinEngRange);
      eu++;
      SafeArrayPutElement(pArray, &eu, (void *) &pItem->pTag->pProcess->MaxEngRange);
      pItemArray[index].vEUInfo.vt = VT_ARRAY | VT_R8;
      pItemArray[index].vEUInfo.parray = pArray;
      }
    }

  if(pceltFetched) *pceltFetched = index;
  *ppItemArray = pItemArray;
  return(celt == index) ? S_OK : S_FALSE;
  }

//*******************************************************************

// just iterate celt times to skip those items
STDMETHODIMP CEnumItemAttributes::Skip(ULONG celt)
  {
  for(ULONG i = 0; i < celt && m_pos; i++)
    {
    COPCItem  *pItem = NULL;
    LPVOID    key = 0;
    m_parent->m_itemMap.GetNextAssoc(m_pos, key, pItem);
    }

  return(celt == i) ? S_OK : S_FALSE;
  }

//*******************************************************************
STDMETHODIMP CEnumItemAttributes::Reset(void)
  {
  m_pos = m_parent->m_itemMap.GetStartPosition();
  return S_OK;
  }

//*******************************************************************
STDMETHODIMP CEnumItemAttributes::Clone
                                  (IEnumOPCItemAttributes  **ppEnumItemAttributes)
  {
  *ppEnumItemAttributes = NULL;

  CComEnumItemAttributes  *pEnumerator = new CComEnumItemAttributes;
  pEnumerator->Initialize(m_parent);
  pEnumerator->m_pos = m_pos;

  return pEnumerator->QueryInterface
    (
      IID_IEnumOPCItemAttributes,
      (LPVOID *) ppEnumItemAttributes
    );
  }
