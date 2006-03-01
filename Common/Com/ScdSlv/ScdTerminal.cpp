// ScdTerminal.cpp : Implementation of CScdTerminal

#include "stdafx.h"
#include "ScdTerminal.h"
#include "flwnode.h"
#include ".\scdterminal.h"


// CScdTerminal

STDMETHODIMP CScdTerminal::InterfaceSupportsErrorInfo(REFIID riid)
  {
  static const IID* arr[] = 
    {
    &IID_IScdTerminal
    };

  for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
    {
    if (InlineIsEqualGUID(*arr[i],riid))
      return S_OK;
    }
  return S_FALSE;
  }

STDMETHODIMP CScdTerminal::SetObjectInfo(DWORD  Terminal)
  {
  dllSCD_COMENTRY(eScdNodeCodes)
    {
    CHECK_SFE_OK;
    //m_eWhat=(eWhatNodes)What;
    m_pTerminal=(CETerminal*)Terminal;
    m_pNode=dynamic_cast<FlwNode*>(m_pTerminal->m_pTStrip->m_pNd);
    m_lTopoCount=gs_pSfeSrvr->FE_GetTopologyCount();
    m_lElectricsTopoCount=-1;
    BuildData();
    }		
  SCD_COMEXIT
  }

STDMETHODIMP CScdTerminal::get_Tag(BSTR* pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_ELEC_TOPO_OK;
    SCD_Copy2BSTR(pVal, m_pTerminal->m_sName);
    }
  SCD_COMEXIT
  }

void CScdTerminal::BuildData()
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
STDMETHODIMP CScdTerminal::get_TermStrip(IScdTermStrip** pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_ELEC_TOPO_OK;

    IScdTermStrip * p;
    ::CoCreateInstance(CLSID_ScdTermStrip, NULL, CLSCTX_ALL, IID_IScdTermStrip, (void**)&p);
    p->SetObjectInfo((DWORD)m_pTerminal->m_pTStrip, eScdElecType_All, eScdDirection_All);
    *pVal=p;
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdTerminal::get_Node(IScdNode** pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_ELEC_TOPO_OK;

    IScdNode * p;
    ::CoCreateInstance(CLSID_ScdNode, NULL, CLSCTX_ALL, IID_IScdNode, (void**)&p);
    p->SetObjectInfo((DWORD)m_pTerminal->m_pTStrip->m_pNd);
    *pVal=p;
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdTerminal::get_Voltage(IScdComplex** pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_ELEC_TOPO_OK;

    IScdComplex * p;
    ::CoCreateInstance(CLSID_ScdComplex, NULL, CLSCTX_ALL, IID_IScdComplex, (void**)&p);
    p->put_Real(m_pTerminal->m_V.real());
    p->put_Imag(m_pTerminal->m_V.imag());
    *pVal=p;
    }
  SCD_COMEXIT
  }

//STDMETHODIMP CScdTerminal::get_Current(IScdComplex** pVal)
//  {
//  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
//    {
//    CHECK_ELEC_TOPO_OK;
//
//    IScdComplex * p;
//    ::CoCreateInstance(CLSID_ScdComplex, NULL, CLSCTX_ALL, IID_IScdComplex, (void**)&p);
//    p->put_Real(m_pTerminal->m_I.real());
//    p->put_Imag(m_pTerminal->m_I.imag());
//    *pVal=p;
//    }
//  SCD_COMEXIT
//  }

