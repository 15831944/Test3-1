// ScdConnectPoint.h : Declaration of the CScdConnectPoint

#ifndef __SCDCONNECTPOINT_H_
#define __SCDCONNECTPOINT_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CScdConnectPoint
class ATL_NO_VTABLE CScdConnectPoint : 
  public CComObjectRootEx<CComMultiThreadModel>,
  public CComCoClass<CScdConnectPoint, &CLSID_ScdConnectPoint>,
  public ISupportErrorInfo,
  //public IConnectionPointContainerImpl<CScdConnectPoint>,
  public IDispatchImpl<IScdConnectPoint, &IID_IScdConnectPoint, &LIBID_ScdSlv>
  {
  public:
    CScdConnectPoint()
      {
      }

    DECLARE_REGISTRY_RESOURCEID(IDR_SCDCONNECTPOINT)

    DECLARE_PROTECT_FINAL_CONSTRUCT()

    BEGIN_COM_MAP(CScdConnectPoint)
      COM_INTERFACE_ENTRY(IScdConnectPoint)
      COM_INTERFACE_ENTRY(IDispatch)
      COM_INTERFACE_ENTRY(ISupportErrorInfo)
      //COM_INTERFACE_ENTRY(IConnectionPointContainer)
    END_COM_MAP()
    //BEGIN_CONNECTION_POINT_MAP(CScdConnectPoint)
    //END_CONNECTION_POINT_MAP()

    DECLARE_SCD(eScdNodeCodes);

    HRESULT FinalConstruct()
      {
      m_pNode=NULL;
      m_lIndex=-1;
      m_lTopoCount=-1;
      return S_OK;
      }

    void FinalRelease()
      {
      }

    FlwNode * m_pNode;
    long       m_lIndex;
    long       m_lTopoCount;

    // ISupportsErrorInfo
    STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

    // IScdConnectPoint
  public:
    STDMETHOD(get_ID)(/*[out, retval]*/ long *pVal);
    STDMETHOD(get_Index)(/*[out, retval]*/ long *pVal);
    STDMETHOD(get_TypeString)(/*[out, retval]*/ BSTR *pVal);
    STDMETHOD(SetObjectInfo)(/*[in]*/ DWORD newVal, /*[in]*/ long Index);
    STDMETHOD(get_ConnectCountMax)(/*[out, retval]*/ long *pVal);
    STDMETHOD(get_ConnectCountRqd)(/*[out, retval]*/ long *pVal);
    STDMETHOD(get_Type)(/*[out, retval]*/ eScdNodeTypes *pVal);
    STDMETHOD(get_Output)(/*[out, retval]*/ VARIANT_BOOL *pVal);
    STDMETHOD(get_Input)(/*[out, retval]*/ VARIANT_BOOL *pVal);
    STDMETHOD(get_Description)(/*[out, retval]*/ BSTR *pVal);
    STDMETHOD(get_Name)(/*[out, retval]*/ BSTR *pVal);
  };

#endif //__SCDCONNECTPOINT_H_
