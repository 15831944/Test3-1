//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __PETRO1_H
#define  __PETRO1_H

#ifndef  __SC_DEFS_H
  #include "sc_defs.h"
#endif

#ifdef __PETRO1_CPP
  #define DllImportExport DllExport
#elif !defined(Petro1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

DllImportExport void ForceLoad_Petro1();

#undef DllImportExport

#endif
