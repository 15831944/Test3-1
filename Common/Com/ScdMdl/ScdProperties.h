// ScdProperties.h : Declaration of the CScdProperties

#pragma once
#include "resource.h"       // main symbols

#include "ScdMdl.h"
#include "propertybase.h" 


// CScdProperties

class ATL_NO_VTABLE CScdProperties : 
  public CComObjectRootEx<CComMultiThreadModel>,
  public CComCoClass<CScdProperties, &CLSID_ScdProperties>,
  public ISupportErrorInfo,
  public IDispatchImpl<IScdProperties, &IID_IScdProperties, &LIBID_ScdMdl, /*wMajor =*/ 1, /*wMinor =*/ 0>
  {
  public:
    CScdProperties()
      {
      }

    DECLARE_REGISTRY_RESOURCEID(IDR_SCDPROPERTIES)


    BEGIN_COM_MAP(CScdProperties)
      COM_INTERFACE_ENTRY(IScdProperties)
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
    // IScdProperties
    STDMETHOD(Property)(VARIANT WhichOne, IScdProperty** pItem);
    STDMETHOD(get_Count)(LONG * pVal);
    STDMETHOD(get__NewEnum)(IUnknown * * pVal);

    STDMETHOD(SetObjectInfo)(ULONG What, ULONG Which);

    ULONG           m_iWhat;     
    CPropertyBase * m_pProps;
  };

OBJECT_ENTRY_AUTO(__uuidof(ScdProperties), CScdProperties)
