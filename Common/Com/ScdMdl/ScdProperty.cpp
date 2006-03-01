// ScdSpProp.cpp : Implementation of CScdProperty

#include "stdafx.h"
#include "ScdProperty.h"
#include "licbase.h"
#include ".\scdproperty.h"


//---------------------------------------------------------------------------
// CScdProperty

HRESULT CScdProperty::FinalConstruct()
	{
	FC_TESTLICENSE(gs_License.AllowCOMMdl());

  m_pProps=NULL;
  m_iPropNo=-1;
	AtlTrace("CScdProperty::FinalConstruct : TID %i ==================================\n", GetCurrentThreadId());
  
  //FC_ROF(CoCreateFreeThreadedMarshaler(GetControllingUnknown(), &m_pUnkMarshaler.p));
//  FC_ROF(m_pSolver.CoCreateInstance(CLSID_ScdSolver));
//  FC_ROF(m_pSolver->get_SpecieDB(&m_pSpecieDB.p));
//  FC_ROF(m_pSpecieDB.CoCreateInstance(CLSID_ScdSpecieDB));

	AtlTrace("CScdProperty::FinalConstruct : Done ==================================\n");
	return S_OK;
  }

//---------------------------------------------------------------------------

void CScdProperty::FinalRelease()
	{
  ////m_pUnkMarshaler.Release();
  // //if (m_spSpecieDB.GetInterfacePtr())
  // m_pSpecieDB.Release();
	}

//---------------------------------------------------------------------------

STDMETHODIMP CScdProperty::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IScdProperty
	};

	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

//---------------------------------------------------------------------------

STDMETHODIMP CScdProperty::SetObjectInfo(ULONG What, ULONG Mdl, ULONG PropNo)
  {
  dllSCD_COMENTRY(long)
    {
    m_iWhat=What;
    m_pProps=(CPropertyBase*)Mdl;
    m_iPropNo=PropNo;
    }		
	SCD_COMEXIT
  }

//---------------------------------------------------------------------------

STDMETHODIMP CScdProperty::get_Value(long Phases, double TdK, double PkPa, VARIANT * pValue)
  {
  dllSCD_COMENTRY(long)
    {
    MPropertyValue TheValue;
    m_pProps->GetPropertyValue(m_iPropNo, Phases, TdK, PkPa, TheValue); 
    switch (TheValue.Type())
      {
      case ePT_Bool:
        pValue->vt=VT_BOOL;
        pValue->boolVal=bool(TheValue)!=0; 
        break;
      case ePT_Long:
        pValue->vt=VT_I4;
        pValue->lVal=TheValue; 
        break;
      //case ePT_ULong:
      //  pValue->vt=VT_UI4;
      //  pValue->ulVal=TheValue; 
      //  break;
      case ePT_Double:
        pValue->vt=VT_R8;
        pValue->dblVal=TheValue; 
        break;
      case ePT_String:
        {
        pValue->vt=VT_BSTR;
        _bstr_t X(TheValue);
        pValue->bstrVal=X.copy(); 
        break;
        }
      }
    }		
  SCD_COMEXIT
  };

//---------------------------------------------------------------------------

STDMETHODIMP CScdProperty::SetValue(VARIANT * pValue)
  {
  dllSCD_COMENTRY(long)
    {
    MPropertyValue TheValue;
    _variant_t V(*pValue);
    switch (V.vt &~VT_BYREF)
      {
      case VT_I1:
      case VT_I2:
      case VT_I4:
      case VT_I8:
        TheValue=(long)V;
        break;
      case VT_UI1:
      case VT_UI2:
      case VT_UI4:
      case VT_UI8:
      case VT_BOOL:
        TheValue=(ULONG)V;
        break;
      case VT_R4:
      case VT_R8:
        TheValue=(double)V;
        break;
      case VT_BSTR:
        {
        _bstr_t T(V.bstrVal);
        TheValue=(LPCTSTR)T;
        break;
        }
      }

    m_pProps->PutPropertyValue(m_iPropNo, TheValue); 
    }		
  SCD_COMEXIT
  };

//---------------------------------------------------------------------------

STDMETHODIMP CScdProperty::get_Tag(BSTR * pTag)
  {
  dllSCD_COMENTRY(long)
    {
	  if (pTag== NULL)
		  return E_POINTER;
	  //switch (m_iWhat)
    CComBSTR X(m_pProps->PropertyMap()[m_iPropNo]->m_sTag); 
    *pTag=X.Copy(); 
    }		
	SCD_COMEXIT
  };

//---------------------------------------------------------------------------

STDMETHODIMP CScdProperty::get_Symbol(BSTR * pSymbol)
  {
  dllSCD_COMENTRY(long)
    {
	  if (pSymbol== NULL)
		  return E_POINTER;
	  //switch (m_iWhat)
    CComBSTR X(m_pProps->PropertyMap()[m_iPropNo]->m_sSym); 
    *pSymbol=X.Copy(); 
    }		
	SCD_COMEXIT
  };

//---------------------------------------------------------------------------

STDMETHODIMP CScdProperty::get_Description(BSTR * pDesc)
  {
  dllSCD_COMENTRY(long)
    {
	  if (pDesc== NULL)
		  return E_POINTER;
	  CComBSTR X(m_pProps->PropertyMap()[m_iPropNo]->m_sDesc);
    *pDesc=X.Copy();
    }		
	SCD_COMEXIT
  };

//---------------------------------------------------------------------------

STDMETHODIMP CScdProperty::get_Cnv(IScdConversion ** pItem)
	{
  dllSCD_COMENTRY(long)
    {
		IScdConversion * p;
		::CoCreateInstance(CLSID_ScdConversion, NULL, CLSCTX_ALL, IID_IScdConversion, (void**)&p);
		p->SetObjectInfo(m_pProps->PropertyMap()[m_iPropNo]->m_iCnv, 0);
		*pItem=p; // User must release
		return S_OK;
    }		
	SCD_COMEXIT
	}

STDMETHODIMP CScdProperty::get_Index(LONG* pVal)
  {
  dllSCD_COMENTRY(long)
    {
	  if (pVal== NULL)
		  return E_POINTER;
    *pVal=m_iPropNo;
    }		
	SCD_COMEXIT
  }

