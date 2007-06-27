//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __ALUMINA2_H
#define  __ALUMINA2_H

#ifdef __ALUMINA2_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(ALUMINA2)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

//===========================================================================

//...

//---------------------------------------------------------------------------
#undef DllImportExport

#endif
