//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __SCDREGISTRY_H
#define  __SCDREGISTRY_H

#ifdef __SCDREGISTRY_CPP
  #define DllImportExport DllExport
#elif !defined(SCDLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//#ifndef  __SC_DEFS_H
//  #include "sc_defs.h"
//#endif

#if MOVEDTO_MD_SHARE_3
class DllImportExport CRegistry
  {
  public:
    CRegistry(void);
    ~CRegistry(void);

    void SetRegistryKey(HKEY hKey, LPCTSTR lpszRegistryKey, LPCTSTR lpszProfile);
    HKEY GetAppRegistryKey();
    HKEY GetSectionKey(LPCTSTR lpszSection);
    UINT GetProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry,	int nDefault);
    CString GetProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry,	LPCTSTR lpszDefault);
    BOOL GetProfileBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry, BYTE** ppData, UINT* pBytes);
    BOOL WriteProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue);
    BOOL WriteProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue);
    BOOL WriteProfileBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPBYTE pData, UINT nBytes);


  protected:
    HKEY    m_hKey;
    LPCTSTR m_pszProfileName;
	  LPCTSTR m_pszRegistryKey;
	  LPCTSTR m_pszAppName;

  };
#endif

#undef DllImportExport

#endif