// ScdQuality.cpp : Implementation of CScdQuality

#include "stdafx.h"
#include "ScdQuality.h"


// CScdQuality

STDMETHODIMP CScdQuality::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IScdQuality
	};

	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}
