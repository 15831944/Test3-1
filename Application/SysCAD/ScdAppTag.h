
// ScdAppTag.h : Declaration of the CScdAppTag

#ifndef __SCDAPPTAG_H_
#define __SCDAPPTAG_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CScdAppTag
class ATL_NO_VTABLE CScdAppTag :
  public CScdCOCmdBase,
  public CComObjectRootEx<CComMultiThreadModel>,
  public CComCoClass<CScdAppTag, &CLSID_ScdAppTag>,
  public ISupportErrorInfo,
  public IConnectionPointContainerImpl<CScdAppTag>,
  public IDispatchImpl<IScdAppTag, &IID_IScdAppTag, &LIBID_ScdApp>,
  public IDispatchImpl<IScdASyncEvents, &IID_IScdASyncEvents, &LIBID_ScdIF>
{
public:
  CScdAppTag() : CScdCOCmdBase(WMU_COM_APP)
  {
    //m_pUnkMarshaler = NULL;
  }

DECLARE_REGISTRY_RESOURCEID(IDR_SCDAPPTAG)
DECLARE_GET_CONTROLLING_UNKNOWN()

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CScdAppTag)
  COM_INTERFACE_ENTRY(IScdAppTag)
  //COM_INTERFACE_ENTRY(IDispatch)
  COM_INTERFACE_ENTRY2(IDispatch, IScdAppTag)
  COM_INTERFACE_ENTRY(ISupportErrorInfo)
  COM_INTERFACE_ENTRY(IConnectionPointContainer)
  //COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, m_pUnkMarshaler.p)
  COM_INTERFACE_ENTRY(IScdASyncEvents)
END_COM_MAP()
BEGIN_CONNECTION_POINT_MAP(CScdAppTag)
END_CONNECTION_POINT_MAP()


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

// IScdAppTag
public:

// IScdASyncEvents
  STDMETHOD(DoEventMsg)(LONG Evt, LONG Data)
    {
    CScdCOCmdBase::DoEventMsg(Evt, Data);
    return S_OK;
    }
  virtual void FireTheEvent(long Evt, long Data);

};

#endif //__SCDAPPTAG_H_
