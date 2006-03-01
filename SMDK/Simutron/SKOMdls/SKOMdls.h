//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __SKOMDLS_H
#define  __SKOMDLS_H

#ifdef __SKOMDLS_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(SKOMDLS)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

//===========================================================================

//...

//---------------------------------------------------------------------------
#undef DllImportExport

#endif
