// ScdComponent.h : Declaration of the CScdComponent

#pragma once
#include "resource.h"       // main symbols

#include "ScdSlv.h"
#include "flwnode.h"


// CScdComponent

class ATL_NO_VTABLE CScdComponent : 
  public CComObjectRootEx<CComMultiThreadModel>,
  public CComCoClass<CScdComponent, &CLSID_ScdComponent>,
  public ISupportErrorInfo,
  public IDispatchImpl<IScdComponent, &IID_IScdComponent, &LIBID_ScdSlv, /*wMajor =*/ 1, /*wMinor =*/ 0>
  {
  public:
    CScdComponent()
      {
      }

    DECLARE_REGISTRY_RESOURCEID(IDR_SCDCOMPONENT)

    DECLARE_NOT_AGGREGATABLE(CScdComponent)

    BEGIN_COM_MAP(CScdComponent)
      COM_INTERFACE_ENTRY(IScdComponent)
      COM_INTERFACE_ENTRY(IDispatch)
      COM_INTERFACE_ENTRY(ISupportErrorInfo)
    END_COM_MAP()

    // ISupportsErrorInfo
    STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

    DECLARE_PROTECT_FINAL_CONSTRUCT()
    DECLARE_SCD(long)

    HRESULT FinalConstruct()
      {
      m_lTopoCount=-1;
      m_lElectricsTopoCount=-1;

      m_pNode=NULL;
      m_pComponent=NULL;
      return S_OK;
      }

    void FinalRelease() 
      {
      }

    long          m_lTopoCount;
    long          m_lElectricsTopoCount;

    FlwNode     * m_pNode;
    CEComponent * m_pComponent;

    void          BuildData();

  public:
    STDMETHOD(SetObjectInfo)(DWORD  Node, DWORD Component);

    STDMETHOD(get_SrcTerminal)(IScdTerminal** pVal);
    STDMETHOD(get_DstTerminal)(IScdTerminal** pVal);
    STDMETHOD(get_Impedance)(IScdComplex** pVal);
    STDMETHOD(get_Source)(IScdComplex** pVal);
    STDMETHOD(get_Current)(IScdComplex** pVal);
    STDMETHOD(get_Applied)(IScdComplex** pVal);
  };

//OBJECT_ENTRY_AUTO(__uuidof(ScdComponent), CScdComponent)
