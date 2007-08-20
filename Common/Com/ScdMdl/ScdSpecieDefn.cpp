// ScdSpecieDefn.cpp : Implementation of CScdSpecieDefn
#include "stdafx.h"
#include "ScdMdl.h"
#include "ScdSpecieDefn.h"
//#include "ScdSpecieDefn.h"
#include "Sp_DB.h"
#include ".\scdspeciedefn.h"
#include "propertybase.h"

/////////////////////////////////////////////////////////////////////////////
// CScdSpecieDefn

STDMETHODIMP CScdSpecieDefn::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IScdSpecieDefn
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

//STDMETHODIMP CScdSpecieDefn::get_Value(LONG Index, DOUBLE TdK, DOUBLE PkPa, VARIANT * pVal)
//	{
//  USES_CONVERSION;
//  dllSCD_COMENTRYGET(long, pVal)
//    {
//    if (m_lIndex>=0 && m_lIndex<gs_CDB.m_SDB.No())
//      {
//      // assume R8
//_asm int 3;
//
//      //pVal->vt=VT_R8;
//      //switch (Index)
//      //  {
//      //  case PI_Tag:
//      //    pVal->vt=VT_BSTR;
//      //    pVal->bstrVal=SysAllocString(T2OLE(SDB[m_lIndex].Tag()));
//      //    break;
//      //  case PI_Symbol:
//      //    pVal->vt=VT_BSTR;
//      //    pVal->bstrVal=SysAllocString(T2OLE(SDB[m_lIndex].Sym()));
//      //    break;
//      //  case PI_Definition:
//      //    pVal->vt=VT_BSTR;
//      //    pVal->bstrVal=SysAllocString(T2OLE(CDB[SDB[m_lIndex].iCId].ElDef()));
//      //    break;
//      //  case PI_Phase:
//      //    //pVal->vt=VT_BSTR;
//      //    //pVal->bstrVal=SysAllocString(T2OLE(SDB[m_lIndex].PhaseName));
//      //    Scd.ReturnH(E_NOTIMPL);
//      //    break;
//      //  case PI_Occurence:
//      //    //pVal->vt=VT_BSTR;
//      //    //pVal->bstrVal=SysAllocString(T2OLE(SDB[m_lIndex].mOcc);
//      //    Scd.ReturnH(E_NOTIMPL);
//      //    break;
//      //  case PI_Checked:
//      //    //pVal->vt=VT_BSTR;
//      //    //pVal->bstrVal=SysAllocString(T2OLE(SDB[m_lIndex].iCId].ElDef()));
//      //    Scd.ReturnH(E_NOTIMPL);
//      //    break;
//      //  case PI_MolecularWt:
//      //    pVal->dblVal=SDB[m_lIndex].MoleWt();
//      //    break;
//      //  case PI_Ts:
//      //    pVal->dblVal=SDB[m_lIndex].LoT(m_lFidelity);
//      //    break;
//      //  case PI_Te:
//      //    pVal->dblVal=SDB[m_lIndex].HiT(m_lFidelity);
//      //    break;
//      //  case PI_Density:
//      //    pVal->dblVal=SDB[m_lIndex].Density(m_lFidelity, TdK,PkPa,NULL,NULL);
//      //    break;
//      //  case PI_Hf:
//      //    pVal->dblVal=SDB[m_lIndex].msHf(m_lFidelity, TdK,PkPa,NULL,NULL);
//      //    break;
//      //  case PI_S:
//      //    pVal->dblVal=SDB[m_lIndex].msSf(m_lFidelity, TdK,PkPa,NULL,NULL);
//      //    break;
//      //  case PI_Cp:
//      //    pVal->dblVal=SDB[m_lIndex].msCp(m_lFidelity, TdK,PkPa,NULL,NULL);
//      //    break;
//      //  case PI_Vp:
//      //    pVal->dblVal=SDB[m_lIndex].VapourP(m_lFidelity, TdK);
//      //    break;
//      //  case PI_Pc:
//      //    pVal->dblVal=SDB[m_lIndex].PCrit();
//      //    break;
//      //  case PI_Tc:
//      //    pVal->dblVal=SDB[m_lIndex].TCrit();
//      //    break;
//      //  case PI_Vc:
//      //    pVal->dblVal=SDB[m_lIndex].VCrit();
//      //    break;
//      //  default:
//      //    Scd.ReturnH(E_INVALIDARG);
//      //  }
//      }
//    else
//      Scd.ReturnH(E_INVALIDARG);
//    }			
//  SCD_COMEXIT
//	}

