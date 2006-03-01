// ScdIOMarshal.h : Declaration of the CScdIOMarshal

#pragma once
#include "resource.h"       // main symbols

#include "SysCAD.h"
#include "_IScdIOMarshalEvents_CP.h"


// CScdIOMarshal

class ATL_NO_VTABLE CScdIOMarshal : 
  public CScdCOCmdBase,
  public CComObjectRootEx<CComMultiThreadModel>,
  public CComCoClass<CScdIOMarshal, &CLSID_ScdIOMarshal>,
  public ISupportErrorInfo,
  public IConnectionPointContainerImpl<CScdIOMarshal>,
  public CProxy_IScdIOMarshalEvents<CScdIOMarshal>, 
  public IDispatchImpl<IScdIOMarshal, &IID_IScdIOMarshal, &LIBID_ScdApp, /*wMajor =*/ 1, /*wMinor =*/ 0>
  {
  public:
    CScdIOMarshal() : CScdCOCmdBase(WMU_COM_APP)
      {
      }

    DECLARE_REGISTRY_RESOURCEID(IDR_SCDIOMARSHAL)
    DECLARE_GET_CONTROLLING_UNKNOWN()

    DECLARE_NOT_AGGREGATABLE(CScdIOMarshal)

    BEGIN_COM_MAP(CScdIOMarshal)
      COM_INTERFACE_ENTRY(IScdIOMarshal)
      COM_INTERFACE_ENTRY2(IDispatch, IScdIOMarshal)
      COM_INTERFACE_ENTRY(ISupportErrorInfo)
      COM_INTERFACE_ENTRY(IConnectionPointContainer)
    END_COM_MAP()

    BEGIN_CONNECTION_POINT_MAP(CScdIOMarshal)
      CONNECTION_POINT_ENTRY(__uuidof(_IScdIOMarshalEvents))
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

//OBJECT_ENTRY_AUTO(__uuidof(ScdIOMarshal), CScdIOMarshal)
