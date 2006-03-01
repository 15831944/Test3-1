// ScdSpVectorDefn.cpp : Implementation of CScdSpVectorDefn

#include "stdafx.h"
#include "ScdSpVectorDefn.h"
#include "Sp_DB.h"


// CScdSpVectorDefn
//TODO Expand CScdSpVector... etc to include Attributes
STDMETHODIMP CScdSpVectorDefn::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IScdSpVectorDefn
	};

	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP CScdSpVectorDefn::get_Count(LONG* pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
    *pVal=gs_CDB.m_SDB.Count()+3;
    }
  SCD_COMEXIT
  }

static struct tXData {LPCTSTR m_Tag; CCnvIndex & m_iCnv;} XData[3]=
  {
    {"Enthalpy",    DC_HMs  },
    {"Temperature", DC_T    },
    {"Pressure",    DC_P    },
  };

STDMETHODIMP CScdSpVectorDefn::get_Tag(LONG Index, BSTR* pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
    if (Index<0)
      return Scd.ReturnH(E_INVALIDARG, "Bad Index");

    if (Index<gs_CDB.m_SDB.Count())
      {
      _bstr_t t((LPCTSTR)gs_CDB.m_SDB[Index].Tag());
      *pVal=t.copy();
      }
    else 
      {
      Index-=gs_CDB.m_SDB.Count();
      if (Index>3)
        return Scd.ReturnH(E_INVALIDARG, "Bad Index");
      _bstr_t t(XData[Index].m_Tag);
      *pVal=t.copy();
      }
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdSpVectorDefn::get_Symbol(LONG Index, BSTR* pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
    if (Index<0)
      return Scd.ReturnH(E_INVALIDARG, "Bad Index");

    if (Index<gs_CDB.m_SDB.Count())
      {
      _bstr_t t((LPCTSTR)gs_CDB.m_SDB[Index].SymOrTag());
      *pVal=t.copy();
      }
    else 
      {
      Index-=gs_CDB.m_SDB.Count();
      if (Index>3)
        return Scd.ReturnH(E_INVALIDARG, "Bad Index");
      _bstr_t t(XData[Index].m_Tag);
      *pVal=t.copy();
      }
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdSpVectorDefn::get_Cnv(VARIANT Index, IScdConversion ** pVal)
  {
	dllSCD_COMENTRY(long)
		{
    long iCnv=DC_;
    switch (Index.vt)
		  {
		  case VT_I2:
		  case VT_I4:
		    {
		    long i=(Index.vt==VT_I2) ? Index.iVal : Index.lVal;

        if (i<0)
          return Scd.ReturnH(E_INVALIDARG, "Bad Index");

        if (i<gs_CDB.m_SDB.Count())
          iCnv=DC_M;
        else 
          {
          i-=gs_CDB.m_SDB.Count();
          if (i>3)
            return Scd.ReturnH(E_INVALIDARG, "Bad Index");
          iCnv=XData[i].m_iCnv;
          }
		    break;
        }
		  case VT_BSTR:
		    {
		    SCD_LclStr(Name, Index.bstrVal);
		    long i=gs_CDB.m_SDB.Find((LPTSTR)Name);//-1;//m_Mdl.ConversionMap().Lookup(Name);
		    if (i<0)
          return Scd.ReturnH(E_INVALIDARG, "Bad Tag");
        iCnv=DC_M;
		    break;
        }
		  default:
		    return Scd.ReturnH(E_INVALIDARG);
		  }
		long N=gs_CnvsMngr.CnvCnt();
		if (iCnv>=0 && iCnv<N)
		  {
		  IScdConversion * p;
		  ::CoCreateInstance(CLSID_ScdConversion, NULL, CLSCTX_ALL, IID_IScdConversion, (void**)&p);
		  p->SetObjectInfo(iCnv, 0);
		  *pVal=p; // User must release
		  return S_OK;
		  }
		return Scd.ReturnH(E_INVALIDARG, "Bad Index");
		}		
	SCD_COMEXIT
  }
