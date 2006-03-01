
// ScdLicenseApp.h : Declaration of the CScdLicense

#ifndef __SCDLICENSEAPP_H_
#define __SCDLICENSEAPP_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CScdLicenseApp
class ATL_NO_VTABLE CScdLicenseApp :
  public CScdCOCmdBase,
  public CComObjectRootEx<CComMultiThreadModel>,
  public CComCoClass<CScdLicenseApp, &CLSID_ScdLicenseApp>,
  public ISupportErrorInfo,
  public IConnectionPointContainerImpl<CScdLicenseApp>,
  public IDispatchImpl<IScdLicenseApp, &IID_IScdLicenseApp, &LIBID_ScdApp>,
  public IDispatchImpl<IScdASyncEvents, &IID_IScdASyncEvents, &LIBID_ScdIF>
{
public:
  CScdLicenseApp()  : CScdCOCmdBase(WMU_COM_APP)
  {
    //m_pUnkMarshaler = NULL;
  }

DECLARE_REGISTRY_RESOURCEID(IDR_SCDLICENSEAPP)
DECLARE_GET_CONTROLLING_UNKNOWN()

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CScdLicenseApp)
  COM_INTERFACE_ENTRY(IScdLicenseApp)
  //COM_INTERFACE_ENTRY(IDispatch)
  COM_INTERFACE_ENTRY2(IDispatch, IScdLicenseApp)
  COM_INTERFACE_ENTRY(ISupportErrorInfo)
  COM_INTERFACE_ENTRY(IConnectionPointContainer)
  //COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, m_pUnkMarshaler.p)
  COM_INTERFACE_ENTRY(IScdASyncEvents)
END_COM_MAP()
BEGIN_CONNECTION_POINT_MAP(CScdLicenseApp)
END_CONNECTION_POINT_MAP()

DECLARE_SCD(long);

#ifdef _DEBUG
BEGIN_CATEGORY_MAP(CScdLicenseApp)
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

// IScdLicenseApp
public:

// IScdASyncEvents
  STDMETHOD(DoEventMsg)(LONG Evt, LONG Data)
    {
    CScdCOCmdBase::DoEventMsg(Evt, Data);
    return S_OK;
    }
  virtual void FireTheEvent(long Evt, long Data);

};

#endif //__SCDLICENSEAPP_H_
