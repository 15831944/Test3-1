// ScdWire.h : Declaration of the CScdWire

#pragma once
#include "resource.h"       // main symbols

#include "ScdSlv.h"


// CScdWire

class ATL_NO_VTABLE CScdWire : 
  public CComObjectRootEx<CComMultiThreadModel>,
  public CComCoClass<CScdWire, &CLSID_ScdWire>,
  public ISupportErrorInfo,
  public IDispatchImpl<IScdWire, &IID_IScdWire, &LIBID_ScdSlv, /*wMajor =*/ 1, /*wMinor =*/ 0>
  {
  public:
    CScdWire() 
      {
      }

    DECLARE_REGISTRY_RESOURCEID(IDR_SCDWIRE)

    DECLARE_NOT_AGGREGATABLE(CScdWire)

    BEGIN_COM_MAP(CScdWire)
      COM_INTERFACE_ENTRY(IScdWire)
      COM_INTERFACE_ENTRY(IDispatch)
      COM_INTERFACE_ENTRY(ISupportErrorInfo)
    END_COM_MAP()

    // ISupportsErrorInfo
    STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

    DECLARE_PROTECT_FINAL_CONSTRUCT()
    DECLARE_SCD(long);

    HRESULT FinalConstruct()
      {
      m_lTopoCount=-1;
      m_lElectricsTopoCount=-1;

      m_pNode=NULL;
      m_pWire=NULL;
      return S_OK;
      }

    void FinalRelease() 
      {
      }

    long          m_lTopoCount;
    long          m_lElectricsTopoCount;

    FlwNode     * m_pNode;
    CEWire      * m_pWire;

    void          BuildData();


  public:
    STDMETHOD(SetObjectInfo)(DWORD  Node, DWORD  Wire);
    STDMETHOD(get_Tag)(BSTR* pVal);

    STDMETHOD(get_Node)(IScdNode** pVal);
    STDMETHOD(get_SrcNode)(IScdNode** pVal);
    STDMETHOD(get_DstNode)(IScdNode** pVal);
    STDMETHOD(get_SrcTermStrip)(IScdTermStrip** pVal);
    STDMETHOD(get_DstTermStrip)(IScdTermStrip** pVal);
    STDMETHOD(get_SrcTerminal)(IScdTerminal** pVal);
    STDMETHOD(get_DstTerminal)(IScdTerminal** pVal);
    STDMETHOD(get_Area)(DOUBLE* pVal);
    STDMETHOD(put_Area)(DOUBLE newVal);
    STDMETHOD(get_Length)(DOUBLE* pVal);
    STDMETHOD(put_Length)(DOUBLE newVal);
    STDMETHOD(get_Component)(IScdComponent** pVal);
  };

//OBJECT_ENTRY_AUTO(__uuidof(ScdWire), CScdWire)
