//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  OPCServer.h
//  $Author    :  Jim Hansen
//
//  Subsystem  :
//  Version    :  2.04
//
//  Description:  OPC Server and String Enumerator. See OPC Spec for details.
//                String Enumerator is used in Browse interface.
//
//**************************************************************************

#ifndef __OPCSERVER_H__             // Only Include Once
#define __OPCSERVER_H__

//**************************************************************************

#include "OPCServerBase.h"
#include "OPCGroup.h"
#include "resource.h"

class COPCBrowser;

//*******************************************************************
class ATL_NO_VTABLE OPCServer :
   public OPCServerBase
{
public:
   OPCServer();
   ~OPCServer();

#ifdef PLC5
DECLARE_REGISTRY_RESOURCEID(IDR_PLC5OPCSERVER)
#else
DECLARE_REGISTRY_RESOURCEID(IDR_OPCSERVER)
#endif

   // Overrides
   virtual HRESULT DoQueryNumProperties(
                              LPWSTR      szItemID,
                              DWORD     * pdwNumItems,
                              LPVOID    * ppVoid);
   virtual HRESULT DoQueryAvailableProperties(
                              LPWSTR      szItemID,
                              DWORD       dwNumItems,
                              LPVOID      pVoid,
                              DWORD     * pPropertyIDs,
                              LPWSTR    * pDescriptions,
                              VARTYPE   * pvtDataTypes);
   virtual HRESULT DoGetItemProperties(
                              LPWSTR      szItemID,
                              DWORD       dwNumItems,
                              DWORD     * pPropertyIDs,
                              VARIANT   * pData,
                              HRESULT   * pErrors);
   virtual HRESULT DoLookupItemIDs(
                              LPWSTR      szItemID,
                              DWORD       dwNumItems,
                              DWORD     * pPropertyIDs,
                              LPWSTR    * pszNewItemIDs,
                              HRESULT   * pErrors);
   virtual HRESULT DoGetStatus( OPCSERVERSTATUS *pServerStatus);
   virtual HRESULT DoGetErrorString(
                              HRESULT     dwError,
                              LCID        dwLocale,
                              LPWSTR      pString);
   virtual OPCNAMESPACETYPE DoQueryOrganization();
   virtual HRESULT DoChangeBrowsePosition(
                              OPCBROWSEDIRECTION dwBrowseDirection,
                              LPCWSTR           szString);
   virtual HRESULT DoBrowseOPCItemIDs(
                              OPCBROWSETYPE     dwBrowseFilterType,
                              LPCWSTR           szFilterCriteria,
                              VARTYPE           vtDataTypeFilter,
                              DWORD             dwAccessRightsFilter,
                              LPENUMSTRING *    ppIEnumString);
   virtual HRESULT DoGetItemID(
                              LPWSTR      szItemDataID,
                              LPWSTR *    szItemID);
   virtual HRESULT DoBrowseAccessPaths(
                              LPCWSTR        szItemID,
                              LPENUMSTRING * ppIEnumString);

protected:
   COPCBrowser*   m_pBrowser;
};

typedef CComObject<OPCServer> OPCServerObject;
//typedef CTypedPtrList<CPtrList, OPCServer*> ServerList;

//*******************************************************************
// A string enumerator for ItemIDs - used for browsing
class CEnumItemIDs :
   public IEnumString,
   public CComObjectRoot
{
public:
   CEnumItemIDs();
   ~CEnumItemIDs();
   void  Initialize( COPCBrowser*      pBrowser,
                     OPCBROWSETYPE     dwFilterType,
                     LPCWSTR           szCriteria,
                     VARTYPE           vtTypeFilter,
                     DWORD             dwRightsFilter);

BEGIN_COM_MAP(CEnumItemIDs)
   COM_INTERFACE_ENTRY(IEnumString)
END_COM_MAP()

   STDMETHODIMP Next( ULONG celt,
                      LPOLESTR * ppStrings,
                      ULONG * pceltFetched );

   STDMETHODIMP Skip( ULONG celt );

   STDMETHODIMP Reset( void );

   STDMETHODIMP Clone( IEnumString ** ppEnumString );

private:
   POSITION          m_pos;
   CStringList       m_strings;

   OPCBROWSETYPE     m_dwBrowseFilterType;
   LPCWSTR           m_szFilterCriteria;
   VARTYPE           m_vtDataTypeFilter;
   DWORD             m_dwAccessRightsFilter;
};

typedef CComObject<CEnumItemIDs> CComEnumItemIDs;

//*******************************************************************
// A string enumerator for ItemIDs - used for browsing
class CEnumAccessPaths :
   public IEnumString,
   public CComObjectRoot
{
public:
   CEnumAccessPaths();
   ~CEnumAccessPaths();
   HRESULT  Initialize( COPCBrowser* pBrowser,//Changed void to HRESULT
                     LPCWSTR szItemID);

BEGIN_COM_MAP(CEnumItemIDs)
   COM_INTERFACE_ENTRY(IEnumString)
END_COM_MAP()

   STDMETHODIMP Next( ULONG celt,
                      LPOLESTR * ppStrings,
                      ULONG * pceltFetched );

   STDMETHODIMP Skip( ULONG celt );

   STDMETHODIMP Reset( void );

   STDMETHODIMP Clone( IEnumString ** ppEnumString );

//private:
   POSITION          m_pos;
   CStringList       m_strings;
};

typedef CComObject<CEnumAccessPaths> CComEnumAccessPaths;

#endif