// ScdSlvTag.cpp : Implementation of CScdTag
#include "stdafx.h"
#include "ScdSlv.h"
#include "ScdSlvTag.h"

/////////////////////////////////////////////////////////////////////////////
// CScdSlvTag

STDMETHODIMP CScdSlvTag::InterfaceSupportsErrorInfo(REFIID riid)
  {
  static const IID* arr[] = 
    {
    &IID_IScdSlvTag
    };
  for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
    {
    if (InlineIsEqualGUID(*arr[i],riid))
      return S_OK;
    }
  return S_FALSE;
  }

