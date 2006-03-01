// ScdEventRelay.h : Declaration of the CScdEventRelay

#ifndef __SCDEVENTRELAY_H_
#define __SCDEVENTRELAY_H_

#include "resource.h"       // main symbols
#include "ScdVBCP.h"

/////////////////////////////////////////////////////////////////////////////
// CScdEventRelay
class ATL_NO_VTABLE CScdEventRelay : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CScdEventRelay, &CLSID_ScdEventRelay>,
	public ISupportErrorInfo,
	public IConnectionPointContainerImpl<CScdEventRelay>,
	public IDispatchImpl<IScdEventRelay, &IID_IScdEventRelay, &LIBID_ScdVBLib>,
	public CProxy_IScdEventRelayEvents< CScdEventRelay >
{
public:
	CScdEventRelay()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_SCDEVENTRELAY)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CScdEventRelay)
	COM_INTERFACE_ENTRY(IScdEventRelay)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
	COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
END_COM_MAP()
BEGIN_CONNECTION_POINT_MAP(CScdEventRelay)
CONNECTION_POINT_ENTRY(DIID__ScdEventRelayEvents)
END_CONNECTION_POINT_MAP()


// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IScdEventRelay
public:
	void FinalRelease();
	HRESULT FinalConstruct();
	STDMETHOD(NotifyMe)(/*[in]*/ long i_lCookie);
private:
	DWORD ThreadProc();
  static DWORD WINAPI StaticThreadProc( LPVOID );
  HANDLE  m_evStop;
  HANDLE  m_hThreadProc;
  HANDLE  m_evNotify;
  std::queue< long , std::list<long> > m_qCookies;
};

#endif //__SCDEVENTRELAY_H_
