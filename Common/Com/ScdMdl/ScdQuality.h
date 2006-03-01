// ScdQuality.h : Declaration of the CScdQuality

#pragma once
#include "resource.h"       // main symbols

#include "ScdMdl.h"


// CScdQuality

class ATL_NO_VTABLE CScdQuality : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CScdQuality, &CLSID_ScdQuality>,
	public ISupportErrorInfo,
	public IDispatchImpl<IScdQuality, &IID_IScdQuality, &LIBID_ScdMdl, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CScdQuality()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_SCDQUALITY)


BEGIN_COM_MAP(CScdQuality)
	COM_INTERFACE_ENTRY(IScdQuality)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

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

};

OBJECT_ENTRY_AUTO(__uuidof(ScdQuality), CScdQuality)
