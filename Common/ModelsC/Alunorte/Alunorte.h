//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __ALUNORTE_H
#define  __ALUNORTE_H

#ifndef  __SC_DEFS_H
  #include "sc_defs.h"
#endif

#ifdef __ALUNORTE_CPP
  #define DllImportExport DllExport
#elif !defined(Alunorte)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

DllImportExport void ForceLoad_Alunorte();

#undef DllImportExport

#endif