STDMETHODIMP CScdSpecieDefn::get_MolecularWt(DOUBLE * pVal)
	{
  dllSCD_COMENTRYGET(long, pVal)
    {
    if (m_lIndex>=0 && m_lIndex<gs_CDB.m_SDB.Count())
      *pVal=gs_CDB.m_SDB[m_lIndex].MoleWt();
    else
      Scd.ReturnH(E_INVALIDARG);
    }
  SCD_COMEXIT
	}
STDMETHODIMP CScdSpecieDefn::get_Density(DOUBLE TdK, DOUBLE PkPa, DOUBLE * pVal)
	{
  dllSCD_COMENTRYGET(long, pVal)
    {
    if (m_lIndex>=0 && m_lIndex<gs_CDB.m_SDB.Count())
      *pVal=gs_CDB.m_SDB[m_lIndex].DensityXZero(CDensityInfo(m_lFidelity, SMDensM_None, TdK, PkPa, NULL, NULL));
    else
      Scd.ReturnH(E_INVALIDARG);
    }
  SCD_COMEXIT
	}
STDMETHODIMP CScdSpecieDefn::get_msEnthalpy(DOUBLE TdK, DOUBLE PkPa, DOUBLE * pVal)
	{
  dllSCD_COMENTRYGET(long, pVal)
    {
    if (m_lIndex>=0 && m_lIndex<gs_CDB.m_SDB.Count())
      *pVal=gs_CDB.m_SDB[m_lIndex].msHf(m_lFidelity, TdK, PkPa, NULL, NULL);
    else
      Scd.ReturnH(E_INVALIDARG);
    }
  SCD_COMEXIT
	}
STDMETHODIMP CScdSpecieDefn::get_msEntropy(DOUBLE TdK, DOUBLE PkPa, DOUBLE * pVal)
	{
  dllSCD_COMENTRYGET(long, pVal)
    {
    if (m_lIndex>=0 && m_lIndex<gs_CDB.m_SDB.Count())
      *pVal=gs_CDB.m_SDB[m_lIndex].msSf(m_lFidelity, TdK, PkPa, NULL, NULL);
    else
      Scd.ReturnH(E_INVALIDARG);
    }
  SCD_COMEXIT
	}
STDMETHODIMP CScdSpecieDefn::get_msCp(DOUBLE TdK, DOUBLE PkPa, DOUBLE * pVal)
	{
  dllSCD_COMENTRYGET(long, pVal)
    {
    if (m_lIndex>=0 && m_lIndex<gs_CDB.m_SDB.Count())
      *pVal=gs_CDB.m_SDB[m_lIndex].msCp(m_lFidelity, TdK, PkPa, NULL, NULL);
    else
      Scd.ReturnH(E_INVALIDARG);
    }
  SCD_COMEXIT
	}
STDMETHODIMP CScdSpecieDefn::get_DynamicViscosity(DOUBLE TdK, DOUBLE PkPa, DOUBLE * pVal)
	{
  dllSCD_COMENTRYGET(long, pVal)
    {
    if (m_lIndex>=0 && m_lIndex<gs_CDB.m_SDB.Count())
      //*pVal=gs_CDB.m_SDB[m_lIndex]..Density(m_lFidelity, TdK, PkPa, NULL, NULL);
      Scd.ReturnH(E_NOTIMPL);
    else
      Scd.ReturnH(E_INVALIDARG);
    }
  SCD_COMEXIT
	}
