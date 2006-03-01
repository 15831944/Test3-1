// ScdLicenseApp.cpp : Implementation of CScdLicense
#include "stdafx.h"
#include "SysCAD_i.h"
#include "ScdLicenseApp.h"

/////////////////////////////////////////////////////////////////////////////
// CScdLicenseApp

STDMETHODIMP CScdLicenseApp::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IScdLicenseApp
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

void CScdLicenseApp::FireTheEvent(long Evt, long Data)
  {
  switch (Evt)
    {
    case ComCmd_NULL : /*Fire_On...*/ ; break;
    };
  };
