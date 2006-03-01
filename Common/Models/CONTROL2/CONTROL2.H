//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __Control2_H
#define  __Control2_H

#ifndef  __SC_DEFS_H
  #include "sc_defs.h"
#endif

#ifdef __Control2_CPP
  #define DllImportExport DllExport
#elif !defined(Control2)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

DllImportExport void ForceLoad_Control2();

#undef DllImportExport

#endif
