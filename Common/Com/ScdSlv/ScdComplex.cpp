// ScdComplex.cpp : Implementation of CScdComplex

#include "stdafx.h"
#include "ScdComplex.h"
#include ".\scdcomplex.h"


// CScdComplex

STDMETHODIMP CScdComplex::InterfaceSupportsErrorInfo(REFIID riid)
  {
  static const IID* arr[] = 
    {
    &IID_IScdComplex
    };

  for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
    {
    if (InlineIsEqualGUID(*arr[i],riid))
      return S_OK;
    }
  return S_FALSE;
  }

STDMETHODIMP CScdComplex::get_Real(DOUBLE* pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    *pVal=m_Real;
    }		
  SCD_COMEXIT

  }

STDMETHODIMP CScdComplex::put_Real(DOUBLE newVal)
  {
  dllSCD_COMENTRY(eScdNodeCodes)
    {
    m_Real=newVal;
    }		
  SCD_COMEXIT
  }

STDMETHODIMP CScdComplex::get_Imag(DOUBLE* pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    *pVal=m_Imag;
    }		
  SCD_COMEXIT
  }

STDMETHODIMP CScdComplex::put_Imag(DOUBLE newVal)
  {
  dllSCD_COMENTRY(eScdNodeCodes)
    {
    m_Imag=newVal;
    }		
  SCD_COMEXIT
  }

STDMETHODIMP CScdComplex::get_Magnitude(DOUBLE* pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CDComplex X(m_Real, m_Imag);
    *pVal=X.Mag();
    }		
  SCD_COMEXIT
  }

STDMETHODIMP CScdComplex::put_Magnitude(DOUBLE newVal)
  {
  AFX_MANAGE_STATE(AfxGetStaticModuleState());
  return E_NOTIMPL;
  }

STDMETHODIMP CScdComplex::get_Phase(DOUBLE* pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CDComplex X(m_Real, m_Imag);
    *pVal=X.Ang();
    }		
  SCD_COMEXIT
  }

STDMETHODIMP CScdComplex::put_Phase(DOUBLE newVal)
  {
  AFX_MANAGE_STATE(AfxGetStaticModuleState());
  return E_NOTIMPL;
  }
