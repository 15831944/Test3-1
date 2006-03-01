//**************************************************************************
//**************************************************************************
//
//        Copyright (c) FactorySoft, INC. 1997, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  OPCServerBase.h
//  $Author    :  Jim Hansen
//
//  Subsystem  :  OPC
//  Version    :  2.04
//
//  Description:  OPC Server and String Enumerator. See OPC Spec for details.
//                String Enumerator is used in Browse interface.
//
//**************************************************************************

#ifndef __OPCSERVERBASE_H__             // Only Include Once
#define __OPCSERVERBASE_H__

#pragma warning( disable : 4786 )

#ifndef OPC_ATL_INCLUDED      // if ATL hasn't been included already...

   #define _ATL_STATIC_REGISTRY

   #include <atlbase.h>
   //You may derive a class from CComModule and use it if you want to override
   //something, but do not change the name of _Module
   extern CComModule _Module;
   #include <atlcom.h>

   #include <statreg.h>

#endif //  OPC_ATL_INCLUDED

#include "OPCDa.h"
#include "OPCComn.h"
#include "OPCError.h"

extern UINT OPCSTMFORMATDATA;
extern UINT OPCSTMFORMATDATATIME;
extern UINT OPCSTMFORMATWRITECOMPLETE;

extern LPWSTR empty;
extern CComBSTR emptyString;
extern CDWordArray LCIDs;

//*******************************************************************
#include "OPCGroup.h"

extern "C" CLSID CLSID_OPCServer;

// The "standard" OPC pattern match function (used in browsing)
BOOL MatchPattern( LPCTSTR String, LPCTSTR Pattern, BOOL bCaseSensitive );

//*******************************************************************
class ATL_NO_VTABLE OPCServerBase :
   public IOPCServer,
   public IOPCCommon,
   public IOPCBrowseServerAddressSpace,
   public IOPCItemProperties,
   public IConnectionPointContainerImpl<OPCServerBase>,
   public IConnectionPointImpl<OPCServerBase, &IID_IOPCShutdown, CComDynamicUnkArray>,
   public CComObjectRootEx<CComMultiThreadModel>,
   public CComCoClass<OPCServerBase,&CLSID_OPCServer>
{
public:
                   OPCServerBase();
   virtual         ~OPCServerBase();
   OPCGroupObject* FindNamedGroup( LPCWSTR name );
   void            UpdateTime();
   void            ServerShutdown( LPTSTR reason );
   void GetUnkGroupList( OPCENUMSCOPE dwScope, LPUNKNOWN** GroupList, int &Count);
   void GetStrGroupList( OPCENUMSCOPE dwScope, CStringList &StringList ); 

BEGIN_COM_MAP(OPCServerBase)
   COM_INTERFACE_ENTRY(IOPCServer)
   COM_INTERFACE_ENTRY(IOPCCommon)
   COM_INTERFACE_ENTRY(IOPCBrowseServerAddressSpace)
   COM_INTERFACE_ENTRY(IOPCItemProperties)
   COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
END_COM_MAP()
DECLARE_NOT_AGGREGATABLE(OPCServer)

   // Connection Point
BEGIN_CONNECTION_POINT_MAP(OPCServerBase)
   CONNECTION_POINT_ENTRY(IID_IOPCShutdown)
END_CONNECTION_POINT_MAP()

   // IOPCServer
   STDMETHODIMP AddGroup(
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
      LPUNKNOWN * ppUnk);

   STDMETHODIMP GetErrorString(
      HRESULT     dwError,
      LCID        dwLocale,
      LPWSTR    * ppString);

   STDMETHODIMP GetGroupByName(
      LPCWSTR     szName,
      REFIID      riid,
      LPUNKNOWN * ppUnk);

   STDMETHODIMP GetStatus(
      OPCSERVERSTATUS **ppServerStatus);

   STDMETHODIMP RemoveGroup(
      OPCHANDLE   hServerGroup,
      BOOL     bForce);

   STDMETHODIMP CreateGroupEnumerator(
      OPCENUMSCOPE dwScope,
      REFIID      riid,
      LPUNKNOWN * ppUnk);

   // IOPCCommon
   STDMETHODIMP SetLocaleID(LCID dwLcid);

   STDMETHODIMP GetLocaleID(LCID * pdwLcid);

   STDMETHODIMP QueryAvailableLocaleIDs(
      DWORD          * pdwCount,
      LCID          ** pdwLcid);

   STDMETHODIMP GetErrorString(
      HRESULT          dwError,
      LPWSTR         * ppString);

   STDMETHODIMP SetClientName(LPCWSTR szName);

   // IOPCBrowseServerAddressSpace
   STDMETHODIMP QueryOrganization(
      OPCNAMESPACETYPE * pNameSpaceType);

   STDMETHODIMP ChangeBrowsePosition(
      OPCBROWSEDIRECTION dwBrowseDirection,
      LPCWSTR           szString);

   STDMETHODIMP BrowseOPCItemIDs(
      OPCBROWSETYPE     dwBrowseFilterType,
      LPCWSTR           szFilterCriteria,
      VARTYPE           vtDataTypeFilter,
      DWORD             dwAccessRightsFilter,
      LPENUMSTRING *    ppIEnumString);

   STDMETHODIMP GetItemID(
      LPWSTR      szItemDataID,
      LPWSTR *    szItemID);

   STDMETHODIMP BrowseAccessPaths(
      LPCWSTR        szItemID,
      LPENUMSTRING * ppIEnumString);

   // IOPCItemProperties
   STDMETHODIMP  QueryAvailableProperties(
      LPWSTR      szItemID,
      DWORD     * pdwCount,
      DWORD    ** ppPropertyIDs,
      LPWSTR   ** ppDescriptions,
      VARTYPE  ** ppvtDataTypes);

   STDMETHODIMP  GetItemProperties(
      LPWSTR      szItemID,
      DWORD       dwCount,
      DWORD     * pdwPropertyIDs,
      VARIANT  ** ppvData,
      HRESULT  ** ppErrors);

   STDMETHODIMP  LookupItemIDs(
      LPWSTR      szItemID,
      DWORD       dwCount,
      DWORD     * pdwPropertyIDs,
      LPWSTR   ** ppszNewItemIDs,
      HRESULT  ** ppErrors);

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
   virtual OPCGroupObject* DoAddGroup(
                              LPCWSTR     szName,
                              BOOL        bActive,
                              DWORD       dwRequestedUpdateRate,
                              OPCHANDLE   hClientGroup,
                              LONG      * pTimeBias,
                              FLOAT     * pPercentDeadband,
                              DWORD       dwLCID,
                              OPCHANDLE * phServerGroup,
                              DWORD     * pRevisedUpdateRate);
   virtual OPCNAMESPACETYPE DoQueryOrganization()=0;
   virtual HRESULT DoChangeBrowsePosition(
                              OPCBROWSEDIRECTION dwBrowseDirection,
                              LPCWSTR           szString)=0;
   virtual HRESULT DoBrowseOPCItemIDs(
                              OPCBROWSETYPE     dwBrowseFilterType,
                              LPCWSTR           szFilterCriteria,
                              VARTYPE           vtDataTypeFilter,
                              DWORD             dwAccessRightsFilter,
                              LPENUMSTRING *    ppIEnumString)=0;
   virtual HRESULT DoGetItemID(
                              LPWSTR      szItemDataID,
                              LPWSTR *    szItemID)=0;
   virtual HRESULT DoBrowseAccessPaths(
                              LPCWSTR        szItemID,
                              LPENUMSTRING * ppIEnumString);
   virtual HRESULT DoCreateGroupEnumerator(OPCENUMSCOPE dwScope, REFIID riid, LPUNKNOWN* ppUnk);

//protected:
   CRITICAL_SECTION  m_cs;
   GroupMap				m_groupMap;
   FILETIME				m_lastUpdateTime;
   LCID					m_localeID;
   CString				m_client;

};

