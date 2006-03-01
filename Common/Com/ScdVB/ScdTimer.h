// ScdTimer.h : Declaration of the CScdTimer

#ifndef __SCDTIMER_H_
#define __SCDTIMER_H_

#include "resource.h"       // main symbols
#include "ScdVBCP.h"

/////////////////////////////////////////////////////////////////////////////
// CScdTimer
class ATL_NO_VTABLE CScdTimer : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CScdTimer, &CLSID_ScdTimer>,
	public ISupportErrorInfo,
	public IConnectionPointContainerImpl<CScdTimer>,
	public IDispatchImpl<IScdTimer, &IID_IScdTimer, &LIBID_ScdVBLib>,
	public CProxy_IScdTimerEvents< CScdTimer >
{
public:
	enum consts { MIN_TIMEOUT = 10 };
	CScdTimer()
	{
		m_lInterval = 1000;
		m_bActive = false;
	}

DECLARE_REGISTRY_RESOURCEID(IDR_SCDTIMER)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CScdTimer)
	COM_INTERFACE_ENTRY(IScdTimer)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
	COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
END_COM_MAP()
BEGIN_CONNECTION_POINT_MAP(CScdTimer)
CONNECTION_POINT_ENTRY(DIID__ScdTimerEvents)
END_CONNECTION_POINT_MAP()


// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IScdTimer
public:
	STDMETHOD(get_Interval)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_Interval)(/*[in]*/ long newVal);
	STDMETHOD(Stop)();
	STDMETHOD(Start)();
	static DWORD WINAPI StaticThreadProc(LPVOID);
	void FinalRelease();
	HRESULT FinalConstruct();

private:
	bool m_bActive;
	HANDLE m_hTerminate;
	DWORD ThreadProc();
	HANDLE m_hThreadHandle;
	DWORD m_dwThreadID;
	long	m_lInterval;
};

#endif //__SCDTIMER_H_
