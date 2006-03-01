// ScdConnectPoints.h : Declaration of the CScdConnectPoints

#ifndef __SCDCONNECTPOINTS_H_
#define __SCDCONNECTPOINTS_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CScdConnectPoints
class ATL_NO_VTABLE CScdConnectPoints : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CScdConnectPoints, &CLSID_ScdConnectPoints>,
	public ISupportErrorInfo,
	//public IConnectionPointContainerImpl<CScdConnectPoints>,
	public IDispatchImpl<IScdConnectPoints, &IID_IScdConnectPoints, &LIBID_ScdSlv>
{
public:
	CScdConnectPoints()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_SCDCONNECTPOINTS)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CScdConnectPoints)
	COM_INTERFACE_ENTRY(IScdConnectPoints)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	//COM_INTERFACE_ENTRY(IConnectionPointContainer)
END_COM_MAP()
//BEGIN_CONNECTION_POINT_MAP(CScdConnectPoints)
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

  CArray <LPDISPATCH, LPDISPATCH> m_IConnectPoints;
  FlwNode     * m_pNode;
  long          m_lTopoCount;

  void          BuildData();

  // ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IScdConnectPoints
public:
	STDMETHOD(get_OutputPathName)(/*[in]*/ eScdLinkTypes Type, /*[in]*/ long PathNo, /*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_InputPathName)(/*[in]*/ eScdLinkTypes Type, /*[in]*/ long PathNo, /*[out, retval]*/ BSTR *pVal);
  STDMETHOD(ConnectPoint)(VARIANT WhichOne, IScdConnectPoint** pItem);
  STDMETHOD(get_Count)(LONG * pVal);
  STDMETHOD(get__NewEnum)(IUnknown * * pVal);

	STDMETHOD(SetObjectInfo)(/*[in]*/ DWORD newVal);
};

#endif //__SCDCONNECTPOINTS_H_
