//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __ALCANSPMDL_H
#define  __ALCANSPMDL_H

#ifndef  __SC_DEFS_H
  #include "sc_defs.h"
#endif

#ifdef __ALCANSPMDL_CPP
  #define DllImportExport DllExport
#elif !defined(AlcanSpMdl)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

DllImportExport void ForceLoad_AlcanSpMdl();

#undef DllImportExport

#endif
