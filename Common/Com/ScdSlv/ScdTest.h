
// ScdTest.h : Declaration of the CScdTest

#ifndef __SCDTEST_H_
#define __SCDTEST_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CScdTest
class ATL_NO_VTABLE CScdTest :
  public CComObjectRootEx<CComMultiThreadModel>,
  public CComCoClass<CScdTest, &CLSID_ScdTest>,
  public ISupportErrorInfo,
  //public IConnectionPointContainerImpl<CScdTest>,
  public IDispatchImpl<IScdTest, &IID_IScdTest, &LIBID_ScdSlv>
  {
  public:
    CScdTest()
      {
      //m_pUnkMarshaler = NULL;
      }

    DECLARE_REGISTRY_RESOURCEID(IDR_SCDTEST)
    DECLARE_GET_CONTROLLING_UNKNOWN()

    DECLARE_PROTECT_FINAL_CONSTRUCT()

    BEGIN_COM_MAP(CScdTest)
      COM_INTERFACE_ENTRY(IScdTest)
      COM_INTERFACE_ENTRY(IDispatch)
      COM_INTERFACE_ENTRY(ISupportErrorInfo)
      //COM_INTERFACE_ENTRY(IConnectionPointContainer)
    END_COM_MAP()
    //BEGIN_CONNECTION_POINT_MAP(CScdTest)
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

    // ISupportsErrorInfo
    STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

    // IScdTest
  public:
    STDMETHOD(ThrowException)(/*[in]*/ long Sev, /*[in]*/ long Code);
    STDMETHOD(ReturnHResult)(long Sev, long Code);

  };

#endif //__SCDTEST_H_
