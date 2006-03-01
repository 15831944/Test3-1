// ScdProperties.cpp : Implementation of CScdProperties

#include "stdafx.h"
#include "ScdProperties.h"
#include ".\scdproperties.h"


// CScdProperties

STDMETHODIMP CScdProperties::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IScdProperties
	};

	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

//---------------------------------------------------------------------------

STDMETHODIMP CScdProperties::Property(VARIANT WhichOne, IScdProperty ** pItem)
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
        long N=m_pProps->PropertyCount();
        if (i>=0 && i<N)
          {
					//IScdSpModel *px=this;
          IScdProperty * p;
          ::CoCreateInstance(CLSID_ScdProperty, NULL, CLSCTX_ALL, IID_IScdProperty, (void**)&p);
          p->SetObjectInfo(0, (ULONG)m_pProps, i);
          *pItem=p; // User must release
          return S_OK;
          }
        return Scd.ReturnH(E_INVALIDARG, "Bad Index");
        }
      case VT_BSTR:
        {
        SCD_LclStr(Name, WhichOne.bstrVal);
        long i=m_pProps->PropertyMap().Lookup(Name);
        if (i>=0)
          {
          IScdProperty * p;
          ::CoCreateInstance(CLSID_ScdProperty, NULL, CLSCTX_ALL, IID_IScdProperty, (void**)&p);
          p->SetObjectInfo(0, (ULONG)m_pProps, i);
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

//---------------------------------------------------------------------------

STDMETHODIMP CScdProperties::get_Count(LONG * pVal)
  {
	dllSCD_COMENTRYGET(long, pVal)
    {
    *pVal=m_pProps->PropertyCount();
    }		
	SCD_COMEXIT
  }

//---------------------------------------------------------------------------

STDMETHODIMP CScdProperties::get__NewEnum(IUnknown * * pVal)
  {
	dllSCD_COMENTRYGET(long, pVal)
    {
	  // This block of code allows VB-like iteration.
	  // Make a temp array of VARIANTS and fill 

    long size=m_pProps->PropertyCount();;

    VARIANT* pVar = new VARIANT[size];
	  for(long i = 0; i < size; i++)
      {
      IScdProperty * p;
      ::CoCreateInstance(CLSID_ScdProperty, NULL, CLSCTX_ALL, IID_IScdProperty, (void**)&p);
      p->SetObjectInfo(0, (ULONG)m_pProps, i);
      LPDISPATCH pDisp;
      SCD_TOF(p->QueryInterface(IID_IDispatch, (void**)&pDisp), "IDispatch on ScdProperty")
      pVar[i].vt = VT_DISPATCH;
      pVar[i].pdispVal=pDisp;
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

STDMETHODIMP CScdProperties::SetObjectInfo(ULONG What, ULONG Which)
  {
  dllSCD_COMENTRY(long)
    {
    m_iWhat=What;
    m_pProps=(CPropertyBase*)Which;
    }		
	SCD_COMEXIT
  }
