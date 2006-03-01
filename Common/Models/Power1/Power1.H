//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __Power1_H
#define  __Power1_H

#ifndef  __SC_DEFS_H
  #include "sc_defs.h"
#endif

#ifdef __Power1_CPP
  #define DllImportExport DllExport
#elif !defined(Power1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

DllImportExport void ForceLoad_Power1();

#undef DllImportExport

#endif
