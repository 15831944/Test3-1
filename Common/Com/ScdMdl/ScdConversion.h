// ScdConversion.h : Declaration of the CScdConversion

#pragma once
#include "resource.h"       // main symbols

#include "ScdMdl.h"


// CScdConversion

class ATL_NO_VTABLE CScdConversion : 
  public CComObjectRootEx<CComMultiThreadModel>,
  public CComCoClass<CScdConversion, &CLSID_ScdConversion>,
  public ISupportErrorInfo,
  public IDispatchImpl<IScdConversion, &IID_IScdConversion, &LIBID_ScdMdl, /*wMajor =*/ 1, /*wMinor =*/ 0>
  {
  public:
    CScdConversion()
      {
      m_iCnv=-1;
      m_iIndex=-1;
      }

    DECLARE_REGISTRY_RESOURCEID(IDR_SCDCONVERSION)


    BEGIN_COM_MAP(CScdConversion)
      COM_INTERFACE_ENTRY(IScdConversion)
      COM_INTERFACE_ENTRY(IDispatch)
      COM_INTERFACE_ENTRY(ISupportErrorInfo)
    END_COM_MAP()

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

    STDMETHOD(get_Family)(BSTR* pVal);
    STDMETHOD(get_Name)(BSTR* pVal);
    STDMETHOD(get_Description)(BSTR* pVal);
    STDMETHOD(get_Scale)(DOUBLE* pVal);
    STDMETHOD(get_Offset)(DOUBLE* pVal);
    STDMETHOD(get_IsSI)(VARIANT_BOOL* pVal);
    STDMETHOD(get_Count)(LONG* pVal);
    STDMETHOD(get__NewEnum)(IUnknown** pVal);
    STDMETHOD(Item)(VARIANT WhichOne, IScdConversion ** pItem);
    STDMETHOD(get_Index)(LONG* pVal);
    STDMETHOD(SetObjectInfo)(LONG iCnv, LONG iIndex);

    long m_iCnv;
    long m_iIndex;
    STDMETHOD(get_Formula)(BSTR* pVal);
  };

OBJECT_ENTRY_AUTO(__uuidof(ScdConversion), CScdConversion)
