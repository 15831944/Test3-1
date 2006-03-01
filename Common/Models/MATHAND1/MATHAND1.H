//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __MatHand1_H
#define  __MatHand1_H

#ifndef  __SC_DEFS_H
  #include "sc_defs.h"
#endif

#ifdef __MatHand1_CPP
  #define DllImportExport DllExport
#elif !defined(MatHand1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

DllImportExport void ForceLoad_MatHand1();

#undef DllImportExport

#endif
