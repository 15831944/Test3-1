
// ScdProbal.h : Declaration of the CScdProbal

#ifndef __SCDPROBAL_H_
#define __SCDPROBAL_H_

#include "resource.h"       // main symbols
#include "ScdSlv.h"

/////////////////////////////////////////////////////////////////////////////
// CScdProbal
class ATL_NO_VTABLE CScdProbal :
  public CScdCOCmdBase,
  public CComObjectRootEx<CComMultiThreadModel>,
  public CComCoClass<CScdProbal, &CLSID_ScdProbal>,
  public ISupportErrorInfo,
  //public IConnectionPointContainerImpl<CScdProbal>,
  public IDispatchImpl<IScdProbal, &IID_IScdProbal, &LIBID_ScdSlv>,
  public IDispatchImpl<IScdASyncEvents, &IID_IScdASyncEvents, &LIBID_ScdIF>
  {
  public:
    CScdProbal():
        CScdCOCmdBase(WMU_COM_APP)
          {
          //m_pUnkMarshaler = NULL;
          }

        DECLARE_REGISTRY_RESOURCEID(IDR_SCDPROBAL)
        DECLARE_GET_CONTROLLING_UNKNOWN()

        DECLARE_PROTECT_FINAL_CONSTRUCT()

        BEGIN_COM_MAP(CScdProbal)
          COM_INTERFACE_ENTRY(IScdProbal)
          //DEL   COM_INTERFACE_ENTRY(IDispatch)
          COM_INTERFACE_ENTRY2(IDispatch, IScdProbal)
          COM_INTERFACE_ENTRY(ISupportErrorInfo)
          //COM_INTERFACE_ENTRY(IConnectionPointContainer)
          //COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, m_pUnkMarshaler.p)
          COM_INTERFACE_ENTRY(IScdASyncEvents)
        END_COM_MAP()
        //BEGIN_CONNECTION_POINT_MAP(CScdProbal)
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

        //CComPtr<IUnknown> m_pUnkMarshaler;

        // ISupportsErrorInfo
        STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

        // IScdProbal
  public:

    // IScdASyncEvents
    STDMETHOD(DoEventMsg)(LONG Evt, LONG Data)
      {
      CScdCOCmdBase::DoEventMsg(Evt, Data);
      return S_OK;
      }
    virtual void FireTheEvent(long Evt, long Data);


    // IScdProbal Methods
  public:
    STDMETHOD(get_IsStopped)( VARIANT_BOOL *  pVal);
    STDMETHOD(get_IsIdling)( VARIANT_BOOL *  pVal);
    STDMETHOD(get_IsRunning)( VARIANT_BOOL *  pVal);
    STDMETHOD(Start)();
    STDMETHOD(Step)( long  IterMark,  double  WaitForNext);
    STDMETHOD(Stop)();
    STDMETHOD(Idle)();
  };

#endif //__SCDPROBAL_H_
