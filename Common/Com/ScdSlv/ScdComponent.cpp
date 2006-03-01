// ScdComponent.cpp : Implementation of CScdComponent

#include "stdafx.h"
#include "ScdComponent.h"


// CScdComponent

STDMETHODIMP CScdComponent::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IScdComponent
	};

	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP CScdComponent::SetObjectInfo(DWORD Node, DWORD  Component)
  {
  dllSCD_COMENTRY(eScdNodeCodes)
    {
    CHECK_SFE_OK;
    //m_eWhat=(eWhatNodes)What;
    m_pComponent=(CEComponent*)Component;
    m_pNode=(FlwNode*)Node;
    m_lTopoCount=gs_pSfeSrvr->FE_GetTopologyCount();
    m_lElectricsTopoCount=-1;
    BuildData();
    }		
  SCD_COMEXIT
  }

void CScdComponent::BuildData()
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
STDMETHODIMP CScdComponent::get_SrcTerminal(IScdTerminal** pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_ELEC_TOPO_OK;

    IScdTerminal * p;
    ::CoCreateInstance(CLSID_ScdTerminal, NULL, CLSCTX_ALL, IID_IScdTerminal, (void**)&p);
    p->SetObjectInfo((DWORD)m_pComponent->m_pSrc);
    *pVal=p;
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdComponent::get_DstTerminal(IScdTerminal** pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_ELEC_TOPO_OK;

    IScdTerminal * p;
    ::CoCreateInstance(CLSID_ScdTerminal, NULL, CLSCTX_ALL, IID_IScdTerminal, (void**)&p);
    p->SetObjectInfo((DWORD)m_pComponent->m_pDst);
    *pVal=p;
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdComponent::get_Impedance(IScdComplex** pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_ELEC_TOPO_OK;

    IScdComplex * p;
    ::CoCreateInstance(CLSID_ScdComplex, NULL, CLSCTX_ALL, IID_IScdComplex, (void**)&p);
    p->put_Real(m_pComponent->m_pCompData->m_Z.real());
    p->put_Imag(m_pComponent->m_pCompData->m_Z.imag());
    *pVal=p;
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdComponent::get_Source(IScdComplex** pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_ELEC_TOPO_OK;

    IScdComplex * p;
    ::CoCreateInstance(CLSID_ScdComplex, NULL, CLSCTX_ALL, IID_IScdComplex, (void**)&p);
    p->put_Real(m_pComponent->m_pCompData->m_V.real());
    p->put_Imag(m_pComponent->m_pCompData->m_V.imag());
    *pVal=p;
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdComponent::get_Current(IScdComplex** pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_ELEC_TOPO_OK;

    IScdComplex * p;
    ::CoCreateInstance(CLSID_ScdComplex, NULL, CLSCTX_ALL, IID_IScdComplex, (void**)&p);
    p->put_Real(m_pComponent->m_pCompData->m_I.real());
    p->put_Imag(m_pComponent->m_pCompData->m_I.imag());
    *pVal=p;
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdComponent::get_Applied(IScdComplex** pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_ELEC_TOPO_OK;

    IScdComplex * p;
    ::CoCreateInstance(CLSID_ScdComplex, NULL, CLSCTX_ALL, IID_IScdComplex, (void**)&p);
    CDComplex AV=m_pComponent->m_pCompData->AppliedVoltage();
    p->put_Real(AV.real());
    p->put_Imag(AV.imag());
    *pVal=p;
    }
  SCD_COMEXIT
  }
