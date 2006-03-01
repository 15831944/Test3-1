//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __MdlBase_H
#define  __MdlBase_H

#ifndef  __SC_DEFS_H
  #include "sc_defs.h"
#endif

#ifdef __MdlBase_CPP
  #define DllImportExport DllExport
#elif !defined(MdlBase)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

DllImportExport void ForceLoad_MdlBase();

#undef DllImportExport

#endif
