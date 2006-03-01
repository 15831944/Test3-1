//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  OPCGroup.h
//  $Author    :  Jim Hansen
//
//  Subsystem  :
//
//  Description:  OPC Item and Group. See OPC Spec for details.
//                COPCItem stores per-client info relating to a tag
//
//**************************************************************************

#ifndef __OPCGROUP_H__              // Only Include Once
#define __OPCGROUP_H__

#include "OPCGroupBase.h"
#include "resource.h"
#include "servercb.h"

class OPCServer;
class CEnumItemAttributes;

typedef CTypedPtrList<CPtrList, COPCItem*> ItemList;

//*******************************************************************
// A collection of COPCItem is held by the group to represent the tags on scan
class CModbusItem : public COPCItem
{
public:
   CModbusItem(OModbusTag*);
   ~CModbusItem();

   OModbusTag* pTag;      // pointer into tag database
};

//*******************************************************************
class ATL_NO_VTABLE OPCGroup :
   public OPCGroupBase
{
   friend CEnumItemAttributes;
public:
   OPCGroup();
   virtual ~OPCGroup();

   // Overrides
   virtual void DoSetUpdateRate( DWORD newUpdateRate );
   virtual HRESULT DoRead(
                              OPCDATASOURCE    dwSource,
                              DWORD            dwNumItems,
                              COPCItem      ** ppItems,
                              OPCITEMSTATE   * pItemValues,
                              HRESULT        * pErrors);
   virtual HRESULT DoWrite(
                              DWORD            dwNumItems,
                              COPCItem      ** ppItems,
                              VARIANT        * pItemValues,
                              HRESULT        * pErrors);
   virtual BOOL DoUpdateGroup();
   virtual HRESULT DoAddItems(
                              DWORD            dwNumItems,
                              OPCITEMDEF     * pItemArray,
                              OPCITEMRESULT  * pAddResults,
                              HRESULT        * pErrors);
   virtual HRESULT DoValidateItems(
                              DWORD            dwNumItems,
                              OPCITEMDEF     * pItemArray,
                              OPCITEMRESULT  * pValidationResults,
                              HRESULT        * pErrors);
   virtual HRESULT DoRemoveItems(
                              DWORD        dwNumItems,
                              COPCItem  ** ppItems,
                              HRESULT    * pErrors);
   virtual HRESULT DoSetActiveState(
                              DWORD        dwNumItems,
                              COPCItem  ** ppItems,
                              BOOL         bActive,
                              HRESULT    * pErrors);
   virtual HRESULT DoSetClientHandles(
                              DWORD        dwNumItems,
                              COPCItem  ** ppItems,
                              OPCHANDLE  * phClient,
                              HRESULT    * pErrors);
   virtual HRESULT DoSetDatatypes(
                              DWORD        dwNumItems,
                              COPCItem  ** ppItems,
                              VARTYPE    * pRequestedDatatypes,
                              HRESULT    * pErrors);
   virtual IUnknown* DoCreateEnumerator();
   virtual HRESULT DoCloneGroupItems(OPCGroupBase* pGroup);

protected:
   ItemList      m_removedItems;

    // General attributes
   OTopicList    m_topics;        // ordered lists for scanning
};

typedef CComObject<OPCGroup> OPCGroupObject;
typedef CTypedPtrMap<CMapPtrToPtr, LPVOID, OPCGroupObject*> GroupMap;

//*******************************************************************
// The group's item attributes enumerator object
class CEnumItemAttributes :
   public IEnumOPCItemAttributes,
   public CComObjectRoot
{
   friend class CEnumItemAttributes;
public:
   CEnumItemAttributes();
   ~CEnumItemAttributes();
   void  Initialize(OPCGroup* pGroup);

BEGIN_COM_MAP(CEnumItemAttributes)
   COM_INTERFACE_ENTRY(IEnumOPCItemAttributes)
END_COM_MAP()

  // Enumerator
  STDMETHODIMP Next(
                ULONG celt,
                OPCITEMATTRIBUTES ** ppItemArray,
                ULONG * pceltFetched );

  STDMETHODIMP Skip( ULONG celt );

  STDMETHODIMP Reset( void );

  STDMETHODIMP Clone( IEnumOPCItemAttributes ** ppEnumItemAttributes );


private:
   POSITION  m_pos;  // iterator for any of the things we may enum (devices or tags)
   OPCGroup* m_parent;
};

typedef CComObject<CEnumItemAttributes> CComEnumItemAttributes;

#endif