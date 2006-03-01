
// ScdHistorian.h : Declaration of the CScdHistorian

#ifndef __SCDHISTORIAN_H_
#define __SCDHISTORIAN_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CScdHistorian
class ATL_NO_VTABLE CScdHistorian :
  public CScdCOCmdBase,
  public CComObjectRootEx<CComMultiThreadModel>,
  public CComCoClass<CScdHistorian, &CLSID_ScdHistorian>,
  public ISupportErrorInfo,
  public IConnectionPointContainerImpl<CScdHistorian>,
  public IDispatchImpl<IScdHistorian, &IID_IScdHistorian, &LIBID_ScdApp>,
  public IDispatchImpl<IScdASyncEvents, &IID_IScdASyncEvents, &LIBID_ScdIF>
  {
  public:
    CScdHistorian() : CScdCOCmdBase(WMU_COM_APP)
      {
      //m_pUnkMarshaler = NULL;
      }

    DECLARE_REGISTRY_RESOURCEID(IDR_SCDHISTORIAN)
    DECLARE_GET_CONTROLLING_UNKNOWN()

    DECLARE_PROTECT_FINAL_CONSTRUCT()

    BEGIN_COM_MAP(CScdHistorian)
      COM_INTERFACE_ENTRY(IScdHistorian)
      //COM_INTERFACE_ENTRY(IDispatch)
      COM_INTERFACE_ENTRY2(IDispatch, IScdHistorian)
      COM_INTERFACE_ENTRY(ISupportErrorInfo)
      COM_INTERFACE_ENTRY(IConnectionPointContainer)
      //COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, m_pUnkMarshaler.p)
      COM_INTERFACE_ENTRY(IScdASyncEvents)
    END_COM_MAP()
    BEGIN_CONNECTION_POINT_MAP(CScdHistorian)
    END_CONNECTION_POINT_MAP()

    DECLARE_SCD(long);

#ifdef _DEBUG
    BEGIN_CATEGORY_MAP(CScdHistorian)
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

    // IScdHistorian
  public:

    // IScdASyncEvents
    STDMETHOD(DoEventMsg)(LONG Evt, LONG Data)
      {
      CScdCOCmdBase::DoEventMsg(Evt, Data);
      return S_OK;
      }
    virtual void FireTheEvent(long Evt, long Data);

    //STDMETHOD(DeleteData)(void);
    STDMETHOD(Delete)(void);
    STDMETHOD(RecordTags)(BSTR TagFileName);
    STDMETHOD(DeleteData)(void);
    STDMETHOD(ExtractData)(BSTR FileName, eScdTimeFormat Fmt, VARIANT StartTime, DOUBLE Duration, DOUBLE Interval, eScdHistDataExtractOptions Options);
  };

#endif //__SCDHISTORIAN_H_
