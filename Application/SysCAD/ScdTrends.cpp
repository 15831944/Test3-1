// ScdTrends.cpp : Implementation of CScdTrends
#include "stdafx.h"
#include "SysCAD_i.h"
#include "ScdTrends.h"

/////////////////////////////////////////////////////////////////////////////
// CScdTrends

STDMETHODIMP CScdTrends::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IScdTrends
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

void CScdTrends::FireTheEvent(long Evt, long Data)
  {
  switch (Evt)
    {
    case ComCmd_NULL : /*Fire_On...*/ ; break;
    };
  };
