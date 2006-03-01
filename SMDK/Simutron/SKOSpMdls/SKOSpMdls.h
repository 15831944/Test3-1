//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __SKOSPMDLS_H
#define  __SKOSPMDLS_H

#ifdef __SKOSPMDLS_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(SKOSPMDLS)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

//===========================================================================

//...

//---------------------------------------------------------------------------
#undef DllImportExport

#endif
