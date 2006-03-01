// ScdMessage.cpp : Implementation of CScdMessage
#include "stdafx.h"
#include "ScdSlv.h"
#include "ScdMessage.h"

/////////////////////////////////////////////////////////////////////////////
// CScdMessage

STDMETHODIMP CScdMessage::InterfaceSupportsErrorInfo(REFIID riid)
  {
  static const IID* arr[] = 
    {
    &IID_IScdMessage
    };
  for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
    {
    if (InlineIsEqualGUID(*arr[i],riid))
      return S_OK;
    }
  return S_FALSE;
  }
