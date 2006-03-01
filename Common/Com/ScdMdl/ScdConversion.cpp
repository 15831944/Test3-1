// ScdConversion.cpp : Implementation of CScdConversion

#include "stdafx.h"
#include "ScdConversion.h"
//#include ".\scdconversion.h"


// CScdConversion

STDMETHODIMP CScdConversion::InterfaceSupportsErrorInfo(REFIID riid)
	{
	static const IID* arr[] = 
		{
		&IID_IScdConversion
		};

	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
		{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
		}
	return S_FALSE;
	}

STDMETHODIMP CScdConversion::get_Family(BSTR* pVal)
	{
	dllSCD_COMENTRYGET(long, pVal)
		{
		_bstr_t t(Cnvs[m_iCnv]->Fam());
		*pVal=t.copy();
		}		
	SCD_COMEXIT
	}

STDMETHODIMP CScdConversion::get_Name(BSTR* pVal)
	{
	dllSCD_COMENTRYGET(long, pVal)
		{
		_bstr_t t((*Cnvs[m_iCnv])[m_iIndex]->Txt());
		*pVal=t.copy();
		}		
	SCD_COMEXIT
	}

STDMETHODIMP CScdConversion::get_Description(BSTR* pVal)
	{
	dllSCD_COMENTRYGET(long, pVal)
		{
		_bstr_t t((*Cnvs[m_iCnv])[m_iIndex]->Desc());
		*pVal=t.copy();
		}		
	SCD_COMEXIT
	}

STDMETHODIMP CScdConversion::get_Scale(DOUBLE* pVal)
	{
	dllSCD_COMENTRYGET(long, pVal)
		{
		*pVal=(*Cnvs[m_iCnv])[m_iIndex]->SclValue();
		}		
	SCD_COMEXIT
	}

STDMETHODIMP CScdConversion::get_Offset(DOUBLE* pVal)
	{
	dllSCD_COMENTRYGET(long, pVal)
		{
		*pVal=(*Cnvs[m_iCnv])[m_iIndex]->OffValue();
		}		
	SCD_COMEXIT
	}

STDMETHODIMP CScdConversion::get_IsSI(VARIANT_BOOL* pVal)
	{
	dllSCD_COMENTRYGET(long, pVal)
		{
		*pVal=m_iIndex==0;
		}		
	SCD_COMEXIT
	}

STDMETHODIMP CScdConversion::get_Count(LONG* pVal)
	{
	dllSCD_COMENTRYGET(long, pVal)
		{
		*pVal=(*Cnvs[m_iCnv])[m_iIndex]->Count();
		}		
	SCD_COMEXIT
	}

STDMETHODIMP CScdConversion::get__NewEnum(IUnknown** pVal)
	{
	dllSCD_COMENTRYGET(long, pVal)
    {
	  // This block of code allows VB-like iteration.
	  // Make a temp array of VARIANTS and fill 
    
    int size=Cnvs[m_iCnv]->Count();
    VARIANT* pVar = new VARIANT[size];
	  for(long i = 0; i < size; i++)
      {
      IScdConversion * p;
      SCD_ROF(::CoCreateInstance(CLSID_ScdConversion, NULL, CLSCTX_ALL, IID_IScdConversion, (void**)&p));
      SCD_ROF(p->SetObjectInfo(m_iCnv, i));
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

STDMETHODIMP CScdConversion::Item(VARIANT WhichOne, IScdConversion ** pItem)
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
		    long N=Cnvs[m_iCnv]->Count();
		    if (i>=0 && i<N)
		      {
		      IScdConversion * p;
		      ::CoCreateInstance(CLSID_ScdConversion, NULL, CLSCTX_ALL, IID_IScdConversion, (void**)&p);
		      p->SetObjectInfo(m_iCnv, i);
		      *pItem=p; // User must release
		      return S_OK;
		      }
		    return Scd.ReturnH(E_INVALIDARG, "Bad Index");
		    }
		  case VT_BSTR:
		    {
		    SCD_LclStr(Name, WhichOne.bstrVal);
		    CDataCnv * pC =Cnvs[m_iCnv]->Find((LPTSTR)Name);//-1;//m_Mdl.ConversionMap().Lookup(Name);
		    if (pC)
		      {
		      IScdConversion * p;
		      ::CoCreateInstance(CLSID_ScdConversion, NULL, CLSCTX_ALL, IID_IScdConversion, (void**)&p);
		      p->SetObjectInfo(m_iCnv, pC->SecIndex());
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


STDMETHODIMP CScdConversion::get_Index(LONG* pVal)
	{
  dllSCD_COMENTRYGET(long, pVal)
    {
    *pVal=m_iIndex;
    }		
	SCD_COMEXIT
	}

STDMETHODIMP CScdConversion::SetObjectInfo(LONG iCnv, LONG iIndex)
	{
  dllSCD_COMENTRY(long)
    {
    m_iCnv=iCnv;
    m_iIndex=iIndex;
    }		
	SCD_COMEXIT
	}

STDMETHODIMP CScdConversion::get_Formula(BSTR* pVal)
	{
	appSCD_COMENTRYGET(long, pVal)
		{
		_bstr_t t(Cnvs[m_iCnv]->XformName());
		*pVal=t.copy();
		}		
	SCD_COMEXIT
	}
