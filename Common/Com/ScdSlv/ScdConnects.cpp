// ScdConnects.cpp : Implementation of CScdConnects
#include "stdafx.h"
#include "ScdSlv.h"
#include "ScdConnects.h"
#include "flwsolve.h"
#include "scdslvwrap.h"

/////////////////////////////////////////////////////////////////////////////
// CScdConnects

STDMETHODIMP CScdConnects::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IScdConnects
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP CScdConnects::SetObjectInfo(DWORD newVal)
  {
  dllSCD_COMENTRY(eScdNodeCodes)
    {
    CHECK_SFE_OK;
    m_pNode=(FlwNode*)newVal;
    m_lTopoCount=gs_pSfeSrvr->FE_GetTopologyCount()-1; // force array build
    BuildData();
    }		
	SCD_COMEXIT
  }

STDMETHODIMP CScdConnects::Connect(VARIANT WhichOne, IScdConnect** pItem)
  {
	dllSCD_COMENTRY(eScdNodeCodes)
    {
    CHECK_TOPO_OK;

    switch (WhichOne.vt)
      {
      case VT_I2:
      case VT_I4:
        {
        long N=m_IConnects.GetSize();
        long i=(WhichOne.vt==VT_I2) ? WhichOne.iVal : WhichOne.lVal;
        if (i>=0 && i<N)
          {
          IScdConnect * p;
          ::CoCreateInstance(CLSID_ScdConnect, NULL, CLSCTX_ALL, IID_IScdConnect, (void**)&p);
          p->SetObjectInfo((DWORD)m_pNode, i);//m_FlwConnects[i]);
          *pItem=p; // User must release
          return S_OK;
          }
        return Scd.ReturnH(E_INVALIDARG, "Bad Index");
        }
      case VT_BSTR:
        {
        SCD_LclStr(Name, WhichOne.bstrVal);
        return Scd.ReturnH(E_NOTIMPL, "Bad Tag");
        FlwNode * pNd=NULL;//gs_pSfeSrvr->FindNode(Name);
        if (pNd)
          {
          IScdConnect * p;
          ::CoCreateInstance(CLSID_ScdConnect, NULL, CLSCTX_ALL, IID_IScdConnect, (void**)&p);
          p->SetObjectInfo((DWORD)pNd, -1);
          *pItem=p; // User must release
          return S_OK;
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

STDMETHODIMP CScdConnects::get_Count(LONG * pVal)
  {
	dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_TOPO_OK;
    *pVal=m_IConnects.GetSize();
    }		
	SCD_COMEXIT
  }

STDMETHODIMP CScdConnects::get__NewEnum(IUnknown * * pVal)
  {
	dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_TOPO_OK;

    long size=m_IConnects.GetSize();

    VARIANT* pVar = new VARIANT[size];
	  for(long i = 0; i < size; i++)
      {
      pVar[i].vt = VT_DISPATCH;
      pVar[i].pdispVal = m_IConnects[i];
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

void CScdConnects::BuildData()
  {
  long TopoCnt=gs_pSfeSrvr->FE_GetTopologyCount();
  if (m_lTopoCount!=TopoCnt)
    {
    m_lTopoCount=TopoCnt;

    if (m_pNode==NULL)
      {
      m_IConnects.SetSize(0);
      }
    else
      {
      UINT size=m_pNode->m_nIOs;
      for(UINT i = 0; i<(UINT)m_IConnects.GetSize(); i++)
        m_IConnects[i]->Release();

      m_IConnects.SetSize(size);
	    for(i = 0; i < size; i++)
        {
        IScdConnect * p;
        ::CoCreateInstance(CLSID_ScdConnect, NULL, CLSCTX_ALL, IID_IScdConnect, (void**)&p);
        p->SetObjectInfo((DWORD)m_pNode, i);
        LPDISPATCH pDisp;
        SCD_TOF(p->QueryInterface(IID_IDispatch, (void**)&pDisp), "IDispatch on ScdConnect")
        m_IConnects[i]=pDisp;
        p->Release();
        }
      }
    }
  //return m_IConnects.GetSize();
  };
