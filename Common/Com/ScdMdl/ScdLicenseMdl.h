
// ScdLicenseMdl.h : Declaration of the CScdLicenseMdl

#ifndef __SCDLICENSEMDL_H_
#define __SCDLICENSEMDL_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CScdLicenseMdl
class ATL_NO_VTABLE CScdLicenseMdl : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CScdLicenseMdl, &CLSID_ScdLicenseMdl>,
	public ISupportErrorInfo,
	public IConnectionPointContainerImpl<CScdLicenseMdl>,
	public IDispatchImpl<IScdLicenseMdl, &IID_IScdLicenseMdl, &LIBID_ScdMdl>
	{
	public:
		CScdLicenseMdl()
			{
			//m_pUnkMarshaler = NULL;
			}

		DECLARE_REGISTRY_RESOURCEID(IDR_SCDLICENSEMDL)
		DECLARE_GET_CONTROLLING_UNKNOWN()

		DECLARE_PROTECT_FINAL_CONSTRUCT()

		BEGIN_COM_MAP(CScdLicenseMdl)
			COM_INTERFACE_ENTRY(IScdLicenseMdl)
			COM_INTERFACE_ENTRY(IDispatch)
			COM_INTERFACE_ENTRY(ISupportErrorInfo)
			COM_INTERFACE_ENTRY(IConnectionPointContainer)
			//COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, m_pUnkMarshaler.p)
		END_COM_MAP()
		BEGIN_CONNECTION_POINT_MAP(CScdLicenseMdl)
		END_CONNECTION_POINT_MAP()

		DECLARE_SCD(long);

		HRESULT FinalConstruct()
			{
			return S_OK; //CoCreateFreeThreadedMarshaler(GetControllingUnknown(), &m_pUnkMarshaler.p);
			}

		void FinalRelease()
			{
			//m_pUnkMarshaler.Release();
			}

		//CComPtr<IUnknown> m_pUnkMarshaler;

		// ISupportsErrorInfo
		STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

		// IScdLicenseMdl
	public:
    STDMETHOD(Dummy)(void);
  };

#endif //__SCDLICENSEMDL_H_
