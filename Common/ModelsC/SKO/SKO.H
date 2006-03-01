//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __SKO_H
#define  __SKO_H

#ifndef  __SC_DEFS_H
  #include "sc_defs.h"
#endif

#ifdef __SKO_CPP
  #define DllImportExport DllExport
#elif !defined(SKO)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

DllImportExport void ForceLoad_SKO();

#undef DllImportExport

#endif
