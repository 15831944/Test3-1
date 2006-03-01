// ScdStopWatch.h : Declaration of the CScdStopWatch

#ifndef __SCDSTOPWATCH_H_
#define __SCDSTOPWATCH_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CScdStopWatch
class ATL_NO_VTABLE CScdStopWatch : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CScdStopWatch, &CLSID_ScdStopWatch>,
	public IDispatchImpl<IScdStopWatch, &IID_IScdStopWatch, &LIBID_ScdVBLib>
{
    LARGE_INTEGER Freq;
    bool bRunning;
    DWORD dwLaps;
    double dCurrent;
    double dAccum;

  double  Time() 
    { 
    LARGE_INTEGER Tmp; 
    QueryPerformanceCounter(&Tmp); 
    return ((double)(Tmp.QuadPart))/Freq.QuadPart; 
    }
  double  Seconds()
    { 
    return dCurrent + (bRunning ? Time(): 0.0); 
    };

public:
	CScdStopWatch()
	{
   Clear(); 
   QueryPerformanceFrequency(&Freq); 
	}

DECLARE_REGISTRY_RESOURCEID(IDR_SCDSTOPWATCH)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CScdStopWatch)
	COM_INTERFACE_ENTRY(IScdStopWatch)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// IScdStopWatch
public:
	STDMETHOD(get_Laps)(/*[out, retval]*/ DWORD *pVal);
	STDMETHOD(get_Lap)(/*[out, retval]*/ double *pVal);
	STDMETHOD(get_Elapsed)(/*[out, retval]*/ double *pVal);
	STDMETHOD(get_Time)(/*[out, retval]*/ double *pVal);
	STDMETHOD(Clear)();
	STDMETHOD(ReStart)();
	STDMETHOD(Stop)();
	STDMETHOD(Start)();
};

#endif //__SCDSTOPWATCH_H_
