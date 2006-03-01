//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __SizeDst1_H
#define  __SizeDst1_H

#ifndef  __SC_DEFS_H
  #include "sc_defs.h"
#endif

#ifdef __SizeDst1_CPP
  #define DllImportExport DllExport
#elif !defined(SizeDst1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

DllImportExport void ForceLoad_SizeDst1();

#undef DllImportExport

#endif
