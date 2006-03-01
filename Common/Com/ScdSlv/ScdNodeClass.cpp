// ScdNodeClass.cpp : Implementation of CScdNodeClass
#include "stdafx.h"
#include "ScdSlv.h"
#include "ScdNodeClass.h"
#include "scdslvwrap.h"

/////////////////////////////////////////////////////////////////////////////
// CScdNodeClass

STDMETHODIMP CScdNodeClass::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IScdNodeClass
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP CScdNodeClass::get_Description(BSTR *pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_SFE_OK;
    SCD_Copy2BSTR(pVal, m_pNodeClass->Desc());
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdNodeClass::get_ShortDescription(BSTR *pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_SFE_OK;
    SCD_Copy2BSTR(pVal, m_pNodeClass->ShortDesc());
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdNodeClass::get_Library(BSTR *pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_SFE_OK;
    SCD_Copy2BSTR(pVal, m_pNodeClass->MdlLibName());
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdNodeClass::get_DefaultTag(BSTR *pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_SFE_OK;
    SCD_Copy2BSTR(pVal, m_pNodeClass->DefaultTag());
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdNodeClass::get_Name(BSTR *pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_SFE_OK;
    SCD_Copy2BSTR(pVal, m_pNodeClass->ClassId());
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdNodeClass::get_Category(eScdNodeCat_Flags *pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_SFE_OK;
    *pVal=(eScdNodeCat_Flags)m_pNodeClass->Category();
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdNodeClass::SetObjectInfo(DWORD newVal)
  {
  dllSCD_COMENTRY(eScdNodeCodes)
    {
    CHECK_SFE_OK;
    m_pNodeClass=(TagObjClass*)newVal;
    m_lTopoCount=gs_pSfeSrvr->FE_GetTopologyCount();
    }		
	SCD_COMEXIT
  }
