//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __Commn1_H
#define  __Commn1_H

#ifndef  __SC_DEFS_H
  #include "sc_defs.h"
#endif

#ifdef __Commn1_CPP
  #define DllImportExport DllExport
#elif !defined(Commn1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

DllImportExport void ForceLoad_Commn1();

#undef DllImportExport

#endif
