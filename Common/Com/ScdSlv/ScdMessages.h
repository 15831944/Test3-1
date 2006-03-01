
// ScdMessages.h : Declaration of the CScdMessages

#ifndef __SCDMESSAGES_H_
#define __SCDMESSAGES_H_

#include "resource.h"       // main symbols

#include "ScdSlv.h"

/////////////////////////////////////////////////////////////////////////////
// CScdMessages
class ATL_NO_VTABLE CScdMessages :
  public CComObjectRootEx<CComMultiThreadModel>,
  public CComCoClass<CScdMessages, &CLSID_ScdMessages>,
  public ISupportErrorInfo,
  //public IConnectionPointContainerImpl<CScdMessages>,
  public IDispatchImpl<IScdMessages, &IID_IScdMessages, &LIBID_ScdSlv>
  {
  public:
    CScdMessages()
      {
      //m_pUnkMarshaler = NULL;
      }

    DECLARE_REGISTRY_RESOURCEID(IDR_SCDMESSAGES)
    DECLARE_GET_CONTROLLING_UNKNOWN()

    DECLARE_PROTECT_FINAL_CONSTRUCT()

    BEGIN_COM_MAP(CScdMessages)
      COM_INTERFACE_ENTRY(IScdMessages)
      COM_INTERFACE_ENTRY(IDispatch)
      COM_INTERFACE_ENTRY(ISupportErrorInfo)
      //COM_INTERFACE_ENTRY(IConnectionPointContainer)
    END_COM_MAP()
    //BEGIN_CONNECTION_POINT_MAP(CScdMessages)
    //END_CONNECTION_POINT_MAP()

    DECLARE_SCD(long);

    HRESULT FinalConstruct();
    void FinalRelease();

    //CComPtr<IUnknown> m_pUnkMarshaler;
    long              m_iViewLimit;
    bool              m_bSnapshotReqd;
    eScdMsg_Flags     m_iSelection;

    CArray<IScdMessage*, IScdMessage*> m_Msgs;

    void BuildSnapshot();

    // ISupportsErrorInfo
    STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

  public:
    STDMETHOD(Flush)(/*[in]*/ long NoToKeep);
    STDMETHOD(LogFileClose)();
    STDMETHOD(LogFileOpen)(/*[in]*/ BSTR FileName);
    // IScdMessages
    STDMETHOD(Item)(long Index, IScdMessage  ** pItem);
    STDMETHOD(get_Count)(LONG * pVal);
    STDMETHOD(get_ViewLimit)(LONG * pVal);
    STDMETHOD(put_ViewLimit)(LONG pVal);
    STDMETHOD(get__NewEnum)(IUnknown * * pVal);
    STDMETHOD(get_Options)(/*[out, retval]*/ eScdMsg_Options *pVal);
    STDMETHOD(put_Options)(/*[in]*/ eScdMsg_Options newVal);
    STDMETHOD(Refresh)();
    STDMETHOD(Select)(/*[in]*/ eScdMsg_Flags Selection);

  };

#endif //__SCDMESSAGES_H_
