// ScdConversions.cpp : Implementation of CScdConversions

#include "stdafx.h"
#include "ScdConversions.h"
#include "datacnvs.h"


// CScdConversions

STDMETHODIMP CScdConversions::InterfaceSupportsErrorInfo(REFIID riid)
	{
	static const IID* arr[] = 
		{
		&IID_IScdConversions
		};

	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
		{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
		}
	return S_FALSE;
	}

STDMETHODIMP CScdConversions::get_Count(LONG* pVal)
	{
	dllSCD_COMENTRYGET(long, pVal)
		{
		*pVal=gs_CnvsMngr.CnvCnt();
		}
	SCD_COMEXIT
	}

STDMETHODIMP CScdConversions::get__NewEnum(IUnknown** pVal)
	{
	dllSCD_COMENTRYGET(long, pVal)
    {
	  // This block of code allows VB-like iteration.
	  // Make a temp array of VARIANTS and fill 
    
    int size=gs_CnvsMngr.CnvCnt();
    VARIANT* pVar = new VARIANT[size];
	  for(long i = 0; i < size; i++)
      {
      IScdConversion * p;
      ::CoCreateInstance(CLSID_ScdConversion, NULL, CLSCTX_ALL, IID_IScdConversion, (void**)&p);
      p->SetObjectInfo(i, 0);
      LPDISPATCH pDisp;
      SCD_TOF(p->QueryInterface(IID_IDispatch, (void**)&pDisp), "IDispatch on ScdConversion")
      pVar[i].vt = VT_DISPATCH;
      pVar[i].pdispVal = pDisp;
      p->Release();
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
//---------------------------------------------------------------------------

STDMETHODIMP CScdConversions::Item(VARIANT WhichOne, IScdConversion ** pItem)
	{
	dllSCD_COMENTRY(long)
		{
		//if (!gs_pFlwLib)
		//  return Scd.ReturnH(E_FAIL);

		switch (WhichOne.vt)
		  {
		  case VT_I2:
		  case VT_I4:
		    {
		    long i=(WhichOne.vt==VT_I2) ? WhichOne.iVal : WhichOne.lVal;
		    long N=gs_CnvsMngr.CnvCnt();
		    if (i>=0 && i<N)
		      {
		      IScdConversion * p;
		      ::CoCreateInstance(CLSID_ScdConversion, NULL, CLSCTX_ALL, IID_IScdConversion, (void**)&p);
		      p->SetObjectInfo(i, 0);
		      *pItem=p; // User must release
		      return S_OK;
		      }
		    return Scd.ReturnH(E_INVALIDARG, "Bad Index");
		    }
		  case VT_BSTR:
		    {
		    SCD_LclStr(Name, WhichOne.bstrVal);
		    long i=gs_CnvsMngr.FindCnv((LPTSTR)Name);//-1;//m_Mdl.ConversionMap().Lookup(Name);
		    if (i>=0)
		      {
		      IScdConversion * p;
		      ::CoCreateInstance(CLSID_ScdConversion, NULL, CLSCTX_ALL, IID_IScdConversion, (void**)&p);
		      p->SetObjectInfo(i, 0);
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
