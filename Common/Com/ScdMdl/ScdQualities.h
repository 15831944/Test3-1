// ScdQualities.h : Declaration of the CScdQualities

#pragma once
#include "resource.h"       // main symbols

#include "ScdMdl.h"


// CScdQualities

class ATL_NO_VTABLE CScdQualities : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CScdQualities, &CLSID_ScdQualities>,
	public ISupportErrorInfo,
	public IDispatchImpl<IScdQualities, &IID_IScdQualities, &LIBID_ScdMdl, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CScdQualities()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_SCDQUALITIES)


BEGIN_COM_MAP(CScdQualities)
	COM_INTERFACE_ENTRY(IScdQualities)
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

OBJECT_ENTRY_AUTO(__uuidof(ScdQualities), CScdQualities)
