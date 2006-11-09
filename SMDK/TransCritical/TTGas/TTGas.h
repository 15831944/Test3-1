//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __TTGAS_H
#define  __TTGAS_H

#ifdef __TTGAS_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(SMDKDemoU)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

//===========================================================================

//...

//---------------------------------------------------------------------------
#undef DllImportExport

#endif
