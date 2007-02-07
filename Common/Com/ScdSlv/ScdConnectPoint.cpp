// ScdConnectPoint.cpp : Implementation of CScdConnectPoint
#include "stdafx.h"
#include "ScdSlv.h"
#include "ScdConnectPoint.h"
#include "flwsolve.h"
#include "scdslvwrap.h"

/////////////////////////////////////////////////////////////////////////////
// CScdConnectPoint

STDMETHODIMP CScdConnectPoint::InterfaceSupportsErrorInfo(REFIID riid)
  {
  static const IID* arr[] = 
    {
    &IID_IScdConnectPoint
    };
  for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
    {
    if (InlineIsEqualGUID(*arr[i],riid))
      return S_OK;
    }
  return S_FALSE;
  }

STDMETHODIMP CScdConnectPoint::get_Name(BSTR *pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_TOPO_OK;
    SCD_Copy2BSTR(pVal, m_pNode->IOArea(m_lIndex).IOName());
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdConnectPoint::get_Description(BSTR *pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_TOPO_OK;
    SCD_Copy2BSTR(pVal, m_pNode->IOArea(m_lIndex).IODesc());
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdConnectPoint::get_Input(VARIANT_BOOL *pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_TOPO_OK;
    *pVal=(m_pNode->IOArea(m_lIndex).m_Dirn & LIO_In)!=0;
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdConnectPoint::get_Output(VARIANT_BOOL *pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_TOPO_OK;
    *pVal=(m_pNode->IOArea(m_lIndex).m_Dirn & LIO_Out)!=0;
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdConnectPoint::get_Type(eScdNodeTypes *pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_TOPO_OK;
    *pVal=(eScdNodeTypes)m_pNode->IOArea(m_lIndex).m_dwType;
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdConnectPoint::get_ConnectCountRqd(long *pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_TOPO_OK;
    *pVal=m_pNode->IOArea(m_lIndex).m_nIORqd;
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdConnectPoint::get_ConnectCountMax(long *pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_TOPO_OK;
    *pVal=m_pNode->IOArea(m_lIndex).m_nIOMax;
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdConnectPoint::SetObjectInfo(DWORD newVal, long Index)
  {
  dllSCD_COMENTRY(eScdNodeCodes)
    {
    CHECK_SFE_OK;
    m_pNode=(FlwNode*)newVal;
    m_lIndex=Index;
    m_lTopoCount=gs_pSfeSrvr->FE_GetTopologyCount();
    }		
  SCD_COMEXIT
  }

STDMETHODIMP CScdConnectPoint::get_TypeString(BSTR *pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_TOPO_OK;
    Strng T;
    SCD_Copy2BSTR(pVal, FlwNode::TypeString(m_pNode->IOArea(m_lIndex).m_dwType, T));
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdConnectPoint::get_Index(long *pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    *pVal=m_lIndex;
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdConnectPoint::get_ID(long *pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_TOPO_OK;
    *pVal=m_pNode->IOArea(m_lIndex).m_Id;
    }
  SCD_COMEXIT
  }
