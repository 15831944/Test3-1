//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __Control1_H
#define  __Control1_H

#ifndef  __SC_DEFS_H
  #include "sc_defs.h"
#endif

#ifdef __Control1_CPP
  #define DllImportExport DllExport
#elif !defined(Control1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

DllImportExport void ForceLoad_Control1();

#undef DllImportExport

#endif
