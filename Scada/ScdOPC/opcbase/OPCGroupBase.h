//**************************************************************************
//**************************************************************************
//
//        Copyright (c) FactorySoft, INC. 1997, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  OPCGroupBase.h
//  $Author    :  Jim Hansen
//
//  Subsystem  :  OPC
//  Version    :  2.04
//
//  Description:  OPC Item and Group. See OPC Spec for details.
//                COPCItem stores per-client info relating to a tag
//
//**************************************************************************

#ifndef __OPCGROUPBASE_H__              // Only Include Once
#define __OPCGROUPBASE_H__

class OPCServerBase;

//*******************************************************************
// A collection of COPCItems is held by the group to represent the tags on scan
class COPCItem
{
public:
   COPCItem();
   virtual ~COPCItem();

   BOOL        active;        // is data to be sent to client
   OPCHANDLE   clientHandle;  // client generated handle
   VARTYPE     clientType;    // datatype client wants to see
   COleVariant value;         // last value sent to this client
   WORD        quality;
   FILETIME    timestamp;
   BOOL        changed;       // flag to update client
   DWORD       accessRights;  // item access rights
};
typedef CTypedPtrMap<CMapPtrToPtr, LPVOID, COPCItem*> ItemMap;

//*******************************************************************
// This stores the information for an ASyncIO request
class ASyncRequest
{
public:
   ASyncRequest() : handles(NULL), values(NULL), errors(NULL) {}
   ~ASyncRequest() { delete [] handles; delete [] values; delete [] errors; }

   DWORD       dwConnection;
   DWORD       dwNumItems;
   OPCDATASOURCE   dwSource;
   OPCHANDLE*  handles;       // an array of handles
   VARIANT*    values;        // an array of VARIANTs for writes
   HRESULT*    errors;        // an array of HRESULTs for writes
   DWORD       dwTransactionID;
   enum  TYPE  { READ, WRITE, REFRESH, READCP, WRITECP, REFRESHCP };
   TYPE        type;
};
typedef CTypedPtrList<CPtrList, ASyncRequest*> ASyncRequestList;

class OPCGroupBase;
typedef IConnectionPointImpl<OPCGroupBase, &IID_IOPCDataCallback, CComDynamicUnkArray> DataCallbackCP;

