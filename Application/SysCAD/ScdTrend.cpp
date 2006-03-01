// ScdTrend.cpp : Implementation of CScdTrend
#include "stdafx.h"
#include "SysCAD_i.h"
#include "ScdTrend.h"

/////////////////////////////////////////////////////////////////////////////
// CScdTrend

STDMETHODIMP CScdTrend::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IScdTrend
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

void CScdTrend::FireTheEvent(long Evt, long Data)
  {
  switch (Evt)
    {
    case ComCmd_NULL : /*Fire_On...*/ ; break;
    };
  };
