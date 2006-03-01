//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __RTTS_H
#define  __RTTS_H

#ifndef  __SC_DEFS_H
  #include "sc_defs.h"
#endif

#ifdef __RTTS_CPP
  #define DllImportExport DllExport
#elif !defined(RTTS)
  #define DllImportExport DllImport
  //#pragma LIBCOMMENT("..\\..\\usrmdls\\rtts\\", "\\RTTS.lib")
#else
  #define DllImportExport
#endif

DllImportExport void ForceLoad_RTTS();

#undef DllImportExport

#endif
