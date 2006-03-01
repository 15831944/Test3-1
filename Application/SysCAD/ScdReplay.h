
// ScdReplay.h : Declaration of the CScdReplay

#ifndef __SCDREPLAY_H_
#define __SCDREPLAY_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CScdReplay
class ATL_NO_VTABLE CScdReplay :
  public CScdCOCmdBase,
  public CComObjectRootEx<CComMultiThreadModel>,
  public CComCoClass<CScdReplay, &CLSID_ScdReplay>,
  public ISupportErrorInfo,
  public IConnectionPointContainerImpl<CScdReplay>,
  public IDispatchImpl<IScdReplay, &IID_IScdReplay, &LIBID_ScdApp>,
  public IDispatchImpl<IScdASyncEvents, &IID_IScdASyncEvents, &LIBID_ScdIF>
{
public:
  CScdReplay() : CScdCOCmdBase(WMU_COM_APP)
  {
    //m_pUnkMarshaler = NULL;
  }

DECLARE_REGISTRY_RESOURCEID(IDR_SCDREPLAY)
DECLARE_GET_CONTROLLING_UNKNOWN()

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CScdReplay)
  COM_INTERFACE_ENTRY(IScdReplay)
  //COM_INTERFACE_ENTRY(IDispatch)
  COM_INTERFACE_ENTRY2(IDispatch, IScdReplay)
  COM_INTERFACE_ENTRY(ISupportErrorInfo)
  COM_INTERFACE_ENTRY(IConnectionPointContainer)
  //COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, m_pUnkMarshaler.p)
  COM_INTERFACE_ENTRY(IScdASyncEvents)
END_COM_MAP()
BEGIN_CONNECTION_POINT_MAP(CScdReplay)
END_CONNECTION_POINT_MAP()

DECLARE_SCD(long);

#ifdef _DEBUG
BEGIN_CATEGORY_MAP(CScdReplay)
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

// IScdReplay
public:

// IScdASyncEvents
  STDMETHOD(DoEventMsg)(LONG Evt, LONG Data)
    {
    CScdCOCmdBase::DoEventMsg(Evt, Data);
    return S_OK;
    }
  virtual void FireTheEvent(long Evt, long Data);

};

#endif //__SCDREPLAY_H_
