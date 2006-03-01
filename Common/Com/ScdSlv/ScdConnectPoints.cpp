// ScdConnectPoints.cpp : Implementation of CScdConnectPoints
#include "stdafx.h"
#include "ScdSlv.h"
#include "ScdConnectPoints.h"
#include "flwsolve.h"
#include "scdslvwrap.h"

/////////////////////////////////////////////////////////////////////////////
// CScdConnectPoints

STDMETHODIMP CScdConnectPoints::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IScdConnectPoints
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP CScdConnectPoints::SetObjectInfo(DWORD newVal)
  {
  dllSCD_COMENTRY(eScdNodeCodes)
    {
    CHECK_SFE_OK;
    m_pNode=(FlwNode*)newVal;
    m_lTopoCount=gs_pSfeSrvr->FE_GetTopologyCount()-1;  // force array build
    BuildData();
    }		
	SCD_COMEXIT
  }

STDMETHODIMP CScdConnectPoints::ConnectPoint(VARIANT WhichOne, IScdConnectPoint** pItem)
  {
	dllSCD_COMENTRY(eScdNodeCodes)
    {
    CHECK_TOPO_OK;

    switch (WhichOne.vt)
      {
      case VT_I2:
      case VT_I4:
        {
        long N=m_IConnectPoints.GetSize();
        long i=(WhichOne.vt==VT_I2) ? WhichOne.iVal : WhichOne.lVal;
        if (i>=0 && i<N)
          {
          IScdConnectPoint * p;
          ::CoCreateInstance(CLSID_ScdConnectPoint, NULL, CLSCTX_ALL, IID_IScdConnectPoint, (void**)&p);
          p->SetObjectInfo((DWORD)m_pNode, i);//m_FlwConnectPoints[i]);
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
          IScdConnectPoint * p;
          ::CoCreateInstance(CLSID_ScdConnectPoint, NULL, CLSCTX_ALL, IID_IScdConnectPoint, (void**)&p);
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

STDMETHODIMP CScdConnectPoints::get_Count(LONG * pVal)
  {
	dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_TOPO_OK;
    *pVal=m_IConnectPoints.GetSize();
    }		
	SCD_COMEXIT
  }

STDMETHODIMP CScdConnectPoints::get__NewEnum(IUnknown * * pVal)
  {
	dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_TOPO_OK;

    long size=m_IConnectPoints.GetSize();

    VARIANT* pVar = new VARIANT[size];
	  for(long i = 0; i < size; i++)
      {
      pVar[i].vt = VT_DISPATCH;
      pVar[i].pdispVal = m_IConnectPoints[i];
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

void CScdConnectPoints::BuildData()
  {
  long TopoCnt=gs_pSfeSrvr->FE_GetTopologyCount();
  if (m_lTopoCount!=TopoCnt)
    {
    m_lTopoCount=TopoCnt;

    if (m_pNode==NULL)
      {
      m_IConnectPoints.SetSize(0);
      }
    else
      {
      UINT size=m_pNode->IOAreaCount();
      for(UINT i = 0; i<(UINT)m_IConnectPoints.GetSize(); i++)
        m_IConnectPoints[i]->Release();

      m_IConnectPoints.SetSize(size);
	    for(i = 0; i < size; i++)
        {
        IScdConnectPoint * p;
        ::CoCreateInstance(CLSID_ScdConnectPoint, NULL, CLSCTX_ALL, IID_IScdConnectPoint, (void**)&p);
        p->SetObjectInfo((DWORD)m_pNode, i);
        LPDISPATCH pDisp;
        SCD_TOF(p->QueryInterface(IID_IDispatch, (void**)&pDisp), "IDispatch on ScdConnectPoint")
        m_IConnectPoints[i]=pDisp;
        p->Release();
        }
      }
    }
  //return m_IConnectPoints.GetSize();
  };

STDMETHODIMP CScdConnectPoints::get_InputPathName(eScdLinkTypes Type, long PathNo, BSTR *pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_TOPO_OK;

    if (PathNo<0 || PathNo>2)
      return Scd.ReturnH(E_INVALIDARG);

    static const byte RqdMasks[]={LIO_In0,LIO_In1,LIO_In2};
    byte RqdMask=RqdMasks[PathNo];
    UINT N=m_pNode->IOAreaCount();
    Strng T;
	  for (UINT i = 0; i < N; i++)
      {
      IOAreaRec &A=m_pNode->IOArea(i);
      if ((A.m_dwType&Type) && ((A.Dirn&RqdMask)==RqdMask))
        {
        T=A.pName;
        break;
        }
      }
    if (T.GetLength()>0)
      SCD_Copy2BSTR(pVal, T())
    else
      *pVal=NULL;
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdConnectPoints::get_OutputPathName(eScdLinkTypes Type, long PathNo, BSTR *pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_TOPO_OK;
    
    if (PathNo<0 || PathNo>2)
      return Scd.ReturnH(E_INVALIDARG);
    
    static const byte RqdMasks[]={LIO_Out0,LIO_Out1,LIO_Out2};
    byte RqdMask=RqdMasks[PathNo];
    UINT N=m_pNode->IOAreaCount();
    Strng T;
	  for (UINT i = 0; i < N; i++)
      {
      IOAreaRec &A=m_pNode->IOArea(i);
      if ((A.m_dwType&Type) && ((A.Dirn&RqdMask)==RqdMask))
        {
        T=A.pName;
        break;
        }
      }
    if (T.GetLength()>0)
      SCD_Copy2BSTR(pVal, T())
    else
      *pVal=NULL;
    }
  SCD_COMEXIT
  }
