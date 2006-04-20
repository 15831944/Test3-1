// ScdWires.cpp : Implementation of CScdWires

#include "stdafx.h"
#include "ScdWires.h"
#include "flwnode.h"
#include ".\scdwires.h"


// CScdWires

STDMETHODIMP CScdWires::InterfaceSupportsErrorInfo(REFIID riid)
  {
  static const IID* arr[] = 
    {
    &IID_IScdWires
    };

  for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
    {
    if (InlineIsEqualGUID(*arr[i],riid))
      return S_OK;
    }
  return S_FALSE;
  }

STDMETHODIMP CScdWires::Wire( VARIANT  WhichOne,  IScdWire * *  pItem)
  {
  dllSCD_COMENTRY(eScdNodeCodes)
    {
    CHECK_ELEC_TOPO_OK;

    switch (WhichOne.vt)
      {
      case VT_I2:
      case VT_I4:
        {
        long N=m_IWires.GetSize();
        long i=(WhichOne.vt==VT_I2) ? WhichOne.iVal : WhichOne.lVal;
        if (i>=0 && i<N)
          {
          IScdWire * p;
          ::CoCreateInstance(CLSID_ScdWire, NULL, CLSCTX_ALL, IID_IScdWire, (void**)&p);
          p->SetObjectInfo((DWORD)m_pNode, (DWORD)m_Wires[i]);
          *pItem=p; // User must release
          return S_OK;
          }
        return Scd.ReturnH(E_INVALIDARG, "Bad Index");
        }
      case VT_BSTR:
        {
        SCD_LclStr(Name, WhichOne.bstrVal);
        for (int i=0; i<m_Wires.GetSize(); i++)
          {
          if (_stricmp(m_Wires[i]->m_sTag, Name)==0)
            {
            IScdWire * p;
            ::CoCreateInstance(CLSID_ScdWire, NULL, CLSCTX_ALL, IID_IScdWire, (void**)&p);
            p->SetObjectInfo((DWORD)m_pNode, (DWORD)m_Wires[i]);
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

STDMETHODIMP CScdWires::get__NewEnum( LPUNKNOWN *  pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_ELEC_TOPO_OK;

    long size=m_IWires.GetSize();

    VARIANT* pVar = new VARIANT[size];
    for(long i = 0; i < size; i++)
      {
      pVar[i].vt = VT_DISPATCH;
      pVar[i].pdispVal = m_IWires[i];
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

STDMETHODIMP CScdWires::get_Count( long *  pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_ELEC_TOPO_OK;

    *pVal=m_pNode->GetWireCount();//m_IWires.GetSize();
    }		
  SCD_COMEXIT
  }

STDMETHODIMP CScdWires::SetObjectInfo(DWORD  Node,  DWORD  Types,  DWORD  Dirn)
  {
  dllSCD_COMENTRY(eScdNodeCodes)
    {
    CHECK_SFE_OK;
    m_pNode       = (FlwNode*)(Node);
    m_eTypes      = (eScdElecTypes)Types;
    m_eDirns      = (eScdDirections)Dirn;
    m_lTopoCount  = gs_pSfeSrvr->FE_GetTopologyCount();
    m_lElectricsTopoCount=-1;
    BuildData();
    }		
  SCD_COMEXIT
  }

STDMETHODIMP CScdWires::Select( eScdElecTypes  Types, eScdDirections Direction)
  {
  dllSCD_COMENTRY(eScdNodeCodes)
    {
    m_eTypes=(eScdElecTypes)Types;
    m_eDirns=(eScdDirections)Direction;
    }		
  SCD_COMEXIT
  }

void CScdWires::BuildData()
  {
  long TopoCnt=gs_pSfeSrvr->FE_GetTopologyCount();
  long ElectricsTopoCount=m_pNode->GetElectricsTopoCount();
  if (m_lElectricsTopoCount!=ElectricsTopoCount)
    {
    int size=0;
    m_lTopoCount=TopoCnt;
    m_lElectricsTopoCount=ElectricsTopoCount;

    //m_pNode->UpdateElectrics();
    //CETermStripPtrs & TS=m_pNode->m_Electrics.m_TermStrips;
    
    m_Wires.SetSize(0);
    for (int i=0; i<m_pNode->GetWireCount(); i++)
      {
      CEWire * p=m_pNode->GetWire(i);
      m_Wires.SetAtGrow(size++, p);
      }
    for (int i = 0; i<m_IWires.GetSize(); i++)
      m_IWires[i]->Release();

    m_IWires.SetSize(size);
    for (int i = 0; i < size; i++)
      {
      IScdWire * p;
      ::CoCreateInstance(CLSID_ScdWire, NULL, CLSCTX_ALL, IID_IScdWire, (void**)&p);
      p->SetObjectInfo((DWORD)m_pNode, (DWORD)m_Wires[i]);//, eScdElecType_All, eScdDirections);
      LPDISPATCH pDisp;
      SCD_TOF(p->QueryInterface(IID_IDispatch, (void**)&pDisp), "IDispatch on ScdWires")
        m_IWires[i]=pDisp;
      p->Release();
      }
    }
  //ASSERT(m_IWires.GetSize()==m_FlwNodes.GetSize());
  //return m_IWires.GetSize();
  }

STDMETHODIMP CScdWires::SetWire(LONG No, BSTR Tag, BSTR SrcTermStrip, BSTR SrcTerminal, BSTR DstTermStrip, BSTR DstTerminal)
  {
  dllSCD_COMENTRY(eScdNodeCodes)
    {
    SCD_LclStr(lTag, Tag);
    SCD_LclStr(lSrcTermStrip, SrcTermStrip);
    SCD_LclStr(lSrcTerminal, SrcTerminal);
    SCD_LclStr(lDstTermStrip, DstTermStrip);
    SCD_LclStr(lDstTerminal, DstTerminal);

    CHECK_TOPO_OK;

    if (!m_pNode->SetWire(No, lTag, lSrcTermStrip, lSrcTerminal, lDstTermStrip, lDstTerminal))
      Scd.ReturnH(E_FAIL);

    }		
  SCD_COMEXIT
  }

STDMETHODIMP CScdWires::AutoWire(BSTR SrcTermStrip, BSTR DstTermStrip)
  {
  dllSCD_COMENTRY(eScdNodeCodes)
    {
    SCD_LclStr(lSrcTermStrip, SrcTermStrip);
    SCD_LclStr(lDstTermStrip, DstTermStrip);

    CHECK_TOPO_OK;
    CLinkWiring LW;
    LW.m_bAutoWire=true;
    LW.m_LinkTag=m_pNode->FullObjTag();
    LW.m_SrcTS=lSrcTermStrip;
    LW.m_DstTS=lDstTermStrip;

    if (m_pNode->SetLinkWiring(LW)<0)
      Scd.ReturnH(E_FAIL);
    }		
  SCD_COMEXIT
  }
