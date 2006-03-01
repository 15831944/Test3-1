// ScdDDEServer.h : Declaration of the CScdDDEServer

#pragma once
#include "resource.h"       // main symbols

#include "SysCAD.h"
#include "_IScdDDEServerEvents_CP.h"


// CScdDDEServer

class ATL_NO_VTABLE CScdDDEServer : 
  public CScdCOCmdBase,
  public CComObjectRootEx<CComMultiThreadModel>,
  public CComCoClass<CScdDDEServer, &CLSID_ScdDDEServer>,
  public ISupportErrorInfo,
  public IConnectionPointContainerImpl<CScdDDEServer>,
  public CProxy_IScdDDEServerEvents<CScdDDEServer>, 
  public IDispatchImpl<IScdDDEServer, &IID_IScdDDEServer, &LIBID_ScdApp, /*wMajor =*/ 1, /*wMinor =*/ 0>
  {
  public:
    CScdDDEServer() : CScdCOCmdBase(WMU_COM_APP)
      {
      }

    DECLARE_REGISTRY_RESOURCEID(IDR_SCDDDESERVER)
    DECLARE_GET_CONTROLLING_UNKNOWN()

    DECLARE_NOT_AGGREGATABLE(CScdDDEServer)

    BEGIN_COM_MAP(CScdDDEServer)
      COM_INTERFACE_ENTRY(IScdDDEServer)
      COM_INTERFACE_ENTRY2(IDispatch, IScdDDEServer)
      COM_INTERFACE_ENTRY(ISupportErrorInfo)
      COM_INTERFACE_ENTRY(IConnectionPointContainer)
    END_COM_MAP()

    BEGIN_CONNECTION_POINT_MAP(CScdDDEServer)
      CONNECTION_POINT_ENTRY(__uuidof(_IScdDDEServerEvents))
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

//OBJECT_ENTRY_AUTO(__uuidof(ScdDDEServer), CScdDDEServer)
