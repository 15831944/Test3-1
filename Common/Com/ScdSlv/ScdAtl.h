// ScdAtl.h: interface for the CScdAtl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCDATL_H__2D6D464E_B4A7_499B_99B2_52848803D576__INCLUDED_)
#define AFX_SCDATL_H__2D6D464E_B4A7_499B_99B2_52848803D576__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef __SCDATL_CPP
  #define DllImportExport DllExport
#elif !defined(SCDSLV)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//extern DllImportExport int ScdModuleUpdateRegistryFromResourceD(_ATL_MODULE* pM, LPCOLESTR lpszRes,
//    BOOL bRegister, struct _ATL_REGMAP_ENTRY* pMapEntries=NULL, IRegistrar* pReg=NULL);

#define DECLARE_REGISTRY_RESOURCEID_SCDEXE_AS_SERVER(nResID)\
	static HRESULT WINAPI UpdateRegistry(BOOL bRegister)\
	{\
	return ScdModuleUpdateRegistryFromResourceD(&_Module, (LPCOLESTR)MAKEINTRESOURCE(nResID), bRegister);\
	}


#undef DllImportExport

#endif // !defined(AFX_SCDATL_H__2D6D464E_B4A7_499B_99B2_52848803D576__INCLUDED_)
