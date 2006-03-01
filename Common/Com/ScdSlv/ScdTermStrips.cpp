// ScdTermStrips.cpp : Implementation of CScdTermStrips

#include "stdafx.h"
#include "ScdTermStrips.h"
#include "flwnode.h"


// CScdTermStrips

STDMETHODIMP CScdTermStrips::InterfaceSupportsErrorInfo(REFIID riid)
  {
  static const IID* arr[] = 
    {
    &IID_IScdTermStrips
    };

  for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
    {
    if (InlineIsEqualGUID(*arr[i],riid))
      return S_OK;
    }
  return S_FALSE;
  }

STDMETHODIMP CScdTermStrips::TermStrip( VARIANT  WhichOne,  IScdTermStrip * *  pItem)
  {
  dllSCD_COMENTRY(eScdNodeCodes)
    {
    CHECK_ELEC_TOPO_OK;

    switch (WhichOne.vt)
      {
      case VT_I2:
      case VT_I4:
        {
        long N=m_TermStrips.GetCount();//BuildData();
        long i=(WhichOne.vt==VT_I2) ? WhichOne.iVal : WhichOne.lVal;
        if (i>=0 && i<N)
          {
          IScdTermStrip * p;
          ::CoCreateInstance(CLSID_ScdTermStrip, NULL, CLSCTX_ALL, IID_IScdTermStrip, (void**)&p);
          p->SetObjectInfo((DWORD)m_TermStrips[i], eScdElecType_All, eScdDirection_All);
          *pItem=p; // User must release
          return S_OK;
          }
        return Scd.ReturnH(E_INVALIDARG, "Bad Index");
        }
      case VT_BSTR:
        {
        SCD_LclStr(Name, WhichOne.bstrVal);
        for (int i=0; i<m_TermStrips.GetCount(); i++)
          {
          if (stricmp(m_TermStrips[i]->m_sName, Name)==0)
            {
            IScdTermStrip * p;
            ::CoCreateInstance(CLSID_ScdTermStrip, NULL, CLSCTX_ALL, IID_IScdTermStrip, (void**)&p);
            p->SetObjectInfo((DWORD)m_TermStrips[i], eScdElecType_All, eScdDirection_All);
            *pItem=p; // User must release
            return S_OK;
            }
          }
        return Scd.ReturnH(E_INVALIDARG, "Bad Tag");
        }
      default:
        return Scd.ReturnH(E_INVALIDARG);
      }
    *pItem=NULL;
    }		
  SCD_COMEXIT
  }
STDMETHODIMP CScdTermStrips::get__NewEnum( LPUNKNOWN *  pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_ELEC_TOPO_OK;

    long size=m_TermStrips.GetCount();

    VARIANT* pVar = new VARIANT[size];
    for(long i = 0; i < size; i++)
      {
      pVar[i].vt = VT_DISPATCH;
      pVar[i].pdispVal = m_ITermStrips[i];
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
STDMETHODIMP CScdTermStrips::get_Count( long *  pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_ELEC_TOPO_OK;

    *pVal=m_TermStrips.GetCount();
    }		
  SCD_COMEXIT
  }
STDMETHODIMP CScdTermStrips::SetObjectInfo( DWORD  Node,  DWORD  Types,  DWORD  Direction)
  {
  dllSCD_COMENTRY(eScdNodeCodes)
    {
    CHECK_SFE_OK;

    m_pNode=(FlwNode*)Node;
    m_eTypes=(eScdElecTypes)Types;
    m_eDirns=(eScdDirections)Direction;
    m_lTopoCount=gs_pSfeSrvr->FE_GetTopologyCount();
    m_lElectricsTopoCount=-1;
    BuildData();
    }		
  SCD_COMEXIT
  }
STDMETHODIMP CScdTermStrips::Select( eScdElecTypes  Types, eScdDirections  Direction)
  {
  dllSCD_COMENTRY(eScdNodeCodes)
    {
    m_eTypes=(eScdElecTypes)Types;
    m_eDirns=(eScdDirections)Direction;
    }		
  SCD_COMEXIT
  }
void CScdTermStrips::BuildData()
  {
  long TopoCnt=gs_pSfeSrvr->FE_GetTopologyCount();
  long ElectricsTopoCount=m_pNode->GetElectricsTopoCount();
  if (m_lElectricsTopoCount!=ElectricsTopoCount)
    {
    int size=0;
    m_lTopoCount=TopoCnt;
    m_lElectricsTopoCount=ElectricsTopoCount;

    m_pNode->UpdateElectrics();
    CNodeElectrics & E=m_pNode->m_Electrics;
    
    m_TermStrips.SetSize(0);
    E.m_TermStrips.GetCount();
    for (int i=0; i<E.m_TermStrips.GetCount(); i++)
      {
      CETermStrip * p=E.m_TermStrips[i];
      m_TermStrips.SetAtGrow(size++, p);
      }
    for (int i = 0; i<m_ITermStrips.GetSize(); i++)
      m_ITermStrips[i]->Release();

    m_ITermStrips.SetSize(size);
    for (int i = 0; i < size; i++)
      {
      IScdTermStrip * p;
      ::CoCreateInstance(CLSID_ScdTermStrip, NULL, CLSCTX_ALL, IID_IScdTermStrip, (void**)&p);
      p->SetObjectInfo((DWORD)m_TermStrips[i], eScdElecType_All, eScdDirection_All);
      LPDISPATCH pDisp;
      SCD_TOF(p->QueryInterface(IID_IDispatch, (void**)&pDisp), "IDispatch on ScdTermStrip")
        m_ITermStrips[i]=pDisp;
      p->Release();
      }
    }
  //ASSERT(m_ITermStrips.GetSize()==m_FlwNodes.GetSize());
  //return m_ITermStrips.GetSize();
  }
