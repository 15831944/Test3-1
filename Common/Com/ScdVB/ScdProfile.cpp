// ScdProfile.cpp : Implementation of CScdProfile

#include "stdafx.h"
#include "ScdProfile.h"
#include ".\scdprofile.h"
#include "comdef.h"


// CScdProfile

STDMETHODIMP CScdProfile::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IScdProfile
	};

	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP CScdProfile::ReadString(BSTR File, BSTR Section, BSTR Entry, BSTR Default, BSTR* pVal)
  {
  try
    {
    char Buff[1024];
    DWORD Len = ::GetPrivateProfileString(_bstr_t(Section), _bstr_t(Entry), "", Buff, sizeof(Buff), _bstr_t(File));
    if (Len==0)
      {
      _bstr_t Df(Default);
      *pVal=Df.copy();
      }
    else
      {
      _bstr_t s(Buff);
      *pVal=s.copy();
      }
    }
  catch(...)
    {
    return E_FAIL;
    }
  return S_OK;
  }

STDMETHODIMP CScdProfile::WriteString(BSTR File, BSTR Section, BSTR Entry, BSTR newVal)
  {
  try
    {
    if (WritePrivateProfileString(_bstr_t(Section), _bstr_t(Entry), _bstr_t(newVal), _bstr_t(File)))
      return S_OK;
    }
  catch(...)
    {
    return E_FAIL;
    }
  return S_OK;
  }

STDMETHODIMP CScdProfile::ReadLong(BSTR File, BSTR Section, BSTR Entry, long Default, long * pVal)
  {
  try
    {
    char Buff[256];
    DWORD Len = ::GetPrivateProfileString(_bstr_t(Section), _bstr_t(Entry), "", Buff, sizeof(Buff), _bstr_t(File));
    if (Len==0)
      *pVal=Default;
    else
      *pVal=atol(Buff);
    }
  catch(...)
    {
    return E_FAIL;
    }
  return S_OK;
  }

STDMETHODIMP CScdProfile::WriteLong(BSTR File, BSTR Section, BSTR Entry, long newVal)
  {
  try
    {
    char Buff[256];
    sprintf(Buff, "%i", newVal);
    if (WritePrivateProfileString(_bstr_t(Section), _bstr_t(Entry), Buff, _bstr_t(File)))
      return S_OK;
    }
  catch(...)
    {
    return E_FAIL;
    }
  return S_OK;
  }

STDMETHODIMP CScdProfile::ReadDouble(BSTR File, BSTR Section, BSTR Entry, double Default, double * pVal)
  {
  try
    {
    char Buff[256];
    DWORD Len = ::GetPrivateProfileString(_bstr_t(Section), _bstr_t(Entry), "", Buff, sizeof(Buff), _bstr_t(File));
    if (Len==0)
      *pVal=Default;
    else
      *pVal=atof(Buff);
    }
  catch(...)
    {
    return E_FAIL;
    }
  return S_OK;
  }

STDMETHODIMP CScdProfile::WriteDouble(BSTR File, BSTR Section, BSTR Entry, double newVal)
  {
  try
    {
    char Buff[256];
    sprintf(Buff, "%.20g", newVal);
    if (WritePrivateProfileString(_bstr_t(Section), _bstr_t(Entry), Buff, _bstr_t(File)))
      return S_OK;
    }
  catch(...)
    {
    return E_FAIL;
    }
  return S_OK;
  }
