// ScdIOMarshal.cpp : Implementation of CScdIOMarshal

#include "stdafx.h"
#include "ScdIOMarshal.h"
#include ".\scdiomarshal.h"
#include ".\project.h"


// CScdIOMarshal

STDMETHODIMP CScdIOMarshal::InterfaceSupportsErrorInfo(REFIID riid)
  {
  static const IID* arr[] = 
    {
    &IID_IScdIOMarshal
    };

  for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
    {
    if (InlineIsEqualGUID(*arr[i],riid))
      return S_OK;
    }
  return S_FALSE;
  }

void CScdIOMarshal::FireTheEvent(long Evt, long Data)
  {
  switch (Evt)
    {
    case ComCmd_NULL : /*Fire_On...*/ ; break;
    };
  };

STDMETHODIMP CScdIOMarshal::get_On(VARIANT_BOOL* pVal)
  {
  appSCD_COMENTRYGET(long, pVal)
    {
    *pVal=gs_pPrj->bIOMOpen;
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdIOMarshal::put_On(VARIANT_BOOL newVal)
  {
  appSCD_COMENTRY(long)
    {
    if (newVal && !gs_pPrj->bIOMOpen)
      gs_pPrj->OpenIOMarshal();
    else if (!newVal && gs_pPrj->bIOMOpen)
      gs_pPrj->CloseIOMarshal(false, false);
    }
  SCD_COMEXIT
  }

