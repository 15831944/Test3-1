// ScdQualities.cpp : Implementation of CScdQualities

#include "stdafx.h"
#include "ScdQualities.h"


// CScdQualities

STDMETHODIMP CScdQualities::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IScdQualities
	};

	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}
