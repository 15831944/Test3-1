// ScdSpVectorDefn.h : Declaration of the CScdSpVectorDefn

#pragma once
#include "resource.h"       // main symbols

#include "ScdMdl.h"


// CScdSpVectorDefn

class ATL_NO_VTABLE CScdSpVectorDefn : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CScdSpVectorDefn, &CLSID_ScdSpVectorDefn>,
	public ISupportErrorInfo,
	public IDispatchImpl<IScdSpVectorDefn, &IID_IScdSpVectorDefn, &LIBID_ScdMdl, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CScdSpVectorDefn()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_SCDSPVECTORDEFN)


BEGIN_COM_MAP(CScdSpVectorDefn)
	COM_INTERFACE_ENTRY(IScdSpVectorDefn)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

	DECLARE_PROTECT_FINAL_CONSTRUCT()

  DECLARE_SCD(long)

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	
	void FinalRelease() 
	{
	}

public:

  STDMETHOD(get_Count)(LONG* pVal);
  STDMETHOD(get_Tag)(LONG Index, BSTR* pVal);
  STDMETHOD(get_Symbol)(LONG Index, BSTR* pVal);
  STDMETHOD(get_Cnv)(VARIANT Index, IScdConversion ** pVal);
};

OBJECT_ENTRY_AUTO(__uuidof(ScdSpVectorDefn), CScdSpVectorDefn)