typedef CTypedPtrList<CPtrList, OPCServerBase*> ServerList;

//*******************************************************************
// An Unknown enumerator for OPC Groups - used for enumerating the groups
class CEnumUnkGroups :
   public IEnumUnknown,
   public CComObjectRoot
{
friend CEnumUnkGroups;

public:
   CEnumUnkGroups();
   ~CEnumUnkGroups();
   void  Initialize( LPUNKNOWN* ppUnkGroupList, ULONG iTotal );

BEGIN_COM_MAP(CEnumUnkGroups)
   COM_INTERFACE_ENTRY(IEnumUnknown)
END_COM_MAP()

   STDMETHODIMP Next( ULONG celt,
                      IUnknown ** pUnk,
                      ULONG * pceltFetched );

   STDMETHODIMP Skip( ULONG celt );

   STDMETHODIMP Reset( void );

   STDMETHODIMP Clone( IEnumUnknown ** ppEnumUnknown );

private:
   ULONG          m_current;
   ULONG          m_totalCount;
	LPUNKNOWN*     m_Groups;
	OPCENUMSCOPE   m_Scope;
};

typedef CComObject<CEnumUnkGroups> CComEnumUnkGroups;

//*********************************************************************
// A String enumerator for OPC Groups - used for enumerating the groups
class CEnumStrGroups :
   public IEnumString,
   public CComObjectRoot
{
friend CEnumStrGroups;

public:
   CEnumStrGroups();
   ~CEnumStrGroups();
   void  Initialize( CStringList &StringList );

BEGIN_COM_MAP(CEnumStrGroups)
   COM_INTERFACE_ENTRY(IEnumString)
END_COM_MAP()

   STDMETHODIMP Next( ULONG celt,
                      LPOLESTR * ppStrings,
                      ULONG * pceltFetched );

   STDMETHODIMP Skip( ULONG celt );

   STDMETHODIMP Reset( void );

   STDMETHODIMP Clone( IEnumString ** ppEnunString );

private:
   POSITION          m_pos;
   CStringList       m_strings;

};

typedef CComObject<CEnumStrGroups> CComEnumStrGroups;

#endif