STDMETHODIMP CScdSpecieDefn::get_ThermalConductivity(DOUBLE TdK, DOUBLE PkPa, DOUBLE * pVal)
	{
  dllSCD_COMENTRYGET(long, pVal)
    {
    if (m_lIndex>=0 && m_lIndex<gs_CDB.m_SDB.Count())
      //*pVal=gs_CDB.m_SDB[m_lIndex]..Density(m_lFidelity, TdK, PkPa, NULL, NULL);
      Scd.ReturnH(E_NOTIMPL);
    else
      Scd.ReturnH(E_INVALIDARG);
    }
  SCD_COMEXIT
	}
STDMETHODIMP CScdSpecieDefn::get_VapourP(DOUBLE TdK, DOUBLE * pVal)
	{
  dllSCD_COMENTRYGET(long, pVal)
    {
    if (m_lIndex>=0 && m_lIndex<gs_CDB.m_SDB.Count())
      *pVal=gs_CDB.m_SDB[m_lIndex].VapourP(m_lFidelity, TdK);
    else
      Scd.ReturnH(E_INVALIDARG);
    }
  SCD_COMEXIT
	}
STDMETHODIMP CScdSpecieDefn::get_VapourT(DOUBLE PkPa, DOUBLE * pVal)
	{
  dllSCD_COMENTRYGET(long, pVal)
    {
    if (m_lIndex>=0 && m_lIndex<gs_CDB.m_SDB.Count())
      *pVal=gs_CDB.m_SDB[m_lIndex].VapourT(m_lFidelity, PkPa);
    else
      Scd.ReturnH(E_INVALIDARG);
    }
  SCD_COMEXIT
	}

STDMETHODIMP CScdSpecieDefn::get_Tag(BSTR *pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
    if (m_lIndex>=0 && m_lIndex<gs_CDB.m_SDB.Count())
      {
      SCD_Copy2BSTR(pVal, gs_CDB.m_SDB[m_lIndex].Tag());
      }
    else
      Scd.ReturnH(E_INVALIDARG);
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdSpecieDefn::get_Symbol(BSTR *pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
    if (m_lIndex>=0 && m_lIndex<gs_CDB.m_SDB.Count())
      {
	    SCD_Copy2BSTR(pVal, gs_CDB.m_SDB[m_lIndex].Sym());
      }
    else
      Scd.ReturnH(E_INVALIDARG);
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdSpecieDefn::get_Index(long *pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
	  *pVal=m_lIndex;
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdSpecieDefn::put_Index(long newVal)
  {
  dllSCD_COMENTRY(long)
    {
    if (newVal>=0 && newVal<gs_CDB.m_SDB.Count())
      {
  	  m_lIndex=newVal;
      }
    else
      Scd.ReturnH(E_INVALIDARG);
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdSpecieDefn::get_Fidelity(long *pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
	  *pVal=m_lFidelity;
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdSpecieDefn::put_Fidelity(long newVal)
  {
  dllSCD_COMENTRY(long)
    {
    if (newVal>=0 && newVal<MaxFidelity)
      {
  	  m_lFidelity=newVal;
      }
    else
      Scd.ReturnH(E_INVALIDARG);
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdSpecieDefn::get_Properties(IScdProperties ** pVal)
  {
  dllSCD_COMENTRY(long)
    {
    IScdProperties * p;
    ::CoCreateInstance(CLSID_ScdProperties, NULL, CLSCTX_ALL, IID_IScdProperties, (void**)&p);
    CPropertyBase *pB=&SDB[m_lIndex];
    p->SetObjectInfo(0, (DWORD)pB);
    *pVal=p;
    }
  SCD_COMEXIT
  }
