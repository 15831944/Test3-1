
// ScdSpecie.h : Declaration of the CScdSpecieDefn

#ifndef __SCDSPECIE_H_
#define __SCDSPECIE_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CScdSpecieDefn
class ATL_NO_VTABLE CScdSpecieDefn : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CScdSpecieDefn, &CLSID_ScdSpecieDefn>,
	public ISupportErrorInfo,
	public IConnectionPointContainerImpl<CScdSpecieDefn>,
	public IDispatchImpl<IScdSpecieDefn, &IID_IScdSpecieDefn, &LIBID_ScdMdl>
	{
	public:
		CScdSpecieDefn()
			{
			m_pUnkMarshaler = NULL;
			}

		DECLARE_REGISTRY_RESOURCEID(IDR_SCDSPECIEDEFN)
		DECLARE_GET_CONTROLLING_UNKNOWN()

		DECLARE_PROTECT_FINAL_CONSTRUCT()

		BEGIN_COM_MAP(CScdSpecieDefn)
			COM_INTERFACE_ENTRY(IScdSpecieDefn)
			//DEL 	COM_INTERFACE_ENTRY(IDispatch)
			COM_INTERFACE_ENTRY(ISupportErrorInfo)
			COM_INTERFACE_ENTRY(IConnectionPointContainer)
			COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, m_pUnkMarshaler.p)
			COM_INTERFACE_ENTRY2(IDispatch, IScdSpecieDefn)
		END_COM_MAP()
		BEGIN_CONNECTION_POINT_MAP(CScdSpecieDefn)
		END_CONNECTION_POINT_MAP()

		DECLARE_SCD(long);

		HRESULT FinalConstruct()
			{
			m_lIndex=-1;
			m_lFidelity=0;
			return CoCreateFreeThreadedMarshaler(
				GetControllingUnknown(), &m_pUnkMarshaler.p);
			}

		void FinalRelease()
			{
			m_pUnkMarshaler.Release();
			}

		CComPtr<IUnknown> m_pUnkMarshaler;
		long m_lIndex;
		long m_lFidelity;

		// ISupportsErrorInfo
		STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

		// IScdSpecie
	public:
		STDMETHOD(get_Fidelity)(/*[out, retval]*/ long  *pVal);
		STDMETHOD(put_Fidelity)(/*[in]*/ long  newVal);
		STDMETHOD(get_Index)(/*[out, retval]*/ long *pVal);
		STDMETHOD(put_Index)(/*[in]*/ long newVal);
		STDMETHOD(get_Symbol)(/*[out, retval]*/ BSTR *pVal);
		STDMETHOD(get_Tag)(/*[out, retval]*/ BSTR *pVal);
		//STDMETHOD(get_Value)(LONG Index, DOUBLE TdK, DOUBLE PkPa, VARIANT * pVal);
		STDMETHOD(get_MolecularWt)(DOUBLE * pVal);
		STDMETHOD(get_Density)(DOUBLE TdK, DOUBLE PkPa, DOUBLE * pVal);
		STDMETHOD(get_msEnthalpy)(DOUBLE TdK, DOUBLE PkPa, DOUBLE * pVal);
		STDMETHOD(get_msEntropy)(DOUBLE TdK, DOUBLE PkPa, DOUBLE * pVal);
		STDMETHOD(get_msCp)(DOUBLE TdK, DOUBLE PkPa, DOUBLE * pVal);
		STDMETHOD(get_DynamicViscosity)(DOUBLE TdK, DOUBLE PkPa, DOUBLE * pVal);
		STDMETHOD(get_ThermalConductivity)(DOUBLE TdK, DOUBLE PkPa, DOUBLE * pVal);
		STDMETHOD(get_VapourP)(DOUBLE TdK, DOUBLE * pVal);
		STDMETHOD(get_VapourT)(DOUBLE PkPa, DOUBLE * pVal);
    STDMETHOD(get_Properties)(IScdProperties ** pVal);
  };

#endif //__SCDSPECIE_H_
