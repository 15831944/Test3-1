// ScdRegistry.cpp : Implementation of CScdRegistry
#include "stdafx.h"
#include "ScdVB.h"
#include "ScdRegistry.h"
#include "comdef.h"

/////////////////////////////////////////////////////////////////////////////
// CScdRegistry

STDMETHODIMP CScdRegistry::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IScdRegistry
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP CScdRegistry::GetKey(ScdKeyTypes Type, ScdHKEY HKEYRoot, BSTR KeyPath, BSTR KeyName, VARIANT *pValue)
{
  try
    {
    HKEY hKey;
    _bstr_t KP(KeyPath, false);
    long lResult = RegOpenKeyEx((HKEY)HKEYRoot, KP, 0, KEY_ALL_ACCESS, &hKey);
    if (lResult != ERROR_SUCCESS)
      // Leave Value as it was
      return S_OK;

    _bstr_t KN(KeyName, false);
    unsigned char cbData[2048];
    DWORD dwType, dwDataLen=sizeof(cbData);
    lResult = RegQueryValueEx(hKey, KN, 0, &dwType, cbData, &dwDataLen);
    if (lResult != ERROR_SUCCESS)
      // Leave Value as it was
      return S_OK;
    
    lResult = RegCloseKey(hKey);

    _variant_t v;
    switch (dwType)
      {
      case REG_NONE:
        break;
      case REG_SZ: 
        v=(LPCTSTR)cbData;
        break;
      case REG_DWORD:
        v=*((long*)cbData);
        break;
      default:;
      }

    VariantCopy(pValue, &v);
    return S_OK;
    }
  catch(...)
    {
    return E_FAIL;
    }
}

STDMETHODIMP CScdRegistry::PutKey(ScdKeyTypes Type, ScdHKEY HKEYRoot, BSTR KeyPath, BSTR KeyName, VARIANT Value)
{
  try
    {
    HKEY hKey;
    _bstr_t KP(KeyPath, false);
    long lResult = RegOpenKeyEx((HKEY)HKEYRoot, KP, 0, KEY_ALL_ACCESS, &hKey);
    if (lResult != ERROR_SUCCESS)
      {
      // does not exist - create it
      DWORD dwDisposition;
      long lResult = RegCreateKeyEx((HKEY)HKEYRoot, KP, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);
      if (lResult != ERROR_SUCCESS)
        return E_FAIL;
      }

    _bstr_t KN(KeyName, false);
    DWORD dwType;
    lResult = RegQueryValueEx(hKey, KN, 0, &dwType, NULL, NULL);
    if (lResult != ERROR_SUCCESS)
      {
      dwType=Type;
      }

    _variant_t v(Value);
    switch (dwType)
      {
      case REG_NONE:
        break;
      case REG_SZ:
        {
        v.ChangeType(VT_BSTR);
        _bstr_t b(v.bstrVal);
        lResult = RegSetValueEx(hKey, KN, 0, dwType, (CONST BYTE *)(char*)b, b.length()+1);
        }
        break;
      case REG_DWORD:
        v.ChangeType(VT_UI4);
        lResult = RegSetValueEx(hKey, KN, 0, dwType, (CONST BYTE *)&v.ulVal, sizeof(v.ulVal));
        break;
      default:;
      }
    if (lResult != ERROR_SUCCESS)
      return E_FAIL;

    lResult = RegCloseKey(hKey);
    if (lResult != ERROR_SUCCESS)
      return E_FAIL;
    
    return S_OK;
    }
  catch(...)
    {
    return E_FAIL;
    }
}



