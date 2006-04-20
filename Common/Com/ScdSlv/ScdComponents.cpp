// ScdComponents.cpp : Implementation of CScdComponents

#include "stdafx.h"
#include "ScdComponents.h"
#include "flwnode.h"


// CScdComponents

STDMETHODIMP CScdComponents::InterfaceSupportsErrorInfo(REFIID riid)
  {
  static const IID* arr[] = 
    {
    &IID_IScdComponents
    };

  for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
    {
    if (InlineIsEqualGUID(*arr[i],riid))
      return S_OK;
    }
  return S_FALSE;
  }

STDMETHODIMP CScdComponents::Component( VARIANT  WhichOne,  IScdComponent * *  pItem)
  {
  dllSCD_COMENTRY(eScdNodeCodes)
    {
    CHECK_ELEC_TOPO_OK;

    switch (WhichOne.vt)
      {
      case VT_I2:
      case VT_I4:
        {
        long N=m_IComponents.GetSize();
        long i=(WhichOne.vt==VT_I2) ? WhichOne.iVal : WhichOne.lVal;
        if (i>=0 && i<N)
          {
          IScdComponent * p;
          ::CoCreateInstance(CLSID_ScdComponent, NULL, CLSCTX_ALL, IID_IScdComponent, (void**)&p);
          p->SetObjectInfo((DWORD)m_pNode, (DWORD)m_Components[i]);
          *pItem=p; // User must release
          return S_OK;
          }
        return Scd.ReturnH(E_INVALIDARG, "Bad Index");
        }
      //case VT_BSTR:
      //  {
      //  SCD_LclStr(Name, WhichOne.bstrVal);
      //  for (int i=0; i<m_Components.GetSize(); i++)
      //    {
      //    if (_stricmp(m_Components[i]->, Name)==0)
      //      {
      //      IScdComponent * p;
      //      ::CoCreateInstance(CLSID_ScdComponent, NULL, CLSCTX_ALL, IID_IScdComponent, (void**)&p);
      //      p->SetObjectInfo((DWORD)m_pNode, (DWORD)m_Components[i]);
      //      *pItem=p; // User must release
      //      return S_OK;
      //      }
      //    }
      //  return Scd.ReturnH(E_INVALIDARG, "Bad Tag");
      //  }
      default:
        return Scd.ReturnH(E_INVALIDARG);
      }
    *pItem=NULL;
    }		
  SCD_COMEXIT
  }
STDMETHODIMP CScdComponents::get__NewEnum( LPUNKNOWN *  pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_ELEC_TOPO_OK;

    long size=m_IComponents.GetSize();

    VARIANT* pVar = new VARIANT[size];
    for(long i = 0; i < size; i++)
      {
      pVar[i].vt = VT_DISPATCH;
      pVar[i].pdispVal = m_IComponents[i];
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
STDMETHODIMP CScdComponents::get_Count( long *  pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_ELEC_TOPO_OK;

    *pVal=m_IComponents.GetSize();
    }		
  SCD_COMEXIT
  }
STDMETHODIMP CScdComponents::SetObjectInfo(DWORD  Node,  DWORD  Types)
  {
  dllSCD_COMENTRY(eScdNodeCodes)
    {
    CHECK_SFE_OK;
    m_pNode       = (FlwNode*)(Node);
    m_eTypes      = (eScdElecTypes)Types;
    //m_eDirns      = (eScdDirections)Dirn;
    m_lTopoCount  = gs_pSfeSrvr->FE_GetTopologyCount();
    m_lElectricsTopoCount=-1;
    BuildData();
    }		
  SCD_COMEXIT
  }
STDMETHODIMP CScdComponents::Select( eScdElecTypes  Types)
  {
  dllSCD_COMENTRY(eScdNodeCodes)
    {
    m_eTypes=(eScdElecTypes)Types;
    }		
  SCD_COMEXIT
  }
void CScdComponents::BuildData()
  {
  long TopoCnt=gs_pSfeSrvr->FE_GetTopologyCount();
  long ElectricsTopoCount=m_pNode->GetElectricsTopoCount();
  if (m_lElectricsTopoCount!=ElectricsTopoCount)
    {
    int size=0;
    m_lTopoCount=TopoCnt;
    m_lElectricsTopoCount=ElectricsTopoCount;

    m_pNode->UpdateElectrics();
    CEComponentPtrs & EC=m_pNode->m_Electrics.m_Components;
    
    m_Components.SetSize(0);
    for (int i=0; i<EC.GetCount(); i++)
      {
      CEComponent* p=EC[i];
      m_Components.SetAtGrow(size++, p);
      }
    for (int i = 0; i<m_IComponents.GetSize(); i++)
      m_IComponents[i]->Release();

    m_IComponents.SetSize(size);
    for (int i = 0; i < size; i++)
      {
      IScdComponent * p;
      ::CoCreateInstance(CLSID_ScdComponent, NULL, CLSCTX_ALL, IID_IScdComponent, (void**)&p);
      p->SetObjectInfo((DWORD)m_pNode, (DWORD)m_Components[i]);
      LPDISPATCH pDisp;
      SCD_TOF(p->QueryInterface(IID_IDispatch, (void**)&pDisp), "IDispatch on ScdComponents")
        m_IComponents[i]=pDisp;
      p->Release();
      }
    }
  //ASSERT(m_IComponents.GetSize()==m_FlwNodes.GetSize());
  //return m_IComponents.GetSize();
  };
