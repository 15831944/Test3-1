
// ScdSpecieDefns.h : Declaration of the CScdSpecieDefns

#ifndef __SCDSPECIES_H_
#define __SCDSPECIES_H_

#include "resource.h" 
//#include "vector"     

/////////////////////////////////////////////////////////////////////////////
// CScdSpecieDefns
class ATL_NO_VTABLE CScdSpecieDefns : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CScdSpecieDefns, &CLSID_ScdSpecieDefns>,
	public ISupportErrorInfo,
	public IConnectionPointContainerImpl<CScdSpecieDefns>,
	public IDispatchImpl<IScdSpecieDefns, &IID_IScdSpecieDefns, &LIBID_ScdMdl>
	{
	public:
		CScdSpecieDefns()
			{
			m_pUnkMarshaler = NULL;
			}

		DECLARE_REGISTRY_RESOURCEID(IDR_SCDSPECIEDEFNS)
		DECLARE_GET_CONTROLLING_UNKNOWN()

		DECLARE_PROTECT_FINAL_CONSTRUCT()

		BEGIN_COM_MAP(CScdSpecieDefns)
			COM_INTERFACE_ENTRY(IScdSpecieDefns)
			COM_INTERFACE_ENTRY(IDispatch)
			COM_INTERFACE_ENTRY(ISupportErrorInfo)
			COM_INTERFACE_ENTRY(IConnectionPointContainer)
			COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, m_pUnkMarshaler.p)
		END_COM_MAP()
		BEGIN_CONNECTION_POINT_MAP(CScdSpecieDefns)
		END_CONNECTION_POINT_MAP()

		DECLARE_SCD(long);

		HRESULT FinalConstruct()
			{
			// delay creation of m_Spcs until SpecieDB Inited
			return CoCreateFreeThreadedMarshaler(
				GetControllingUnknown(), &m_pUnkMarshaler.p);
			}

		void FinalRelease()
			{
			m_pUnkMarshaler.Release();
			for (long i=0; i<m_Spcs.GetSize(); i++)
				m_Spcs[i]->Release();
			}

		CComPtr<IUnknown> m_pUnkMarshaler;
		CArray <LPDISPATCH, LPDISPATCH> m_Spcs;

		// ISupportsErrorInfo
		STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

		// IScdSpecieDefns
	public:
		STDMETHOD(Specie)(VARIANT WhichOne, IScdSpecieDefn** pItem);
		STDMETHOD(get__NewEnum)(LPUNKNOWN *pVal);
		STDMETHOD(get_Count)(LONG * pVal);
		STDMETHOD(get_SomAll)(LONG * pVal);
		STDMETHOD(get_SomVap)(LONG * pVal);
		STDMETHOD(get_SomLiq)(LONG * pVal);
		STDMETHOD(get_SomSol)(LONG * pVal);
		STDMETHOD(get_SomSL)(LONG * pVal);
		STDMETHOD(get_Phases)(LONG ReqdPhases, SAFEARRAY * * pVal);
		STDMETHOD(get_MolecularWeights)(SAFEARRAY * * pVal);
	};

#endif //__SCDSPECIES_H_
