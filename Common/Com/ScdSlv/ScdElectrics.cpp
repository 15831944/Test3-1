// ScdElectrics.cpp : Implementation of CScdElectrics

#include "stdafx.h"
#include "ScdElectrics.h"
#include ".\scdelectrics.h"


// CScdElectrics

STDMETHODIMP CScdElectrics::InterfaceSupportsErrorInfo(REFIID riid)
  {
  static const IID* arr[] = 
    {
    &IID_IScdElectrics
    };

  for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
    {
    if (InlineIsEqualGUID(*arr[i],riid))
      return S_OK;
    }
  return S_FALSE;
  }

STDMETHODIMP CScdElectrics::SetObjectInfo(DWORD newVal)
  {
  dllSCD_COMENTRY(eScdNodeCodes)
    {
    CHECK_SFE_OK;
    m_pNode=(FlwNode*)newVal;
    m_lTopoCount=gs_pSfeSrvr->FE_GetTopologyCount();
    }		
  SCD_COMEXIT
  }

STDMETHODIMP CScdElectrics::get_TermStrips(eScdElecTypes Types, eScdDirections Direction, IScdTermStrips** pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_TOPO_OK;

    IScdTermStrips * p;
    ::CoCreateInstance(CLSID_ScdTermStrips, NULL, CLSCTX_ALL, IID_IScdTermStrips, (void**)&p);
    p->SetObjectInfo((DWORD)m_pNode, Types, Direction);
    *pVal=p; // User must release
    return S_OK;
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdElectrics::get_Wires(eScdElecTypes Types, IScdWires** pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_TOPO_OK;

    IScdWires * p;
    ::CoCreateInstance(CLSID_ScdWires, NULL, CLSCTX_ALL, IID_IScdWires, (void**)&p);
    p->SetObjectInfo((DWORD)m_pNode, 0, Types);
    *pVal=p; // User must release
    return S_OK;
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdElectrics::get_Terminals(eScdElecTypes Types, IScdTerminals** pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_TOPO_OK;

    IScdTerminals * p;
    ::CoCreateInstance(CLSID_ScdTerminals, NULL, CLSCTX_ALL, IID_IScdTerminals, (void**)&p);
    p->SetObjectInfo((DWORD)m_pNode, 0, Types);
    *pVal=p; // User must release
    return S_OK;
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdElectrics::get_Components(eScdElecTypes Types, IScdComponents** pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_TOPO_OK;

    IScdComponents * p;
    ::CoCreateInstance(CLSID_ScdComponents, NULL, CLSCTX_ALL, IID_IScdComponents, (void**)&p);
    p->SetObjectInfo((DWORD)m_pNode, Types);
    *pVal=p; // User must release
    return S_OK;
    }
  SCD_COMEXIT
  }
