// ScdProfile.h : Declaration of the CScdProfile

#pragma once
#include "resource.h"       // main symbols

#include "ScdVB.h"


// CScdProfile

class ATL_NO_VTABLE CScdProfile : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CScdProfile, &CLSID_ScdProfile>,
	public ISupportErrorInfo,
	public IDispatchImpl<IScdProfile, &IID_IScdProfile, &LIBID_ScdVBLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CScdProfile()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_SCDPROFILE)


BEGIN_COM_MAP(CScdProfile)
	COM_INTERFACE_ENTRY(IScdProfile)
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

  STDMETHOD(ReadString)(BSTR File, BSTR Section, BSTR Entry, BSTR Default, BSTR* pVal);
  STDMETHOD(WriteString)(BSTR File, BSTR Section, BSTR Entry, BSTR newVal);
  STDMETHOD(ReadLong)(BSTR File, BSTR Section, BSTR Entry, long Default, long * pVal);
  STDMETHOD(WriteLong)(BSTR File, BSTR Section, BSTR Entry, long newVal);
  STDMETHOD(ReadDouble)(BSTR File, BSTR Section, BSTR Entry, double Default, double * pVal);
  STDMETHOD(WriteDouble)(BSTR File, BSTR Section, BSTR Entry, double newVal);
};

OBJECT_ENTRY_AUTO(__uuidof(ScdProfile), CScdProfile)
