// ScdConnects.h : Declaration of the CScdConnects

#ifndef __SCDCONNECTS_H_
#define __SCDCONNECTS_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CScdConnects
class ATL_NO_VTABLE CScdConnects : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CScdConnects, &CLSID_ScdConnects>,
	public ISupportErrorInfo,
	//public IConnectionPointContainerImpl<CScdConnects>,
	public IDispatchImpl<IScdConnects, &IID_IScdConnects, &LIBID_ScdSlv>
{
public:
	CScdConnects()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_SCDCONNECTS)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CScdConnects)
	COM_INTERFACE_ENTRY(IScdConnects)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	//COM_INTERFACE_ENTRY(IConnectionPointContainer)
END_COM_MAP()
//BEGIN_CONNECTION_POINT_MAP(CScdConnects)
//END_CONNECTION_POINT_MAP()

DECLARE_SCD(eScdNodeCodes);

  HRESULT FinalConstruct()
    {
    m_pNode=NULL;
    m_lTopoCount=-1;
    return S_OK;
    }

  void FinalRelease()
    {
    }

  CArray <LPDISPATCH, LPDISPATCH> m_IConnects;
  FlwNode     * m_pNode;
  long          m_lTopoCount;

  void          BuildData();

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IScdConnects
public:
  STDMETHOD(Connect)(VARIANT WhichOne, IScdConnect** pItem);
  STDMETHOD(get_Count)(LONG * pVal);
  STDMETHOD(get__NewEnum)(IUnknown * * pVal);
	
	STDMETHOD(SetObjectInfo)(/*[in]*/ DWORD newVal);
};

#endif //__SCDCONNECTS_H_
