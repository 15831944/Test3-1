
// ScdSnapshot.h : Declaration of the CScdSnapshot

#ifndef __SCDSNAPSHOT_H_
#define __SCDSNAPSHOT_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CScdSnapshot
class ATL_NO_VTABLE CScdSnapshot :
  public CScdCOCmdBase,
  public CComObjectRootEx<CComMultiThreadModel>,
  public CComCoClass<CScdSnapshot, &CLSID_ScdSnapshot>,
  public ISupportErrorInfo,
  public IConnectionPointContainerImpl<CScdSnapshot>,
  public IDispatchImpl<IScdSnapshot, &IID_IScdSnapshot, &LIBID_ScdApp>,
  public IDispatchImpl<IScdASyncEvents, &IID_IScdASyncEvents, &LIBID_ScdIF>
{
public:
  CScdSnapshot() : CScdCOCmdBase(WMU_COM_APP)
  {
    //m_pUnkMarshaler = NULL;
  }

DECLARE_REGISTRY_RESOURCEID(IDR_SCDSNAPSHOT)
DECLARE_GET_CONTROLLING_UNKNOWN()

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CScdSnapshot)
  COM_INTERFACE_ENTRY(IScdSnapshot)
  //COM_INTERFACE_ENTRY(IDispatch)
  COM_INTERFACE_ENTRY2(IDispatch, IScdSnapshot)
  COM_INTERFACE_ENTRY(ISupportErrorInfo)
  COM_INTERFACE_ENTRY(IConnectionPointContainer)
  //COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, m_pUnkMarshaler.p)
  COM_INTERFACE_ENTRY(IScdASyncEvents)
END_COM_MAP()
BEGIN_CONNECTION_POINT_MAP(CScdSnapshot)
END_CONNECTION_POINT_MAP()

DECLARE_SCD(long);

#ifdef _DEBUG
BEGIN_CATEGORY_MAP(CScdSnapshot)
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

// IScdSnapshot
public:

// IScdASyncEvents
  STDMETHOD(DoEventMsg)(LONG Evt, LONG Data)
    {
    CScdCOCmdBase::DoEventMsg(Evt, Data);
    return S_OK;
    }
  virtual void FireTheEvent(long Evt, long Data);

};

#endif //__SCDSNAPSHOT_H_
