//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __CLEVLAND_H
#define  __CLEVLAND_H

#ifndef  __SC_DEFS_H
  #include "sc_defs.h"
#endif

#ifdef __CLEVLAND_CPP
  #define DllImportExport DllExport
#elif !defined(CLEVLAND)
  #define DllImportExport DllImport
  #pragma comment(lib, "CLEVLAND.lib")
#else
  #define DllImportExport
#endif

DllImportExport void ForceLoad_CLEVLAND();

#undef DllImportExport

#endif
