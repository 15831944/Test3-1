// ScdProperty.h : Declaration of the CScdProperty

#pragma once
#include "resource.h"       // main symbols

#include "ScdMdl.h"
#include "propertybase.h" 


// CScdProperty

class ATL_NO_VTABLE CScdProperty : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CScdProperty, &CLSID_ScdProperty>,
	public ISupportErrorInfo,
	public IDispatchImpl<IScdProperty, &IID_IScdProperty, &LIBID_ScdMdl, /*wMajor =*/ 1, /*wMinor =*/ 0>
	{
	public:
		CScdProperty()
			{
			}

		DECLARE_REGISTRY_RESOURCEID(IDR_SCDPROPERTY)

		BEGIN_COM_MAP(CScdProperty)
			COM_INTERFACE_ENTRY(IScdProperty)
			COM_INTERFACE_ENTRY(IDispatch)
			COM_INTERFACE_ENTRY(ISupportErrorInfo)
		END_COM_MAP()

		DECLARE_SCD(long);

		// ISupportsErrorInfo
		STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

		DECLARE_PROTECT_FINAL_CONSTRUCT()

		HRESULT FinalConstruct();
		void FinalRelease(); 

	public:
		STDMETHOD(SetObjectInfo)(/*[in]*/ ULONG What, /*[in]*/ ULONG Mdl, /*[in]*/ ULONG PropNo);
		STDMETHOD(get_Value)(/*[in,defaultvalue(-1)]*/ long Phases, /*[in,defaultvalue(-1)]*/ double TdK, /*[in,defaultvalue(-1)]*/ double PkPa, /*[out, retval]*/ VARIANT * pValue);
		STDMETHOD(SetValue)(/*[in]*/VARIANT * pValue);
		STDMETHOD(get_Tag)(/*[out, retval]*/ BSTR * pValue);
		STDMETHOD(get_Symbol)(/*[out, retval]*/ BSTR * pValue);
		STDMETHOD(get_Description)(/*[out, retval]*/ BSTR * pValue);
		STDMETHOD(get_Cnv)(IScdConversion ** pVal);
    STDMETHOD(get_Index)(LONG* pVal);

    ULONG           m_iWhat;     
    CPropertyBase * m_pProps;
    long            m_iPropNo;

  };

OBJECT_ENTRY_AUTO(__uuidof(ScdProperty), CScdProperty)
