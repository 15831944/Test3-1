// ScdTermStrip.cpp : Implementation of CScdTermStrip

#include "stdafx.h"
#include "ScdTermStrip.h"
#include "flwnode.h"
#include ".\scdtermstrip.h"


// CScdTermStrip

STDMETHODIMP CScdTermStrip::InterfaceSupportsErrorInfo(REFIID riid)
  {
  static const IID* arr[] = 
    {
    &IID_IScdTermStrip
    };

  for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
    {
    if (InlineIsEqualGUID(*arr[i],riid))
      return S_OK;
    }
  return S_FALSE;
  }

STDMETHODIMP CScdTermStrip::Terminal( VARIANT  WhichOne,  IScdTerminal * *  pItem)
  {
  // Add your function implementation here.
  return E_NOTIMPL;
  }
STDMETHODIMP CScdTermStrip::get__NewEnum( LPUNKNOWN *  pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_ELEC_TOPO_OK;

    long size=m_ITerminals.GetSize();
    VARIANT* pVar = new VARIANT[size];
    for(long i = 0; i < size; i++)
      {
      pVar[i].vt = VT_DISPATCH;
      pVar[i].pdispVal = m_ITerminals[i];
      }

    // Now make the enum.
    typedef CComObject< CComEnum< IEnumVARIANT, &IID_IEnumVARIANT, VARIANT, _Copy< VARIANT > > > enumVar;
    enumVar* pEnumVar = new enumVar;
    pEnumVar->Init(&pVar[0], &pVar[size], NULL, AtlFlagCopy);
    delete[] pVar;
    // return the enum.
    return pEnumVar->QueryInterface(IID_IUnknown, (void**)pVal);
    //Scd.ReturnH(E_NOTIMPL);
    }		
  SCD_COMEXIT
  }
STDMETHODIMP CScdTermStrip::get_Count( long *  pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_ELEC_TOPO_OK;
    *pVal=m_ITerminals.GetSize();
    }		
  SCD_COMEXIT
  }
STDMETHODIMP CScdTermStrip::SetObjectInfo(DWORD  TermStrip,  DWORD  Types,  DWORD  Dirn)
  {
  dllSCD_COMENTRY(eScdNodeCodes)
    {
    CHECK_SFE_OK;

    //m_eWhat=(eWhatNodes)What;
    m_pTermStrip  = (CETermStrip*)TermStrip;
    m_pNode       = dynamic_cast<FlwNode*>(m_pTermStrip->m_pNd);
    m_eTypes      = (eScdElecTypes)Types;
    m_eDirns      = (eScdDirections)Dirn;
    m_lTopoCount  = gs_pSfeSrvr->FE_GetTopologyCount();
    m_lElectricsTopoCount=-1;
    BuildData();
    }		
  SCD_COMEXIT
  }
STDMETHODIMP CScdTermStrip::Select( eScdElecTypes  Types, eScdDirections  Direction)
  {
  dllSCD_COMENTRY(eScdNodeCodes)
    {
    m_eTypes=(eScdElecTypes)Types;
    m_eDirns=(eScdDirections)Direction;
    }		
  SCD_COMEXIT
  }
STDMETHODIMP CScdTermStrip::get_Tag(BSTR* pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_ELEC_TOPO_OK;

    SCD_Copy2BSTR(pVal, m_pTermStrip->m_sName);
    }
  SCD_COMEXIT
  }

void CScdTermStrip::BuildData()
  {
  long TopoCnt=gs_pSfeSrvr->FE_GetTopologyCount();
  long ElectricsTopoCount=m_pNode->GetElectricsTopoCount();
  if (m_lElectricsTopoCount!=ElectricsTopoCount)
    {
    int size=0;
    m_lTopoCount=TopoCnt;
    m_lElectricsTopoCount=ElectricsTopoCount;

    m_pNode->UpdateElectrics();
    CETermStrip & TS=*m_pTermStrip;
    
    m_Terminals.SetSize(0);
    for (int i=0; i<TS.m_Terms.GetCount(); i++)
      {
      CETerminal * p=TS.m_Terms[i];
      m_Terminals.SetAtGrow(size++, p);
      }
    for (int i = 0; i<m_ITerminals.GetSize(); i++)
      m_ITerminals[i]->Release();

    m_ITerminals.SetSize(size);
    for (int i = 0; i < size; i++)
      {
      IScdTerminal * p;
      ::CoCreateInstance(CLSID_ScdTerminal, NULL, CLSCTX_ALL, IID_IScdTerminal, (void**)&p);
      p->SetObjectInfo((DWORD)m_Terminals[i]);
      LPDISPATCH pDisp;
      SCD_TOF(p->QueryInterface(IID_IDispatch, (void**)&pDisp), "IDispatch on ScdTermStrip")
        m_ITerminals[i]=pDisp;
      p->Release();
      }
    }
  //ASSERT(m_ITerminals.GetSize()==m_FlwNodes.GetSize());
  //return m_ITerminals.GetSize();
  }

STDMETHODIMP CScdTermStrip::get_Node(IScdNode** pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_ELEC_TOPO_OK;

    IScdNode * p;
    ::CoCreateInstance(CLSID_ScdNode, NULL, CLSCTX_ALL, IID_IScdNode, (void**)&p);
    p->SetObjectInfo((DWORD)m_pNode);
    *pVal=p;
    }
  SCD_COMEXIT
  }
