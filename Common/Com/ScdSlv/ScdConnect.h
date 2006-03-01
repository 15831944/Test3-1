// ScdConnect.h : Declaration of the CScdConnect

#ifndef __SCDCONNECT_H_
#define __SCDCONNECT_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CScdConnect
class ATL_NO_VTABLE CScdConnect : 
  public CComObjectRootEx<CComMultiThreadModel>,
  public CComCoClass<CScdConnect, &CLSID_ScdConnect>,
  public ISupportErrorInfo,
  //public IConnectionPointContainerImpl<CScdConnect>,
  public IDispatchImpl<IScdConnect, &IID_IScdConnect, &LIBID_ScdSlv>
  {
  public:
    CScdConnect()
      {
      }

    DECLARE_REGISTRY_RESOURCEID(IDR_SCDCONNECT)

    DECLARE_PROTECT_FINAL_CONSTRUCT()

    BEGIN_COM_MAP(CScdConnect)
      COM_INTERFACE_ENTRY(IScdConnect)
      COM_INTERFACE_ENTRY(IDispatch)
      COM_INTERFACE_ENTRY(ISupportErrorInfo)
      //COM_INTERFACE_ENTRY(IConnectionPointContainer)
    END_COM_MAP()
    //BEGIN_CONNECTION_POINT_MAP(CScdConnect)
    //END_CONNECTION_POINT_MAP()

    DECLARE_SCD(eScdNodeCodes);

    HRESULT FinalConstruct()
      {
      m_pNode=NULL;
      m_lTopoCount=-1;
      m_lIndex=-1;
      return S_OK;
      }

    void FinalRelease()
      {
      }

    //CIORec*  m_pIO;
    FlwNode*  m_pNode;
    long      m_lIndex;
    long      m_lTopoCount;

    // ISupportsErrorInfo
    STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

    // IScdConnect
  public:
    STDMETHOD(get_Node)(/*[out, retval]*/ IScdNode* *pVal);
    STDMETHOD(get_RmtConnectPoint)(/*[out, retval]*/ IScdConnectPoint* *pVal);
    STDMETHOD(get_ConnectPoint)(/*[out, retval]*/ IScdConnectPoint* *pVal);
    STDMETHOD(get_RmtConnectString)(/*[out, retval]*/ BSTR *pVal);
    STDMETHOD(get_ConnectString)(/*[out, retval]*/ BSTR *pVal);
    STDMETHOD(get_RmtNode)(/*[out, retval]*/ IScdNode* *pVal);
    STDMETHOD(get_RmtIndex)(/*[out, retval]*/ long *pVal);
    STDMETHOD(get_Index)(/*[out, retval]*/ long *pVal);
    STDMETHOD(SetObjectInfo)(/*[in]*/ DWORD newVal, /*[in]*/ long Index);
  };

#endif //__SCDCONNECT_H_
