// ScdStopWatch.cpp : Implementation of CScdStopWatch
#include "stdafx.h"
#include "ScdVB.h"
#include "ScdStopWatch.h"

/////////////////////////////////////////////////////////////////////////////
// CScdStopWatch


STDMETHODIMP CScdStopWatch::Start()
  {
  dCurrent -= Time(); dwLaps++; bRunning=1; 
	return S_OK;
  }

STDMETHODIMP CScdStopWatch::Stop()
  {
  dCurrent += Time(); 
  bRunning=0;
	return S_OK;
  }

STDMETHODIMP CScdStopWatch::ReStart()
  {
	dCurrent=dAccum=0.0; 
  Start();

	return S_OK;
  }

STDMETHODIMP CScdStopWatch::Clear()
  {
  dCurrent=dAccum=0.0;
  dwLaps=0; 
  bRunning=0;
	return S_OK;
  }

STDMETHODIMP CScdStopWatch::get_Time(double *pVal)
  {
	*pVal=Time();
	return S_OK;
  }

STDMETHODIMP CScdStopWatch::get_Elapsed(double *pVal)
  {
	*pVal=Seconds();
	return S_OK;
  }

STDMETHODIMP CScdStopWatch::get_Lap(double *pVal)
  {
	double d1=Seconds();
  double d2=d1-dAccum; 
  dAccum=d1; 
  dwLaps++; 
  *pVal=d2;
  
	return S_OK;
}

STDMETHODIMP CScdStopWatch::get_Laps(DWORD *pVal)
  {
	*pVal=dwLaps;
  return S_OK;
  }
