
// ScdDebug.h : Declaration of the CScdDebug

#ifndef __SCDDEBUG_H_
#define __SCDDEBUG_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CScdDebug
class ATL_NO_VTABLE CScdDebug : 
  //public CScdCOCmdBase,
  public CComObjectRootEx<CComMultiThreadModel>,
  public CComCoClass<CScdDebug, &CLSID_ScdDebug>,
  public IDispatchImpl<IScdDebug, &IID_IScdDebug, &LIBID_ScdSlv>
  //,	public IDispatchImpl<IScdASyncEvents, &IID_IScdASyncEvents, &LIBID_ScdIF>
  {
  public:
    CScdDebug() //: CScdCOCmdBase(WMU_COM_APP)
      {
      //m_pUnkMarshaler = NULL;
      }

    DECLARE_REGISTRY_RESOURCEID(IDR_SCDDEBUG)
    DECLARE_GET_CONTROLLING_UNKNOWN()

    DECLARE_PROTECT_FINAL_CONSTRUCT()

    BEGIN_COM_MAP(CScdDebug)
      COM_INTERFACE_ENTRY(IScdDebug)
      COM_INTERFACE_ENTRY(IDispatch)
      //COM_INTERFACE_ENTRY2(IDispatch, IScdDebug)
      //COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, m_pUnkMarshaler.p)
      //COM_INTERFACE_ENTRY(IScdASyncEvents)
    END_COM_MAP()

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

    // IScdDebug
  public:
    STDMETHOD(FileClose)();
    STDMETHOD(FileOpen)(/*[in]*/ BSTR FileName);

    // // IScdASyncEvents
    //STDMETHOD(DoEventMsg)(LONG Evt, LONG Data)
    //  {
    //   CScdCOCmdBase::DoEventMsg(Evt, Data);
    //	return S_OK;
    //  }
    // virtual void FireTheEvent(long Evt, long Data){};

  };

#endif //__SCDDEBUG_H_
