//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  OPCServer.h
//  $Author    :  Jim Hansen
//
//  Subsystem  :  OPC
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

//*******************************************************************
class ATL_NO_VTABLE OPCServer :
   public OPCServerBase
{
public:
   OPCServer();
   ~OPCServer();

DECLARE_REGISTRY_RESOURCEID(IDR_OPCSERVER)

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
                              VARIANT   * ppvData,
                              HRESULT   * ppErrors);
   virtual HRESULT DoGetStatus( OPCSERVERSTATUS *pServerStatus);
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

	// VB6
   virtual OPCGroupObject* OPCServer::DoAddGroup(
                            LPCWSTR     szName,
                            BOOL        bActive,
                            DWORD       dwRequestedUpdateRate,
                            OPCHANDLE   hClientGroup,
                            LONG      * pTimeBias,
                            FLOAT     * pPercentDeadband,
                            DWORD       dwLCID,
                            OPCHANDLE * phServerGroup,
                            DWORD     * pRevisedUpdateRate);

protected:
};

//*******************************************************************
// A string enumerator for ItemIDs - used for browsing
class CEnumItemIDs :
   public IEnumString,
   public CComObjectRoot
{
public:
   CEnumItemIDs();
   ~CEnumItemIDs();
   void  Initialize( OPCBROWSETYPE     dwFilterType,
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

   OPCBROWSETYPE     m_BrowseFilterType;
   CString           m_FilterCriteria;
   VARTYPE           m_DataTypeFilter;
   DWORD             m_AccessRightsFilter;
};

typedef CComObject<CEnumItemIDs> CComEnumItemIDs;

#endif