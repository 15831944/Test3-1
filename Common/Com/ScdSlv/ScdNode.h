
// ScdNode.h : Declaration of the CScdNode

#ifndef __SCDNODE_H_
#define __SCDNODE_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CScdNode
class ATL_NO_VTABLE CScdNode :
  public CComObjectRootEx<CComMultiThreadModel>,
  public CComCoClass<CScdNode, &CLSID_ScdNode>,
  public ISupportErrorInfo,
//  public IConnectionPointContainerImpl<CScdNode>,
  public IDispatchImpl<IScdNode, &IID_IScdNode, &LIBID_ScdSlv>
  {
  public:
    CScdNode() 
      {
      //m_pUnkMarshaler = NULL;
      }

    DECLARE_REGISTRY_RESOURCEID(IDR_SCDNODE)
    DECLARE_GET_CONTROLLING_UNKNOWN()

    DECLARE_PROTECT_FINAL_CONSTRUCT()

    BEGIN_COM_MAP(CScdNode)
      COM_INTERFACE_ENTRY(IScdNode)
      COM_INTERFACE_ENTRY(IDispatch)
      COM_INTERFACE_ENTRY(ISupportErrorInfo)
      //COM_INTERFACE_ENTRY(IConnectionPointContainer)
    END_COM_MAP()
    //BEGIN_CONNECTION_POINT_MAP(CScdNode)
    //END_CONNECTION_POINT_MAP()

    DECLARE_SCD(eScdNodeCodes);

    HRESULT FinalConstruct()
      {
      m_pNode=NULL;
      m_lTopoCount=-1;
      return S_OK; //CoCreateFreeThreadedMarshaler(GetControllingUnknown(), &m_pUnkMarshaler.p);
      }

    void FinalRelease()
      {
      //m_pUnkMarshaler.Release();
      }

    //CComPtr<IUnknown> m_pUnkMarshaler;
    FlwNode * m_pNode;
    long      m_lTopoCount;

    // ISupportsErrorInfo
    STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

    // IScdNode
  public:
    STDMETHOD(get_RmtNodes)(/*[in]*/ eScdDirections Dirns, /*[in,defaultvalue(eScdNodeType_All)]*/ eScdNodeTypes Types, /*[out, retval]*/ IScdNodes* *pVal);
    STDMETHOD(get_DstNode)(/*[out, retval]*/ IScdNode * *pVal);
    STDMETHOD(get_SrcNode)(/*[out, retval]*/ IScdNode * *pVal);
    STDMETHOD(get_ConnectPoints)(/*[in]*/ eScdDirections Direction, /*[in]*/ eScdNodeTypes Types, /*[out, retval]*/ IScdConnectPoints * *pConnectPoints);
    STDMETHOD(get_Connects)(/*[in]*/ eScdDirections Direction, /*[in]*/ eScdNodeTypes Types, /*[out, retval]*/ IScdConnects**pConnects);
    STDMETHOD(get_TypeString)(/*[out, retval]*/ BSTR *pVal);
    STDMETHOD(SetObjectInfo)(/*[in]*/ DWORD newVal);
    STDMETHOD(get_Class)(/*[out, retval]*/ IScdNodeClass * *pVal);
    STDMETHOD(get_Type)(/*[out, retval]*/ eScdNodeTypes *pVal);
    STDMETHOD(get_Tag)(/*[out, retval]*/ BSTR *pVal);
    STDMETHOD(put_Tag)(/*[in]*/ BSTR newVal);
    STDMETHOD(get_Electrics)(IScdElectrics** pVal);
  };

#endif //__SCDNODE_H_
