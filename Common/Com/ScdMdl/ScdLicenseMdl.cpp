// ScdLicenseMdl.cpp : Implementation of CScdLicenseMdl
#include "stdafx.h"
#include "ScdMdl.h"
#include "ScdLicenseMdl.h"
#include ".\scdlicensemdl.h"

/////////////////////////////////////////////////////////////////////////////
// CScdLicenseMdl

STDMETHODIMP CScdLicenseMdl::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IScdLicenseMdl
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

//void CScdLicenseMdl::FireTheEvent(long Evt, long Data)
//  {
//  switch (e)
//    {
//    case ComCmd_LoadProject:  
//      Fire_OnProjectOpened(l); 
//      break;
//    };
//  };

STDMETHODIMP CScdLicenseMdl::Dummy(void)
  {
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your implementation code here

  return S_OK;
  }
