// ScdOptions.cpp : Implementation of CScdOptions
#include "stdafx.h"
#include "SysCAD_i.h"
#include "ScdOptions.h"

/////////////////////////////////////////////////////////////////////////////
// CScdOptions

STDMETHODIMP CScdOptions::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IScdOptions
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

void CScdOptions::FireTheEvent(long Evt, long Data)
  {
  switch (Evt)
    {
    case ComCmd_NULL : /*Fire_On...*/ ; break;
    };
  };
