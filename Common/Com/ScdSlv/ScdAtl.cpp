// ScdAtl.cpp: implementation of the CScdAtl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#define __SCDATL_CPP
#include "resource.h"
#include "ScdAtl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/**
int ScdModuleUpdateRegistryFromResourceD(_ATL_MODULE* pM, LPCOLESTR lpszRes,
  BOOL bRegister, struct _ATL_REGMAP_ENTRY* pMapEntries, IRegistrar* pReg)
  {
  USES_CONVERSION;
  ATLASSERT(pM != NULL);
  HRESULT hRes = S_OK;
  CComPtr<IRegistrar> p;
  if (pReg != NULL)
    p = pReg;
  else
    {
    hRes = CoCreateInstance(CLSID_Registrar, NULL,
      CLSCTX_INPROC_SERVER, IID_IRegistrar, (void**)&p);
    }
  if (SUCCEEDED(hRes))
    {
    TCHAR szModule[_MAX_PATH];
    TCHAR szModuleRep[_MAX_PATH];
    GetModuleFileName(pM->m_hInst, szModule, _MAX_PATH);

    // replace ScdApp.dll with SysCAD.exe
    // This makes the assumption that SysCAD.exe and ScdApp.dll are in the same folder
    strcpy(szModuleRep, szModule);
    _strlwr(szModuleRep);
    TCHAR * pName=strstr(szModuleRep, _T("scdapp.dll"));
    if (pName!=NULL)
      strcpy(pName, _T("syscad.exe"));

    LPOLESTR pszModuleRep = T2OLE(szModuleRep);
    LPOLESTR pszModule;
    if ((pM->m_hInst == NULL) || (pM->m_hInst == GetModuleHandle(NULL))) // register as EXE
      {
      // Convert to short path to work around bug in NT4's CreateProcess
      TCHAR szModuleShort[_MAX_PATH];
      int cbShortName = GetShortPathName(szModule, szModuleShort, _MAX_PATH);

      if (cbShortName == _MAX_PATH)
        return E_OUTOFMEMORY;

      pszModule = (cbShortName == 0 || cbShortName == ERROR_INVALID_PARAMETER) ? T2OLE(szModule) : T2OLE(szModuleShort);
      }
    else
      pszModule = T2OLE(szModule);

    int nLen = ocslen(pszModuleRep);
    LPOLESTR pszModuleRepQuote = (LPOLESTR)alloca((nLen*2+1)*sizeof(OLECHAR));
    CComModule::ReplaceSingleQuote(pszModuleRepQuote, pszModuleRep);
    p->AddReplacement(OLESTR("Module"), pszModuleRepQuote);

    if (NULL != pMapEntries)
      {
      while (NULL != pMapEntries->szKey)
        {
        ATLASSERT(NULL != pMapEntries->szData);
        p->AddReplacement((LPOLESTR)pMapEntries->szKey, (LPOLESTR)pMapEntries->szData);
        pMapEntries++;
        }
      }
    LPCOLESTR szType = OLESTR("REGISTRY");
    if (HIWORD(lpszRes)==0)
      {
      if (bRegister)
        hRes = p->ResourceRegister(pszModule, ((UINT)LOWORD((DWORD)lpszRes)), szType);
      else
        hRes = p->ResourceUnregister(pszModule, ((UINT)LOWORD((DWORD)lpszRes)), szType);
      }
    else
      {
      if (bRegister)
        hRes = p->ResourceRegisterSz(pszModule, lpszRes, szType);
      else
        hRes = p->ResourceUnregisterSz(pszModule, lpszRes, szType);
      }

    }
  return hRes;
  }
**/
//////////////////////////////////////////////////////////////////////
