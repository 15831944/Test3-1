// ScdWire.cpp : Implementation of CScdWire

#include "stdafx.h"
#include "ScdWire.h"
#include "flwnode.h"
#include ".\scdwire.h"


// CScdWire

STDMETHODIMP CScdWire::InterfaceSupportsErrorInfo(REFIID riid)
  {
  static const IID* arr[] = 
    {
    &IID_IScdWire
    };

  for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
    {
    if (InlineIsEqualGUID(*arr[i],riid))
      return S_OK;
    }
  return S_FALSE;
  }

STDMETHODIMP CScdWire::SetObjectInfo(DWORD  Node, DWORD  Wire)
  {
  dllSCD_COMENTRY(eScdNodeCodes)
    {
    CHECK_SFE_OK;
    //m_eWhat=(eWhatNodes)What;
    m_pWire=(CEWire*)Wire;
    m_pNode=(FlwNode*)Node;
    m_lTopoCount=gs_pSfeSrvr->FE_GetTopologyCount();
    m_lElectricsTopoCount=-1;
    BuildData();
    }		
  SCD_COMEXIT
  }

STDMETHODIMP CScdWire::get_Tag(BSTR* pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_ELEC_TOPO_OK;
    SCD_Copy2BSTR(pVal, m_pWire->m_sTag);
    }
  SCD_COMEXIT
  }

void CScdWire::BuildData()
  {
  long TopoCnt=gs_pSfeSrvr->FE_GetTopologyCount();
  long ElectricsTopoCount=m_pNode->GetElectricsTopoCount();
  if (m_lElectricsTopoCount!=ElectricsTopoCount)
    {
    m_lTopoCount=TopoCnt;
    m_lElectricsTopoCount=ElectricsTopoCount;
    }
  //return 0;
  }
STDMETHODIMP CScdWire::get_Node(IScdNode** pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_ELEC_TOPO_OK;

    IScdNode* p;
    ::CoCreateInstance(CLSID_ScdNode, NULL, CLSCTX_ALL, IID_IScdNode, (void**)&p);
    p->SetObjectInfo((DWORD)m_pNode);
    *pVal=p;
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdWire::get_SrcNode(IScdNode** pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_ELEC_TOPO_OK;

    IScdNode* p;
    ::CoCreateInstance(CLSID_ScdNode, NULL, CLSCTX_ALL, IID_IScdNode, (void**)&p);
    p->SetObjectInfo((DWORD)m_pWire->m_pSrcTerm->m_pTStrip->m_pNd);
    *pVal=p;
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdWire::get_DstNode(IScdNode** pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_ELEC_TOPO_OK;

    IScdNode* p;
    ::CoCreateInstance(CLSID_ScdNode, NULL, CLSCTX_ALL, IID_IScdNode, (void**)&p);
    p->SetObjectInfo((DWORD)m_pWire->m_pDstTerm->m_pTStrip->m_pNd);
    *pVal=p;
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdWire::get_SrcTermStrip(IScdTermStrip** pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_ELEC_TOPO_OK;

    IScdTermStrip* p;
    ::CoCreateInstance(CLSID_ScdTermStrip, NULL, CLSCTX_ALL, IID_IScdTermStrip, (void**)&p);
    p->SetObjectInfo((DWORD)m_pWire->m_pSrcTerm->m_pTStrip, eScdElecType_All, eScdDirection_All);
    *pVal=p;
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdWire::get_DstTermStrip(IScdTermStrip** pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_ELEC_TOPO_OK;

    IScdTermStrip* p;
    ::CoCreateInstance(CLSID_ScdTermStrip, NULL, CLSCTX_ALL, IID_IScdTermStrip, (void**)&p);
    p->SetObjectInfo((DWORD)m_pWire->m_pDstTerm->m_pTStrip, eScdElecType_All, eScdDirection_All);
    *pVal=p;
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdWire::get_SrcTerminal(IScdTerminal** pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_ELEC_TOPO_OK;

    IScdTerminal* p;
    ::CoCreateInstance(CLSID_ScdTerminal, NULL, CLSCTX_ALL, IID_IScdTerminal, (void**)&p);
    p->SetObjectInfo((DWORD)m_pWire->m_pSrcTerm);
    *pVal=p;
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdWire::get_DstTerminal(IScdTerminal** pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_ELEC_TOPO_OK;

    IScdTerminal* p;
    ::CoCreateInstance(CLSID_ScdTerminal, NULL, CLSCTX_ALL, IID_IScdTerminal, (void**)&p);
    p->SetObjectInfo((DWORD)m_pWire->m_pDstTerm);
    *pVal=p;
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdWire::get_Component(IScdComponent** pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_ELEC_TOPO_OK;

    IScdComponent* p;
    ::CoCreateInstance(CLSID_ScdComponent, NULL, CLSCTX_ALL, IID_IScdComponent, (void**)&p);
    p->SetObjectInfo((DWORD)m_pNode, (DWORD)&m_pWire->m_CompData);
    *pVal=p;
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdWire::get_Area(DOUBLE* pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_ELEC_TOPO_OK;
    *pVal=m_pWire->m_Area;
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdWire::put_Area(DOUBLE newVal)
  {
  dllSCD_COMENTRY(eScdNodeCodes)
    {
    CHECK_ELEC_TOPO_OK;
    m_pWire->m_Area=newVal;
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdWire::get_Length(DOUBLE* pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_ELEC_TOPO_OK;
    *pVal=m_pWire->m_Length;
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdWire::put_Length(DOUBLE newVal)
  {
  dllSCD_COMENTRY(eScdNodeCodes)
    {
    CHECK_ELEC_TOPO_OK;
    m_pWire->m_Length=newVal;
    }
  SCD_COMEXIT
  }

