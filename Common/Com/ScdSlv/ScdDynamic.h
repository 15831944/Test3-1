	
// ScdDynamic.h : Declaration of the CScdDynamic

#ifndef __SCDDYNAMIC_H_
#define __SCDDYNAMIC_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CScdDynamic
class ATL_NO_VTABLE CScdDynamic : 
  public CScdCOCmdBase,
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CScdDynamic, &CLSID_ScdDynamic>,
	public ISupportErrorInfo,
	//public IConnectionPointContainerImpl<CScdDynamic>,
	public IDispatchImpl<IScdDynamic, &IID_IScdDynamic, &LIBID_ScdSlv>,
	public IDispatchImpl<IScdASyncEvents, &IID_IScdASyncEvents, &LIBID_ScdIF>
{
public:
	CScdDynamic():
	CScdCOCmdBase(WMU_COM_APP)
	{
		//m_pUnkMarshaler = NULL;
	}

DECLARE_REGISTRY_RESOURCEID(IDR_SCDDYNAMIC)
DECLARE_GET_CONTROLLING_UNKNOWN()

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CScdDynamic)
	COM_INTERFACE_ENTRY(IScdDynamic)
	//COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY2(IDispatch, IScdDynamic)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	//COM_INTERFACE_ENTRY(IConnectionPointContainer)
	//COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, m_pUnkMarshaler.p)
	COM_INTERFACE_ENTRY(IScdASyncEvents)
END_COM_MAP()
//BEGIN_CONNECTION_POINT_MAP(CScdDynamic)
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

// IScdDynamic
public:
  STDMETHOD(RunToSteadyState)();
	STDMETHOD(Idle)();
	STDMETHOD(Stop)();
	STDMETHOD(Step)(/*[in, defaultvalue(0)]*/ long IterMark, /*[in, defaultvalue(0.0)]*/ double WaitForNext);
	STDMETHOD(Start)();
	STDMETHOD(get_RealTimeMultiplier)(/*[out, retval]*/ double *pVal);
	STDMETHOD(put_RealTimeMultiplier)(/*[in]*/ double newVal);
	STDMETHOD(get_RealTime)(/*[out, retval]*/ VARIANT_BOOL *pVal);
	STDMETHOD(put_RealTime)(/*[in]*/ VARIANT_BOOL newVal);
	STDMETHOD(get_StepSize)(/*[out, retval]*/ double *pVal);
	STDMETHOD(put_StepSize)(/*[in]*/ double newVal);
	STDMETHOD(get_IsRunning)(/*[out, retval]*/ VARIANT_BOOL *pVal);
	STDMETHOD(get_IsIdling)(/*[out, retval]*/ VARIANT_BOOL *pVal);
	STDMETHOD(get_IsStopped)(/*[out, retval]*/ VARIANT_BOOL *pVal);

  // IScdASyncEvents
	STDMETHOD(DoEventMsg)(LONG Evt, LONG Data)
	  {
    CScdCOCmdBase::DoEventMsg(Evt, Data);
		return S_OK;
	  }
  virtual void FireTheEvent(long Evt, long Data);
};

#endif //__SCDDYNAMIC_H_
