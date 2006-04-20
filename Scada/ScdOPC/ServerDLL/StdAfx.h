// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

//**************************************************************************

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#define _WIN32_DCOM

#include <afxwin.h>
#include <afxdisp.h>
#include <afxcoll.h>
#include <afxtempl.h>
#include <afxadv.h>        // CSharedFile
#include <math.h>

#include "..\..\..\SMDK\Include\ScdIF.h"

#include "OPCServer.h"
#include "ScdOPCSrv.h"

extern BOOL running;
extern ServerList servers;

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
