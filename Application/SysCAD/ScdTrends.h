
// ScdTrends.h : Declaration of the CScdTrends

#ifndef __SCDTRENDS_H_
#define __SCDTRENDS_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CScdTrends
class ATL_NO_VTABLE CScdTrends :
  public CScdCOCmdBase,
  public CComObjectRootEx<CComMultiThreadModel>,
  public CComCoClass<CScdTrends, &CLSID_ScdTrends>,
  public ISupportErrorInfo,
  public IConnectionPointContainerImpl<CScdTrends>,
  public IDispatchImpl<IScdTrends, &IID_IScdTrends, &LIBID_ScdApp>,
  public IDispatchImpl<IScdASyncEvents, &IID_IScdASyncEvents, &LIBID_ScdIF>
{
public:
  CScdTrends() : CScdCOCmdBase(WMU_COM_APP)
  {
    //m_pUnkMarshaler = NULL;
  }

DECLARE_REGISTRY_RESOURCEID(IDR_SCDTRENDS)
DECLARE_GET_CONTROLLING_UNKNOWN()

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CScdTrends)
  COM_INTERFACE_ENTRY(IScdTrends)
  //COM_INTERFACE_ENTRY(IDispatch)
  COM_INTERFACE_ENTRY2(IDispatch, IScdTrends)
  COM_INTERFACE_ENTRY(ISupportErrorInfo)
  COM_INTERFACE_ENTRY(IConnectionPointContainer)
  //COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, m_pUnkMarshaler.p)
  COM_INTERFACE_ENTRY(IScdASyncEvents)
END_COM_MAP()
BEGIN_CONNECTION_POINT_MAP(CScdTrends)
END_CONNECTION_POINT_MAP()

DECLARE_SCD(long);

#ifdef _DEBUG
BEGIN_CATEGORY_MAP(CScdTrends)
  IMPLEMENTED_CATEGORY(CATID_SysCADAppObject)
END_CATEGORY_MAP()
#endif

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

// IScdTrends
public:

// IScdASyncEvents
  STDMETHOD(DoEventMsg)(LONG Evt, LONG Data)
    {
    CScdCOCmdBase::DoEventMsg(Evt, Data);
    return S_OK;
    }
  virtual void FireTheEvent(long Evt, long Data);

};

#endif //__SCDTRENDS_H_
