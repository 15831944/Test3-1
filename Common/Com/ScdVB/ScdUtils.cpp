// ScdUtils.cpp : Implementation of CScdUtils

#include "stdafx.h"
#include "ScdUtils.h"
#include ".\scdutils.h"


// CScdUtils


STDMETHODIMP CScdUtils::Sleep(LONG PeriodMS)
  {
  try
    {
    ::Sleep(PeriodMS);
    }
  catch(...)
    {
    return E_FAIL;
    }
  return S_OK;
  }
