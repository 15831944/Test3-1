// ScdDDEServer.cpp : Implementation of CScdDDEServer

#include "stdafx.h"
#include "ScdDDEServer.h"
#include ".\scdddeserver.h"


// CScdDDEServer

STDMETHODIMP CScdDDEServer::InterfaceSupportsErrorInfo(REFIID riid)
  {
  static const IID* arr[] = 
    {
    &IID_IScdDDEServer
    };

  for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
    {
    if (InlineIsEqualGUID(*arr[i],riid))
      return S_OK;
    }
  return S_FALSE;
  }

void CScdDDEServer::FireTheEvent(long Evt, long Data)
  {
  switch (Evt)
    {
    case ComCmd_NULL : /*Fire_On...*/ ; break;
    };
  };

STDMETHODIMP CScdDDEServer::get_On(VARIANT_BOOL* pVal)
  {
  AFX_MANAGE_STATE(AfxGetAppModuleState());

  // TODO: Add your implementation code here

  return E_NOTIMPL;
  }

STDMETHODIMP CScdDDEServer::put_On(VARIANT_BOOL newVal)
  {
  AFX_MANAGE_STATE(AfxGetAppModuleState());

  // TODO: Add your implementation code here

  return E_NOTIMPL;
  }
