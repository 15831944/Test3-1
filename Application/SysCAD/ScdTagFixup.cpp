// ScdTagFixup.cpp : Implementation of CScdTagFixup

#include "stdafx.h"
#include "ScdTagFixup.h"
#include ".\scdtagfixup.h"
#include "SysCAD_i.h"
#include "ScdProject.h"
#include "licbase.h"
#include "project.h"
#include "tagvdoc.h"

// CScdTagFixup

STDMETHODIMP CScdTagFixup::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IScdTagFixup
	};

	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP CScdTagFixup::get_Rule(eScdTagFixupRules* pVal)
  {
  appSCD_COMENTRYGET(long, pVal)
    {
    *pVal=m_iRule;
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdTagFixup::put_Rule(eScdTagFixupRules newVal)
  {
  appSCD_COMENTRY(long)
    {
    m_iRule = newVal;
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdTagFixup::get_String1(BSTR* pVal)
  {
  appSCD_COMENTRYGET(long, pVal)
    {
    SCD_Copy2BSTR(pVal, m_sString1);
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdTagFixup::put_String1(BSTR newVal)
  {
  appSCD_COMENTRY(long)
    {
    SCD_LclStr(String1, newVal);
    m_sString1=String1;
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdTagFixup::get_String2(BSTR* pVal)
  {
  appSCD_COMENTRYGET(long, pVal)
    {
    SCD_Copy2BSTR(pVal, m_sString2);
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdTagFixup::put_String2(BSTR newVal)
  {
  appSCD_COMENTRY(long)
    {
    SCD_LclStr(String2, newVal);
    m_sString2=String2;
    }
  SCD_COMEXIT
  }



