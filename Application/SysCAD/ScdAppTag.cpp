// ScdAppTag.cpp : Implementation of CScppdATag
#include "stdafx.h"
#include "SysCAD_i.h"
#include "ScdAppTag.h"

/////////////////////////////////////////////////////////////////////////////
// CScdAppTag

STDMETHODIMP CScdAppTag::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IScdAppTag
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

void CScdAppTag::FireTheEvent(long Evt, long Data)
  {
  switch (Evt)
    {
    case ComCmd_NULL : /*Fire_On...*/ ; break;
    };
  };
