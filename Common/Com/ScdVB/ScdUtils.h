// ScdUtils.h : Declaration of the CScdUtils

#pragma once
#include "resource.h"       // main symbols

#include "ScdVB.h"


// CScdUtils

class ATL_NO_VTABLE CScdUtils : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CScdUtils, &CLSID_ScdUtils>,
	public IDispatchImpl<IScdUtils, &IID_IScdUtils, &LIBID_ScdVBLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CScdUtils()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_SCDUTILS)


BEGIN_COM_MAP(CScdUtils)
	COM_INTERFACE_ENTRY(IScdUtils)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	
	void FinalRelease() 
	{
	}

public:

  STDMETHOD(Sleep)(LONG PeriodMS);
};

OBJECT_ENTRY_AUTO(__uuidof(ScdUtils), CScdUtils)
