//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __HeatXch1_H
#define  __HeatXch1_H

#ifndef  __SC_DEFS_H
  #include "sc_defs.h"
#endif

#ifdef __HeatXch1_CPP
  #define DllImportExport DllExport
#elif !defined(HeatXch1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

DllImportExport void ForceLoad_HeatXch1();

#undef DllImportExport

#endif
