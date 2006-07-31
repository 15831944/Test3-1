// ScdOPCServer.cpp : Implementation of CScdOPCServer

#include "stdafx.h"
#include "ScdOPCServer.h"
#include ".\scdopcserver.h"
#include ".\opcsrvrwrapper.h"
#include "Project.h"

// CScdOPCServer

STDMETHODIMP CScdOPCServer::InterfaceSupportsErrorInfo(REFIID riid)
  {
  static const IID* arr[] = 
    {
    &IID_IScdOPCServer
    };

  for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
    {
    if (InlineIsEqualGUID(*arr[i],riid))
      return S_OK;
    }
  return S_FALSE;
  }

void CScdOPCServer::FireTheEvent(long Evt, long Data)
  {
  switch (Evt)
    {
    case ComCmd_NULL : /*Fire_On...*/ ; break;
    };
  };


STDMETHODIMP CScdOPCServer::get_On(VARIANT_BOOL* pVal)
  {
  appSCD_COMENTRYGET(long, pVal)
    {
    *pVal=GetMainOPCSrvrEnable() && gs_pPrj->m_bOPCOn;
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdOPCServer::put_On(VARIANT_BOOL newVal)
  {
  appSCD_COMENTRY(long)
    {
    DisableMainOPCSrvr();
    gs_pPrj->m_bOPCOn = (newVal!=0);
    EnableMainOPCSrvr(gs_pPrj->m_bOPCOn!=0);
    }
  SCD_COMEXIT
  }

