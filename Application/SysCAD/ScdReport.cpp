// ScdReport.cpp : Implementation of CScdReport
#include "stdafx.h"
#include "SysCAD_i.h"
#include "ScdReport.h"

/////////////////////////////////////////////////////////////////////////////
// CScdReport

STDMETHODIMP CScdReport::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IScdReport
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

void CScdReport::FireTheEvent(long Evt, long Data)
  {
  switch (Evt)
    {
    case ComCmd_NULL : /*Fire_On...*/ ; break;
    };
  };
