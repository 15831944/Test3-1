// ScdNodeClass.h : Declaration of the CScdNodeClass

#ifndef __SCDNODECLASS_H_
#define __SCDNODECLASS_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CScdNodeClass
class ATL_NO_VTABLE CScdNodeClass : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CScdNodeClass, &CLSID_ScdNodeClass>,
	public ISupportErrorInfo,
//	public IConnectionPointContainerImpl<CScdNodeClass>,
	public IDispatchImpl<IScdNodeClass, &IID_IScdNodeClass, &LIBID_ScdSlv>
{
public:
	CScdNodeClass()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_SCDNODECLASS)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CScdNodeClass)
	COM_INTERFACE_ENTRY(IScdNodeClass)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	//COM_INTERFACE_ENTRY(IConnectionPointContainer)
END_COM_MAP()
//BEGIN_CONNECTION_POINT_MAP(CScdNodeClass)
//END_CONNECTION_POINT_MAP()

DECLARE_SCD(long);

  HRESULT FinalConstruct()
    {
    m_pNodeClass=NULL;
    m_lTopoCount=-1;
    return S_OK; //CoCreateFreeThreadedMarshaler(GetControllingUnknown(), &m_pUnkMarshaler.p);
    }

  void FinalRelease()
    {
    //m_pUnkMarshaler.Release();
    }

  //CComPtr<IUnknown> m_pUnkMarshaler;
  TagObjClass * m_pNodeClass;
  long          m_lTopoCount;

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IScdNodeClass
public:
	STDMETHOD(SetObjectInfo)(/*[in]*/ DWORD newVal);
	STDMETHOD(get_Category)(/*[out, retval]*/ eScdNodeCat_Flags *pVal);
	STDMETHOD(get_Name)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_DefaultTag)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_Library)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_ShortDescription)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_Description)(/*[out, retval]*/ BSTR *pVal);
};

#endif //__SCDNODECLASS_H_
