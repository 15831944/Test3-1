//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __QALMDLS_H
#define  __QALMDLS_H

#ifndef  __SC_DEFS_H
  #include "sc_defs.h"
#endif

#ifdef __QALMDLS_CPP
  #define DllImportExport DllExport
#elif !defined(QALMDLS)
  #define DllImportExport DllImport
  #pragma LIBCOMMENT("..\\..\\usrmdls\\QALMDLS\\", "\\QALMDLS")
#else
  #define DllImportExport
#endif

DllImportExport void ForceLoad_QALMDLS();

#undef DllImportExport

#endif