//*******************************************************************
class OPCGroupBase :
   public IOPCGroupStateMgt,
   public IOPCItemMgt,
   public IOPCSyncIO,
   public IOPCAsyncIO,
   public IDataObject,
   public IOPCAsyncIO2,
   public IConnectionPointContainerImpl<OPCGroupBase>,
   public DataCallbackCP,
   public CComObjectRootEx<CComMultiThreadModel>
{
public:
   OPCGroupBase();
   virtual  ~OPCGroupBase();
   virtual void Initialize(LPCWSTR szName,
             BOOL bActive,
             DWORD dwRequestedUpdateRate,
             OPCHANDLE hClientGroup,
             LONG* pTimeBias,
             FLOAT* pPercentDeadband,
             DWORD dwLCID,
             OPCServerBase* pServer);
   void    Shutdown();
   CString GetGroupName() {return m_name;}

BEGIN_COM_MAP(OPCGroupBase)
   COM_INTERFACE_ENTRY(IOPCGroupStateMgt)
   COM_INTERFACE_ENTRY(IOPCItemMgt)
   COM_INTERFACE_ENTRY(IOPCSyncIO)
   COM_INTERFACE_ENTRY(IOPCAsyncIO)
   COM_INTERFACE_ENTRY(IDataObject)
   COM_INTERFACE_ENTRY(IOPCAsyncIO2)
   COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
END_COM_MAP()

DECLARE_NOT_AGGREGATABLE(OPCGroupBase)

   // Connection Point
BEGIN_CONNECTION_POINT_MAP(OPCGroupBase)
   CONNECTION_POINT_ENTRY(IID_IOPCDataCallback)
END_CONNECTION_POINT_MAP()

STDMETHODIMP Advise( IUnknown *pUnk, DWORD *pdwCookie);

   // IOPCGroupStateMgt interface
   STDMETHODIMP GetState(
    DWORD     * pUpdateRate,
    BOOL      * pActive,
    LPWSTR    * ppName,
    LONG      * pTimeBias,
    FLOAT     * pPercentDeadband,
    DWORD     * pLCID,
    OPCHANDLE * phClientGroup,
    OPCHANDLE * phServerGroup );

  STDMETHODIMP SetState(
    DWORD     * pRequestedUpdateRate,
    DWORD     * pRevisedUpdateRate,
    BOOL      * pActive,
    LONG      * pTimeBias,
    FLOAT     * pPercentDeadband,
    DWORD     * pLCID,
    OPCHANDLE * phClientGroup );

  STDMETHODIMP SetName( LPCWSTR szName );

  STDMETHODIMP CloneGroup(
    LPCWSTR     szName,
    REFIID      riid,
    LPUNKNOWN * ppUnk );

  // IOPCSyncIO
  STDMETHODIMP Read(
    OPCDATASOURCE   dwSource,
    DWORD           dwNumItems,
    OPCHANDLE     * phServer,
    OPCITEMSTATE ** ppItemValues,
    HRESULT      ** ppErrors );

  STDMETHODIMP Write(
    DWORD        dwNumItems,
    OPCHANDLE  * phServer,
    VARIANT    * pItemValues,
    HRESULT   ** ppErrors );

  // IOPCItemMgt
  STDMETHODIMP AddItems(
    DWORD            dwNumItems,
    OPCITEMDEF     * pItemArray,
    OPCITEMRESULT ** ppAddResults,
    HRESULT       ** ppErrors );

  STDMETHODIMP ValidateItems(
    DWORD             dwNumItems,
    OPCITEMDEF      * pItemArray,
    BOOL              bBlobUpdate,
    OPCITEMRESULT  ** ppValidationResults,
    HRESULT        ** ppErrors );

  STDMETHODIMP RemoveItems(
    DWORD        dwNumItems,
    OPCHANDLE  * phServer,
    HRESULT   ** ppErrors );

  STDMETHODIMP SetActiveState(
   DWORD        dwNumItems,
   OPCHANDLE  * phServer,
   BOOL         bActive,
   HRESULT   ** ppErrors );

  STDMETHODIMP SetClientHandles(
    DWORD        dwNumItems,
    OPCHANDLE  * phServer,
    OPCHANDLE  * phClient,
    HRESULT   ** ppErrors );

  STDMETHODIMP SetDatatypes(
    DWORD        dwNumItems,
    OPCHANDLE  * phServer,
    VARTYPE    * pRequestedDatatypes,
    HRESULT   ** ppErrors );

  STDMETHODIMP CreateEnumerator(
    REFIID      riid,
    LPUNKNOWN * ppUnk );

  // IOPCAsyncIO2
  STDMETHODIMP Read(
    DWORD           dwNumItems,
    OPCHANDLE     * phServer,
    DWORD           TransactionID,
    DWORD         * pCancelID,
    HRESULT      ** ppErrors );

  STDMETHODIMP Write(
    DWORD       dwNumItems,
    OPCHANDLE * phServer,
    VARIANT   * pItemValues,
    DWORD       TransactionID,
    DWORD     * pCancelID,
    HRESULT  ** ppErrors );

  STDMETHODIMP Refresh2(
    OPCDATASOURCE   dwSource,
    DWORD           TransactionID,
    DWORD         * pCancelID );

  STDMETHODIMP Cancel2( DWORD dwCancelID );

  STDMETHODIMP SetEnable( BOOL bEnable );

  STDMETHODIMP GetEnable( BOOL *pbEnable );

  // IOPCAsyncIO
  STDMETHODIMP Read(
    DWORD           dwConnection,
    OPCDATASOURCE   dwSource,
    DWORD           dwNumItems,
    OPCHANDLE     * phServer,
    DWORD         * pTransactionID,
    HRESULT      ** ppErrors );

  STDMETHODIMP Write(
    DWORD       dwConnection,
    DWORD       dwNumItems,
    OPCHANDLE * phServer,
    VARIANT   * pItemValues,
    DWORD     * pTransactionID,
    HRESULT ** ppErrors );

  STDMETHODIMP Refresh(
    DWORD           dwConnection,
    OPCDATASOURCE   dwSource,
    DWORD         * pTransactionID );

  STDMETHODIMP Cancel( DWORD dwTransactionID );

  // IDataObject
  STDMETHOD( DAdvise )(
        FORMATETC* pformatetc,
        DWORD advf,
        IAdviseSink* pAdvSink,
        DWORD* pdwConnection);
  STDMETHOD( DUnadvise )( DWORD dwConnection);
  STDMETHOD( EnumDAdvise )( IEnumSTATDATA** ppenumAdvise);
  STDMETHOD( GetData )( FORMATETC* pformatetcIn, STGMEDIUM* pmedium);
  STDMETHOD( QueryGetData )( FORMATETC* pformatetc);

  STDMETHOD( GetDataHere )( FORMATETC* pformatetc, STGMEDIUM* pmedium)
                          { return E_NOTIMPL; }
  STDMETHOD( GetCanonicalFormatEtc )( FORMATETC* pformatectIn, FORMATETC* pformatetcOut)
                          { return E_NOTIMPL; }
  STDMETHOD( SetData )( FORMATETC* pformatetc, STGMEDIUM* pmedium, BOOL fRelease)
                          { return E_NOTIMPL; }
  STDMETHOD( EnumFormatEtc )( DWORD dwDirection, IEnumFORMATETC** ppenumFormatEtc)
                          { return E_NOTIMPL; }
  // helper functions
  STDMETHOD( CreateDataStream )(STGMEDIUM* pmedium, DWORD transactionID);
  STDMETHOD( CreateDataTimeStream )(STGMEDIUM* pmedium, DWORD transactionID);


  STDMETHOD(UpdateClients)();

   // Non COM interface
   void      Remove() { m_removed = TRUE; }
   BOOL      CompareName( LPCWSTR pName ) { return m_name == pName; }
   OPCHANDLE GetClientHandle() { return m_clientHandle; }
   DWORD     GetUpdateRate() {return m_updateRate;}  // gets actual rate

   // Overrides
   virtual void DoSetUpdateRate( DWORD newUpdateRate );
   virtual void DoSetState( BOOL activeChanged );
   virtual BOOL IsPublic() {return FALSE;}
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
   virtual BOOL     IsItemValid( const COPCItem* pItem );
   virtual void		ClearRemovedItems();
   virtual IUnknown* DoCreateEnumerator();
   virtual HRESULT DoCloneGroupItems(OPCGroupBase* pCloneGroup);

//protected:
   // IDataObject and data collection
   IAdviseSink*        m_DataAdviseSink;
   IAdviseSink*        m_DataTimeAdviseSink;
   IAdviseSink*        m_AsyncAdviseSink;
   static DWORD        ThreadStub(void*);
   DWORD               DataThread();
   HANDLE              m_hDataThread;
   BOOL                m_running;
   BOOL                m_dataWaiting;
   CRITICAL_SECTION    m_cs;
   HANDLE              m_hTimer;
   BOOL                m_cmdWaiting;

   // ConnectionPoint
   BOOL              m_bEnable;

   // General attributes
   OPCServerBase*    m_parent;
   CString           m_name;
   BOOL              m_active;
   DWORD             m_updateRate;    // in mSec
   OPCHANDLE         m_clientHandle;
   LONG              m_timeBias;
   FLOAT             m_deadBand;
   DWORD             m_LCID;
   BOOL              m_removed;       // waiting for final release
   ItemMap           m_itemMap;       // map of client handles -> items
   DWORD             m_transactionID; // incremented to give next ID

   // ASyncIO related
   int               m_ASYNC_REQUEST_LIMIT;     // Max number of async requests
   HANDLE            m_hASyncThread;
   static DWORD      ASyncThreadStub(void*);
   DWORD             ASyncThread();
   HANDLE            m_hAsyncRequest; // Event to notify the async thread
   STDMETHOD( AsyncUpdate )(ASyncRequest* pRequest);
   STDMETHOD( AsyncUpdate2 )(ASyncRequest* pRequest);
   ASyncRequestList  m_asyncRequests;
   CRITICAL_SECTION    m_IOcs;        // This protects DoUpdateGroup from multiple threads
};

#endif