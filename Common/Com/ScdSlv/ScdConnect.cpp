// ScdConnect.cpp : Implementation of CScdConnect
#include "stdafx.h"
#include "ScdSlv.h"
#include "ScdConnect.h"
#include "flwsolve.h"
#include "scdslvwrap.h"

/////////////////////////////////////////////////////////////////////////////
// CScdConnect

STDMETHODIMP CScdConnect::InterfaceSupportsErrorInfo(REFIID riid)
  {
  static const IID* arr[] = 
    {
    &IID_IScdConnect
    };
  for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
    {
    if (InlineIsEqualGUID(*arr[i],riid))
      return S_OK;
    }
  return S_FALSE;
  }

STDMETHODIMP CScdConnect::SetObjectInfo(DWORD newVal, long Index)
  {
  dllSCD_COMENTRY(eScdNodeCodes)
    {
    CHECK_SFE_OK;

    m_pNode=(FlwNode*)newVal;
    m_lIndex=Index;
    m_lTopoCount=gs_pSfeSrvr->FE_GetTopologyCount()-1; // force array build
    }		
  SCD_COMEXIT
  }

STDMETHODIMP CScdConnect::get_Index(long *pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    *pVal=m_lIndex;
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdConnect::get_RmtIndex(long *pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    *pVal=m_pNode->IOIONo_Rmt(m_lIndex);
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdConnect::get_Node(IScdNode **pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    IScdNode* p;
    ::CoCreateInstance(CLSID_ScdNode, NULL, CLSCTX_ALL, IID_IScdNode, (void**)&p);
    p->SetObjectInfo((DWORD)m_pNode);
    *pVal=p; // User must release
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdConnect::get_RmtNode(IScdNode **pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    IScdNode* p;
    ::CoCreateInstance(CLSID_ScdNode, NULL, CLSCTX_ALL, IID_IScdNode, (void**)&p);
    p->SetObjectInfo((DWORD)m_pNode->Nd_Rmt(m_lIndex));
    *pVal=p; // User must release
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdConnect::get_ConnectString(BSTR *pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    Strng T;
    T=m_pNode->Tag();
    T+=".";
    T+=m_pNode->IODesc_Self(m_lIndex)->pName;
    SCD_Copy2BSTR(pVal, T());
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdConnect::get_RmtConnectString(BSTR *pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    Strng T;
    T=m_pNode->Nd_Rmt(m_lIndex)->Tag();
    T+=".";
    T+=m_pNode->IODesc_Rmt(m_lIndex)->pName;
    SCD_Copy2BSTR(pVal, T());
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdConnect::get_ConnectPoint(IScdConnectPoint **pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    IScdConnectPoint * p;
    ::CoCreateInstance(CLSID_ScdConnectPoint, NULL, CLSCTX_ALL, IID_IScdConnectPoint, (void**)&p);
    int i=0;
    int Id=m_pNode->IOId_Self(m_lIndex);
    IOAreaRec * pRec=m_pNode->IOAreas; 
    while (pRec && pRec->pName && pRec->m_Id!=Id) 
      { pRec++; i++; };
    p->SetObjectInfo((DWORD)m_pNode, i);
    *pVal=p; // User must release
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdConnect::get_RmtConnectPoint(IScdConnectPoint **pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    IScdConnectPoint * p;
    ::CoCreateInstance(CLSID_ScdConnectPoint, NULL, CLSCTX_ALL, IID_IScdConnectPoint, (void**)&p);
    int i=0;
    int Id=m_pNode->IOId_Rmt(m_lIndex);
    IOAreaRec * pRec=m_pNode->Nd_Rmt(m_lIndex)->IOAreas; 
    while (pRec && pRec->pName && pRec->m_Id!=Id) 
      { pRec++; i++; };
    p->SetObjectInfo((DWORD)m_pNode, i);
    *pVal=p; // User must release
    }
  SCD_COMEXIT
  }
