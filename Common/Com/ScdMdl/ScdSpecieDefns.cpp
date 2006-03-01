// ScdSpecieDefns.cpp : Implementation of CScdSpecieDefns
#include "stdafx.h"
#include "ScdMdl.h"
#include "ScdSpecieDefns.h"
#include "Sp_DB.h"

/////////////////////////////////////////////////////////////////////////////
// CScdSpecieDefns

STDMETHODIMP CScdSpecieDefns::InterfaceSupportsErrorInfo(REFIID riid)
  {
	static const IID* arr[] = 
	  {
		&IID_IScdSpecieDefns
	  };
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	  {
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	  }
	return S_FALSE;
  }


//void CScdSpecieDefns::FireTheEvent(long Evt, long Data)
//  {
//  switch (e)
//    {
//    case ComCmd_LoadProject:  
//      Fire_OnProjectOpened(l); 
//      break;
//    };
//  };

STDMETHODIMP CScdSpecieDefns::Specie(VARIANT WhichOne, IScdSpecieDefn** pItem)
  {
	dllSCD_COMENTRY(long)
    {
    switch (WhichOne.vt)
      {
      case VT_I2:
      case VT_I4:
        {
        long i=(WhichOne.vt==VT_I2) ? WhichOne.iVal : WhichOne.lVal;
        long N=SDB.Count();
        if (i>=0 && i<N)
          {
          IScdSpecieDefn * p;
          ::CoCreateInstance(CLSID_ScdSpecieDefn, NULL, CLSCTX_ALL, IID_IScdSpecieDefn, (void**)&p);
          p->put_Index(i);
          *pItem=p; // User must release
          return S_OK;
          }
        return Scd.ReturnH(E_INVALIDARG, "Bad Index");
        }
      case VT_BSTR:
        {
        SCD_LclStr(Name, WhichOne.bstrVal);
        long N=SDB.Count();
        long i=SDB.Find(Name);
        if (i>=0 && i<N)
          {
          IScdSpecieDefn * p;
          ::CoCreateInstance(CLSID_ScdSpecieDefn, NULL, CLSCTX_ALL, IID_IScdSpecieDefn, (void**)&p);
          p->put_Index(i);
          *pItem=p; // User must release
          return S_OK;
          }
        return Scd.ReturnH(E_INVALIDARG, "Bad Index");
        }
      default:
        return Scd.ReturnH(E_INVALIDARG);
      }
    *pItem=NULL;
    }		
	SCD_COMEXIT
  }

STDMETHODIMP CScdSpecieDefns::get_Count(/*[out,retval]*/ LONG *pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
    *pVal=gs_CDB.m_SDB.Count();
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdSpecieDefns::get__NewEnum(LPUNKNOWN * pVal)
  {
	dllSCD_COMENTRYGET(long, pVal)
    {
	  // This block of code allows VB-like iteration.
	  // Make a temp array of VARIANTS and fill 
    
    int size=gs_CDB.m_SDB.Count();
    if (m_Spcs.GetSize()==0)
      {
      m_Spcs.SetSize(size);
	    for(long i = 0; i < size; i++)
        {
        IScdSpecieDefn * p;
        ::CoCreateInstance(CLSID_ScdSpecieDefn, NULL, CLSCTX_ALL, IID_IScdSpecieDefn, (void**)&p);
        p->put_Index(i);
        LPDISPATCH pDisp;
        SCD_TOF(p->QueryInterface(IID_IDispatch, (void**)&pDisp), "IDispatch on ScdSpecieDefn")
        m_Spcs[i]=pDisp;
        p->Release();
        }
      }

    VARIANT* pVar = new VARIANT[size];
	  for(long i = 0; i < size; i++)
      {
      pVar[i].vt = VT_DISPATCH;
      pVar[i].pdispVal = m_Spcs[i];
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


//STDMETHODIMP CScdSpecieDefns::get_Symbol(long Index, BSTR *pVal)
//  {
//  dllSCD_COMENTRYGET(long, pVal)
//    {
//    _bstr_t s;
//    s=gs_CDB.m_SDB[Index].Sym();
//	  *pVal=s;
//    }
//  SCD_COMEXIT
//  }
//
//STDMETHODIMP CScdSpecieDefns::get_Tag(long Index, BSTR *pVal)
//  {
//  dllSCD_COMENTRYGET(long, pVal)
//    {
//    _bstr_t s;
//    s=gs_CDB.m_SDB[Index].Tag();
//	  *pVal=s;
//    }
//  SCD_COMEXIT
//  }

STDMETHODIMP CScdSpecieDefns::get_SomAll(LONG *pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
    *pVal=gs_CDB.m_som_ALL;
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdSpecieDefns::get_SomVap(LONG *pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
    *pVal=gs_CDB.m_som_Gas;
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdSpecieDefns::get_SomLiq(LONG *pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
   	*pVal=gs_CDB.m_som_Liq;
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdSpecieDefns::get_SomSol(LONG *pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
    *pVal=gs_CDB.m_som_Sol;
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdSpecieDefns::get_SomSL(LONG *pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
    *pVal=gs_CDB.m_som_SL;
    }
  SCD_COMEXIT
  }

//---------------------------------------------------------------------------

STDMETHODIMP CScdSpecieDefns::get_Phases(/*[in]*/ LONG ReqdPhases, /*[out, retval]*/ SAFEARRAY **pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
    SAFEARRAY *pSA=NULL;
    SAFEARRAYBOUND bound[1];
    bound[0].lLbound=0;
    bound[0].cElements=SDB.Count();
    pSA=SafeArrayCreate(VT_I4, 1, bound);
    long Cnt=bound[0].cElements;
    for (long i=0; i<Cnt; i++)
      {
      long l=(SDB[i].m_PhMsk&ReqdPhases)!=0 ? SDB[i].m_PhMsk: 0;
      SafeArrayPutElement(pSA, &i, &l);
      }
    *pVal=pSA;
    }
  SCD_COMEXIT
  };

//---------------------------------------------------------------------------

STDMETHODIMP CScdSpecieDefns::get_MolecularWeights(/*[out, retval]*/ SAFEARRAY **pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
    SAFEARRAY *pSA=NULL;
    SAFEARRAYBOUND bound[1];
    bound[0].lLbound=0;
    bound[0].cElements=SDB.Count();
    pSA=SafeArrayCreate(VT_R8, 1, bound);
    long Cnt=bound[0].cElements;
    for (long i=0; i<Cnt; i++)
      {
      double d=SDB[i].MoleWt();
      SafeArrayPutElement(pSA, &i, &d);
      }
    *pVal=pSA;
    }
  SCD_COMEXIT
  };

//---------------------------------------------------------------------------
