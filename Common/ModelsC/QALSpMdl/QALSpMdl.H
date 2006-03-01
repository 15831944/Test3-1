//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __QALSPMDL_H
#define  __QALSPMDL_H

#ifndef  __SC_DEFS_H
  #include "sc_defs.h"
#endif

#ifdef __QALSPMDL_CPP
  #define DllImportExport DllExport
#elif !defined(QALSPMDL)
  #define DllImportExport DllImport
  #pragma LIBCOMMENT("..\\..\\usrmdls\\QALSPMDL\\", "\\QALSPMDL")
#else
  #define DllImportExport
#endif

DllImportExport void ForceLoad_QALSPMDL();

#undef DllImportExport

#endif
