//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  Stdafx.h
//  $Author    :  Jim Hansen
//
//  Subsystem  :
//
//  Description:  Precompiled header
//
//
//**************************************************************************

#define VC_EXTRALEAN    // Exclude rarely-used stuff from Windows headers
#define NOIME
#define NOKANJI
#define NOTAPE
#define NOSOUND
#define NOSERVICE
#define NOIMAGE

//#ifdef _DEBUG
//#define _ATL_DEBUG_INTERFACES
//#define _ATL_DEBUG_REFCOUNT
//#define _ATL_DEBUG_QI
//#endif


#include <afxwin.h>        // MFC core and standard components
#include <afxext.h>        // MFC extensions
#include <afxcview.h>
#include <afxcmn.h>        // MFC support for Windows 95 Common Controls
#include <afxcoll.h>       // MFC collections
#include <afxtempl.h>      // MFC template collections
#include <afxole.h>
#include <afxadv.h>        // CSharedFile
#include <stdlib.h>        // only for rand()
#include <afxdisp.h>

#define HICKITITEM DWORD
#define HICKITTOPIC DWORD

#define  ENG_LCID  MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),SORT_DEFAULT)

#include "resource.h"
#include "Modbus.h"
#include "Doc.h"

// OPC
#define OPC_ATL_INCLUDED

#define _ATL_STATIC_REGISTRY
#include <atlbase.h>

//You may derive a class from CComModule and use it if you want to override
#define     WM_OPCLOCKUNLOCK     WM_USER + 50
#define     OPC_LOCK          0
#define     OPC_UNLOCK           1

//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
// ATL can increment/decrement MFC's lock count
class CAtlGlobalModule : public CComModule
{
public:
};

extern CAtlGlobalModule _Module;

#include <atlcom.h>
#include <statreg.h>

#include "OPCServer.h"

extern CRITICAL_SECTION uiCS;  // protect access to current device ptr, etc.
extern CRITICAL_SECTION tagCS; // protect access to tag values with this

#define TRACE_MSG  { \
      TCHAR msg[MAX_PATH]; \
      FormatMessage( FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM, \
                     NULL, \
                     GetLastError(), \
                     MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), \
                     msg, MAX_PATH, NULL ); \
      TRACE(msg); \
      TRACE(_T("\n")); }

#define WM_POLLCLOSE WM_USER+1 
#define WM_ADDDEVICE WM_USER+2 
#define WM_UPDATETAGS WM_USER+3 
#define WM_POLLUPDATE WM_USER+4 


//*******************************************************************
