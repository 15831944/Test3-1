//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __ALUMINA1_H
#define  __ALUMINA1_H

#ifndef  __SC_DEFS_H
  #include "sc_defs.h"
#endif

#ifdef __ALUMINA1_CPP
  #define DllImportExport DllExport
#elif !defined(Alumina1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

DllImportExport void ForceLoad_Alumina1();

#undef DllImportExport

#endif
