// ScdTagFixup.h : Declaration of the CScdTagFixup

#pragma once
#include "resource.h"       // main symbols

#include "SysCAD.h"


// CScdTagFixup

class ATL_NO_VTABLE CScdTagFixup : 
  public CComObjectRootEx<CComMultiThreadModel>,
  public CComCoClass<CScdTagFixup, &CLSID_ScdTagFixup>,
  public ISupportErrorInfo,
  public IDispatchImpl<IScdTagFixup, &IID_IScdTagFixup, &LIBID_ScdApp, /*wMajor =*/ 1, /*wMinor =*/ 0>
  {
  public:
    CScdTagFixup()
      {
      m_iRule=eFixup_NoChange;
      }

    DECLARE_REGISTRY_RESOURCEID(IDR_SCDTAGFIXUP)

    DECLARE_NOT_AGGREGATABLE(CScdTagFixup)

    BEGIN_COM_MAP(CScdTagFixup)
      COM_INTERFACE_ENTRY(IScdTagFixup)
      COM_INTERFACE_ENTRY(IDispatch)
      COM_INTERFACE_ENTRY(ISupportErrorInfo)
    END_COM_MAP()

    // ISupportsErrorInfo
    STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

    DECLARE_PROTECT_FINAL_CONSTRUCT()

    DECLARE_SCD(long);

    HRESULT FinalConstruct()
      {
      return S_OK;
      }

    void FinalRelease() 
      {
      }

  public:

    STDMETHOD(get_Rule)(eScdTagFixupRules* pVal);
    STDMETHOD(put_Rule)(eScdTagFixupRules newVal);
    STDMETHOD(get_String1)(BSTR* pVal);
    STDMETHOD(put_String1)(BSTR newVal);
    STDMETHOD(get_String2)(BSTR* pVal);
    STDMETHOD(put_String2)(BSTR newVal);

    eScdTagFixupRules   m_iRule;
    CString             m_sString1;
    CString             m_sString2;
    //... Tag Lists etc.
  };

OBJECT_ENTRY_AUTO(__uuidof(ScdTagFixup), CScdTagFixup)
