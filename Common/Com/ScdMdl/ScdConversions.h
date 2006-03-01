// ScdConversions.h : Declaration of the CScdConversions

#pragma once
#include "resource.h"       // main symbols

#include "ScdMdl.h"


// CScdConversions

class ATL_NO_VTABLE CScdConversions : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CScdConversions, &CLSID_ScdConversions>,
	public ISupportErrorInfo,
	public IDispatchImpl<IScdConversions, &IID_IScdConversions, &LIBID_ScdMdl, /*wMajor =*/ 1, /*wMinor =*/ 0>
	{
	public:
		CScdConversions()
			{
			}

		DECLARE_REGISTRY_RESOURCEID(IDR_SCDCONVERSIONS)


		BEGIN_COM_MAP(CScdConversions)
			COM_INTERFACE_ENTRY(IScdConversions)
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
		STDMETHOD(get_Count)(LONG* pVal);
		STDMETHOD(get__NewEnum)(IUnknown** pVal);
		STDMETHOD(Item)(VARIANT WhichOne, IScdConversion ** pItem);

		CArray <LPDISPATCH, LPDISPATCH> m_Spcs;
	};

OBJECT_ENTRY_AUTO(__uuidof(ScdConversions), CScdConversions)
