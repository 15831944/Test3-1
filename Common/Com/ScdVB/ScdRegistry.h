// ScdRegistry.h : Declaration of the CScdRegistry

#ifndef __SCDREGISTRY_H_
#define __SCDREGISTRY_H_

#include "resource.h"       // main symbols
#include "ScdVBCP.h"

/////////////////////////////////////////////////////////////////////////////
// CScdRegistry
class ATL_NO_VTABLE CScdRegistry : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CScdRegistry, &CLSID_ScdRegistry>,
	public ISupportErrorInfoImpl<&IID_IScdRegistry>, 
	public IConnectionPointContainerImpl<CScdRegistry>,
	public IDispatchImpl<IScdRegistry, &IID_IScdRegistry, &LIBID_ScdVBLib>,
	public CProxy_IScdRegistryEvents< CScdRegistry >
{
public:
	CScdRegistry()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_SCDREGISTRY)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CScdRegistry)
	COM_INTERFACE_ENTRY(IScdRegistry)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
	COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
END_COM_MAP()
BEGIN_CONNECTION_POINT_MAP(CScdRegistry)
CONNECTION_POINT_ENTRY(DIID__ScdRegistryEvents)
END_CONNECTION_POINT_MAP()


// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IScdRegistry
public:
	STDMETHOD(PutKey)(/*[in]*/ ScdKeyTypes Type, /*[in]*/ ScdHKEY HKEYRoot, /*[in]*/ BSTR KeyPath, /*[in]*/ BSTR KeyName, /*[in]*/ VARIANT Value);
	STDMETHOD(GetKey)(/*[in]*/ ScdKeyTypes Type, /*[in]*/ ScdHKEY HKEYRoot, /*[in]*/ BSTR KeyPath, /*[in]*/ BSTR KeyName, /*[in,out]*/ VARIANT * pValue);
};

#endif //__SCDREGISTRY_H_
