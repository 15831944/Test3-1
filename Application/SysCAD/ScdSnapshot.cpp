// ScdSnapshot.cpp : Implementation of CScdSnapshot
#include "stdafx.h"
#include "SysCAD_i.h"
#include "ScdSnapshot.h"

/////////////////////////////////////////////////////////////////////////////
// CScdSnapshot

STDMETHODIMP CScdSnapshot::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IScdSnapshot
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

void CScdSnapshot::FireTheEvent(long Evt, long Data)
  {
  switch (Evt)
    {
    case ComCmd_NULL : /*Fire_On...*/ ; break;
    };
  };
