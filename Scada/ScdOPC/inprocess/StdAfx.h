// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#include <afxwin.h>
#include <afxdisp.h>
#include <afxcoll.h>
#include <afxtempl.h> // "xafxtempl.h"
#include <afxadv.h>        // CSharedFile

#define _ATL_FREE_THREADED
#define _ATL_STATIC_REGISTRY
#define _ATL_DEBUG_QI
#define USING_AFX_EXTENSION_MODULE

// Make a US English LCID
#define  ENG_LCID  MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),SORT_DEFAULT)

#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>

#ifdef _ATL_STATIC_REGISTRY
#include <statreg.h>
#endif

#ifdef VC5
    typedef GUID CATID;
#endif

#include "resource.h"
#include "OPCDa.h"
#if !defined(_WIN32_WCE)
   #include "OPCComn.h"
#else
   #include "OPCComn_CE.h"
#endif
#include "OPCError.h"
extern UINT OPCSTMFORMATDATA;
extern UINT OPCSTMFORMATDATATIME;
extern UINT OPCSTMFORMATWRITECOMPLETE;

#if defined(_WIN32_WCE)
#if _WIN32_WCE<211
inline long CoFileTimeNow( FILETIME* pft )
{
   SYSTEMTIME now;
   GetSystemTime( &now );
   SystemTimeToFileTime( &now, pft );
}
#endif
#endif
//*******************************************************************
#define TRACE_MSG  { \
      TCHAR msg[MAX_PATH]; \
      FormatMessage( FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM, \
                     NULL, \
                     GetLastError(), \
                     MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), \
                     msg, MAX_PATH, NULL ); \
      TRACE(msg); \
      TRACE(_T("\n")); }
