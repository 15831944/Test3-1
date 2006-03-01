//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  OPCGroup.h
//  $Author    :  Jim Hansen
//
//  Subsystem  :  OPC
//  Version    :  2.04
//
//  Description:  OPC Item and Group. See OPC Spec for details.
//                COPCItem stores per-client info relating to a tag
//
//**************************************************************************

#ifndef __OPCGROUP_H__              // Only Include Once
#define __OPCGROUP_H__

#include<atlwin.h>

#include "OPCGroupBase.h"

class OPCServer;
class CEnumItemAttributes;

//*******************************************************************
// A collection of COPCItem is held by the group to represent the tags on scan
class CFSItem : public COPCItem
{
public:
   CFSItem(LPWSTR itemID);
   ~CFSItem();
   BOOL Simulate();

   CString     name;
   enum SIMTYPE {RAMP,SINE,WAVE,RANDOM,DIG1,DIG5,TOGGLE};
   SIMTYPE     m_type;
   DWORD       m_tick;
};

//*******************************************************************
// Use to Initialize a critical section and automatically delete it
// when the object goes out of scope (including exceptions, etc).
class CSObject
{
public:
   CSObject(CRITICAL_SECTION* lock)
      : pLock(lock) {InitializeCriticalSection(pLock);}
   ~CSObject()
      {DeleteCriticalSection(pLock);}
private:
   CRITICAL_SECTION* pLock;
};

//*******************************************************************
// Use to enter a critical section and automatically leave it
// when the object goes out of scope (including exceptions, etc).
class CSLock
{
public:
   CSLock(CRITICAL_SECTION* lock)
      : pLock(lock) {EnterCriticalSection(pLock);}
   ~CSLock()
      {LeaveCriticalSection(pLock);}
private:
   CRITICAL_SECTION* pLock;
};

//*******************************************************************

#define WM_UPDATECLIENTS (WM_USER + 101)
#define WM_ASYNCUPDATE   (WM_USER + 102)
#define WM_ASYNCUPDATE2  (WM_USER + 103)

class ATL_NO_VTABLE OPCGroup :   
   public OPCGroupBase,
   public CWindowImpl<OPCGroup> // VB6
{
   friend CEnumItemAttributes;
public:
   OPCGroup();
   virtual ~OPCGroup();

   // VB6 - Message Handling
   // Visual Basic does not support events fired directly from any thread
   // other than the main thread created by the VB project without marshalling.
   BEGIN_MSG_MAP(OPCGroup)
	   MESSAGE_HANDLER(WM_UPDATECLIENTS, OnUpdateClients)
	   MESSAGE_HANDLER(WM_ASYNCUPDATE,   OnAsyncUpdate)
	   MESSAGE_HANDLER(WM_ASYNCUPDATE2,  OnAsyncUpdate2)
   END_MSG_MAP()

   LRESULT OnUpdateClients(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
   {
	   UpdateClients();
	   return 0;
   }

   LRESULT OnAsyncUpdate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
   {
	   AsyncUpdate((ASyncRequest*)wParam);
	   return 0;
   }

   LRESULT OnAsyncUpdate2(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
   {
	   AsyncUpdate2((ASyncRequest*)wParam);
	   return 0;
   }

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

	// VB6
   virtual void DoUpdateClients();

   virtual void DoAsyncUpdate(ASyncRequest* pRequest);

   virtual void DoAsyncUpdate2(ASyncRequest* pRequest);

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
/* No need to override these
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
*/

   virtual IUnknown* DoCreateEnumerator();

protected:

    // General attributes
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