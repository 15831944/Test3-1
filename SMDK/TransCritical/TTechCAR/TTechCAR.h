//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __TTECHCAR_H
#define  __TTECHCAR_H

#ifdef __TTECHCAR_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(SMDKDemoU)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

DllImportExport void ForceLoad_TTechCAR();

//===========================================================================

//.....

//---------------------------------------------------------------------------
#undef DllImportExport

#endif
