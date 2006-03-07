
// ScdReports.h : Declaration of the CScdReports

#ifndef __SCDREPORTS_H_
#define __SCDREPORTS_H_

#include "resource.h"       // main symbols
#include "..\..\common\com\scdif\scdcomevts.h"       // main symbols
#include "ScdAppCP.h"

/////////////////////////////////////////////////////////////////////////////
// CScdReports
class ATL_NO_VTABLE CScdReports :
  public CScdCOCmdBase,
  public CComObjectRootEx<CComMultiThreadModel>,
  public CComCoClass<CScdReports, &CLSID_ScdReports>,
  public ISupportErrorInfo,
  public IConnectionPointContainerImpl<CScdReports>,
  public IDispatchImpl<IScdReports, &IID_IScdReports, &LIBID_ScdApp>,
  public IDispatchImpl<IScdASyncEvents, &IID_IScdASyncEvents, &LIBID_ScdIF>,
  public CProxy_IScdReportsEvents< CScdReports >
{
public:
  CScdReports() :
  CScdCOCmdBase(WMU_COM_APP)
  {
    //m_pUnkMarshaler = NULL;
  }

DECLARE_REGISTRY_RESOURCEID(IDR_SCDREPORTS)
DECLARE_GET_CONTROLLING_UNKNOWN()

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CScdReports)
  COM_INTERFACE_ENTRY(IScdReports)
  //COM_INTERFACE_ENTRY(IDispatch)
  COM_INTERFACE_ENTRY2(IDispatch, IScdReports)
  COM_INTERFACE_ENTRY(ISupportErrorInfo)
  COM_INTERFACE_ENTRY(IConnectionPointContainer)
  //COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, m_pUnkMarshaler.p)
  COM_INTERFACE_ENTRY(IScdASyncEvents)
  COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
END_COM_MAP()
BEGIN_CONNECTION_POINT_MAP(CScdReports)
CONNECTION_POINT_ENTRY(DIID__IScdReportsEvents)
END_CONNECTION_POINT_MAP()

DECLARE_SCD(eScdReportsCodes);

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

// IScdReports
public:
  STDMETHOD(Process)(/*[in]*/ eScdReportTypes Type, /*[in]*/ BSTR Filename, /*[in]*/ BSTR Reportname, /*[in]*/ VARIANT_BOOL SaveAndClose);
  STDMETHOD(Report)(VARIANT WhichOne, IScdReport** pItem);
  STDMETHOD(get_Count)(LONG * pVal);
  STDMETHOD(Add)(BSTR IDStr, IScdReport** pItem);
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

#endif //__SCDREPORTS_H_
