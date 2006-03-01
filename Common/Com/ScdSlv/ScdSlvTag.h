
// ScdSlvTag.h : Declaration of the CScdTag

#ifndef __SCDTAG_H_
#define __SCDTAG_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CScdSlvTag
class ATL_NO_VTABLE CScdSlvTag :
  public CComObjectRootEx<CComMultiThreadModel>,
  public CComCoClass<CScdSlvTag, &CLSID_ScdSlvTag>,
  public ISupportErrorInfo,
  //public IConnectionPointContainerImpl<CScdSlvTag>,
  public IDispatchImpl<IScdSlvTag, &IID_IScdSlvTag, &LIBID_ScdSlv>
  {
  public:
    CScdSlvTag() 
      {
      //m_pUnkMarshaler = NULL;
      }

    DECLARE_REGISTRY_RESOURCEID(IDR_SCDSLVTAG)
    DECLARE_GET_CONTROLLING_UNKNOWN()

    DECLARE_PROTECT_FINAL_CONSTRUCT()

    BEGIN_COM_MAP(CScdSlvTag)
      COM_INTERFACE_ENTRY(IScdSlvTag)
      COM_INTERFACE_ENTRY(IDispatch)
      COM_INTERFACE_ENTRY(ISupportErrorInfo)
      //COM_INTERFACE_ENTRY(IConnectionPointContainer)
    END_COM_MAP()
    //BEGIN_CONNECTION_POINT_MAP(CScdSlvTag)
    //END_CONNECTION_POINT_MAP()

    DECLARE_SCD(long);

    HRESULT FinalConstruct()
      {
      return S_OK; //CoCreateFreeThreadedMarshaler(GetControllingUnknown(), &m_pUnkMarshaler.p);
      }

    void FinalRelease()
      {
      //m_pUnkMarshaler.Release();
      }

    //CComPtr<IUnknown> m_pUnkMarshaler;

    // ISupportsErrorInfo
    STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

    // IScdSlvTag
  public:

  };

#endif //__SCDTAG_H_
