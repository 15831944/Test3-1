//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __ALCOASPMDL_H
#define  __ALCOASPMDL_H

#ifndef  __SC_DEFS_H
  #include "sc_defs.h"
#endif

#ifdef __ALCOASPMDL_CPP
  #define DllImportExport DllExport
#elif !defined(AlcoaSpMdl)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

DllImportExport void ForceLoad_AlcoaSpMdl();

#undef DllImportExport

#endif
