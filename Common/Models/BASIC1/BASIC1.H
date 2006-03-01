//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __Basic1_H
#define  __Basic1_H

#ifndef  __SC_DEFS_H
  #include "sc_defs.h"
#endif

#ifdef __Basic1_CPP
  #define DllImportExport DllExport
#elif !defined(Basic1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

DllImportExport void ForceLoad_Basic1();

#undef DllImportExport

#endif
