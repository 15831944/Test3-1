//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __QALPRCP1_H
#define  __QALPRCP1_H

#ifndef  __SC_DEFS_H
  #include "sc_defs.h"
#endif

#ifdef __QALPRCP1_CPP
  #define DllImportExport DllExport
#elif !defined(QALPRCP1)
  #define DllImportExport DllImport
//  #pragma LIBCOMMENT("..\\..\\usrmdls\\QALPRCP1\\", "\\QALPRCP1")
#else
  #define DllImportExport
#endif

DllImportExport void ForceLoad_QALPRCP1();

#undef DllImportExport

#endif
