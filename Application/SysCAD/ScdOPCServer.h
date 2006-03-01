// ScdOPCServer.h : Declaration of the CScdOPCServer

#pragma once
#include "resource.h"       // main symbols

#include "SysCAD.h"
#include "_IScdOPCServerEvents_CP.h"


// CScdOPCServer

class ATL_NO_VTABLE CScdOPCServer : 
  public CScdCOCmdBase,
  public CComObjectRootEx<CComMultiThreadModel>,
  public CComCoClass<CScdOPCServer, &CLSID_ScdOPCServer>,
  public ISupportErrorInfo,
  public IConnectionPointContainerImpl<CScdOPCServer>,
  public CProxy_IScdOPCServerEvents<CScdOPCServer>, 
  public IDispatchImpl<IScdOPCServer, &IID_IScdOPCServer, &LIBID_ScdApp, /*wMajor =*/ 1, /*wMinor =*/ 0>
  {
  public:
    CScdOPCServer() : CScdCOCmdBase(WMU_COM_APP)
      {
      }

    DECLARE_REGISTRY_RESOURCEID(IDR_SCDOPCSERVER)
    DECLARE_GET_CONTROLLING_UNKNOWN()

    DECLARE_NOT_AGGREGATABLE(CScdOPCServer)

    BEGIN_COM_MAP(CScdOPCServer)
      COM_INTERFACE_ENTRY(IScdOPCServer)
      COM_INTERFACE_ENTRY(IDispatch)
      COM_INTERFACE_ENTRY(ISupportErrorInfo)
      COM_INTERFACE_ENTRY(IConnectionPointContainer)
    END_COM_MAP()

    BEGIN_CONNECTION_POINT_MAP(CScdOPCServer)
      CONNECTION_POINT_ENTRY(__uuidof(_IScdOPCServerEvents))
    END_CONNECTION_POINT_MAP()

    DECLARE_SCD(long);

    // ISupportsErrorInfo
    STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

    DECLARE_PROTECT_FINAL_CONSTRUCT()

    HRESULT FinalConstruct()
      {
      return S_OK;
      }

    void FinalRelease() 
      {
      }

  public:
    // IScdASyncEvents
    STDMETHOD(DoEventMsg)(LONG Evt, LONG Data)
      {
      CScdCOCmdBase::DoEventMsg(Evt, Data);
      return S_OK;
      }
    virtual void FireTheEvent(long Evt, long Data);

    STDMETHOD(get_On)(VARIANT_BOOL* pVal);
    STDMETHOD(put_On)(VARIANT_BOOL newVal);
  };

//OBJECT_ENTRY_AUTO(__uuidof(ScdOPCServer), CScdOPCServer)
