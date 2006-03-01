//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __Separ2_H
#define  __Separ2_H

#ifndef  __SC_DEFS_H
  #include "sc_defs.h"
#endif

#ifdef __Separ2_CPP
  #define DllImportExport DllExport
#elif !defined(Separ2)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

DllImportExport void ForceLoad_Separ2();

#undef DllImportExport

#endif
