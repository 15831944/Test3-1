// ScdComplex.h : Declaration of the CScdComplex

#pragma once
#include "resource.h"       // main symbols

#include "ScdSlv.h"


// CScdComplex

class ATL_NO_VTABLE CScdComplex : 
  public CComObjectRootEx<CComMultiThreadModel>,
  public CComCoClass<CScdComplex, &CLSID_ScdComplex>,
  public ISupportErrorInfo,
  public IDispatchImpl<IScdComplex, &IID_IScdComplex, &LIBID_ScdSlv, /*wMajor =*/ 1, /*wMinor =*/ 0>
  {
  public:
    CScdComplex()
      {
      }

    DECLARE_REGISTRY_RESOURCEID(IDR_SCDCOMPLEX)

    DECLARE_NOT_AGGREGATABLE(CScdComplex)

    BEGIN_COM_MAP(CScdComplex)
      COM_INTERFACE_ENTRY(IScdComplex)
      COM_INTERFACE_ENTRY(IDispatch)
      COM_INTERFACE_ENTRY(ISupportErrorInfo)
    END_COM_MAP()

    // ISupportsErrorInfo
    STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

    DECLARE_PROTECT_FINAL_CONSTRUCT()
    DECLARE_SCD(long)

    HRESULT FinalConstruct()
      {
      m_Real=0;
      m_Imag=0;
      return S_OK;
      }

    void FinalRelease() 
      {
      }

    double m_Real;
    double m_Imag;

  public:

    STDMETHOD(get_Real)(DOUBLE* pVal);
    STDMETHOD(put_Real)(DOUBLE newVal);
    STDMETHOD(get_Imag)(DOUBLE* pVal);
    STDMETHOD(put_Imag)(DOUBLE newVal);
    STDMETHOD(get_Magnitude)(DOUBLE* pVal);
    STDMETHOD(put_Magnitude)(DOUBLE newVal);
    STDMETHOD(get_Phase)(DOUBLE* pVal);
    STDMETHOD(put_Phase)(DOUBLE newVal);
  };

//OBJECT_ENTRY_AUTO(__uuidof(ScdComplex), CScdComplex)
