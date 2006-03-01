// ScdTest.cpp : Implementation of CScdTest
#include "stdafx.h"
#include "ScdSlv.h"
#include "ScdTest.h"

/////////////////////////////////////////////////////////////////////////////
// CScdTest

STDMETHODIMP CScdTest::InterfaceSupportsErrorInfo(REFIID riid)
  {
  static const IID* arr[] = 
    {
    &IID_IScdTest
    };
  for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
    {
    if (InlineIsEqualGUID(*arr[i],riid))
      return S_OK;
    }
  return S_FALSE;
  }


STDMETHODIMP CScdTest::ReturnHResult(long Sev, long Code)
  {
  dllSCD_COMENTRY(long)
    {
    // RETURNONFAIL(MAKE_HRESULT(Sev, FACILITY_ITF, Code));
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdTest::ThrowException(long Sev, long Code)
  {
  dllSCD_COMENTRY(long)
    {
    SCD_TOF(MAKE_HRESULT(Sev, FACILITY_ITF, Code), "CScdTest::ThrowException");
    }
  SCD_COMEXIT
  }
