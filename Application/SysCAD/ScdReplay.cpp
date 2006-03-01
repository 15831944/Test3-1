// ScdReplay.cpp : Implementation of CScdReplay
#include "stdafx.h"
#include "SysCAD_i.h"
#include "ScdReplay.h"

/////////////////////////////////////////////////////////////////////////////
// CScdReplay

STDMETHODIMP CScdReplay::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IScdReplay
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

void CScdReplay::FireTheEvent(long Evt, long Data)
  {
  switch (Evt)
    {
    case ComCmd_NULL : /*Fire_On...*/ ; break;
    };
  };
