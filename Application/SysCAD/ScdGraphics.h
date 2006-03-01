
// ScdGraphics.h : Declaration of the CScdGraphics

#ifndef __SCDGRAPHICS_H_
#define __SCDGRAPHICS_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CScdGraphics
class ATL_NO_VTABLE CScdGraphics :
  public CScdCOCmdBase,
  public CComObjectRootEx<CComMultiThreadModel>,
  public CComCoClass<CScdGraphics, &CLSID_ScdGraphics>,
  public ISupportErrorInfo,
  public IConnectionPointContainerImpl<CScdGraphics>,
  public IDispatchImpl<IScdGraphics, &IID_IScdGraphics, &LIBID_ScdApp>,
  public IDispatchImpl<IScdASyncEvents, &IID_IScdASyncEvents, &LIBID_ScdIF>
{
public:
  CScdGraphics();

DECLARE_REGISTRY_RESOURCEID(IDR_SCDGRAPHICS)
DECLARE_GET_CONTROLLING_UNKNOWN()

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CScdGraphics)
  COM_INTERFACE_ENTRY(IScdGraphics)
  //COM_INTERFACE_ENTRY(IDispatch)
  COM_INTERFACE_ENTRY2(IDispatch, IScdGraphics)
  COM_INTERFACE_ENTRY(ISupportErrorInfo)
  COM_INTERFACE_ENTRY(IConnectionPointContainer)
  //COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, m_pUnkMarshaler.p)
  COM_INTERFACE_ENTRY(IScdASyncEvents)
END_COM_MAP()
BEGIN_CONNECTION_POINT_MAP(CScdGraphics)
END_CONNECTION_POINT_MAP()

DECLARE_SCD(eScdGraphicsCodes);

//#ifdef _DEBUG
//BEGIN_CATEGORY_MAP(CScdGraphics)
//  IMPLEMENTED_CATEGORY(CATID_SysCADAppObject)
//END_CATEGORY_MAP()
//#endif

  HRESULT FinalConstruct();
  void FinalRelease();

  //CComPtr<IUnknown> m_pUnkMarshaler;

// ISupportsErrorInfo
  STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IScdGraphics
public:
  STDMETHOD(Page)(VARIANT WhichOne, IScdGraphic** pItem);
  STDMETHOD(get_Count)(LONG * pVal);
  STDMETHOD(Add)(BSTR Name, IScdGraphic** pItem);
  STDMETHOD(Remove)(VARIANT WhichOne);
  STDMETHOD(get__NewEnum)(IUnknown * * pVal);

// IScdASyncEvents
  STDMETHOD(DoEventMsg)(LONG Evt, LONG Data)
    {
    CScdCOCmdBase::DoEventMsg(Evt, Data);
    return S_OK;
    }
  virtual void FireTheEvent(long Evt, long Data);

};

#endif //__SCDGRAPHICS_H_
