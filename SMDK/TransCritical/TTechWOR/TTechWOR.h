//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __TTECHWOR_H
#define  __TTECHWOR_H

#ifdef __TTECHWOR_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(SMDKDemoU)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

DllImportExport void ForceLoad_BATC_Brahma();

//===========================================================================

//.....

//---------------------------------------------------------------------------
#undef DllImportExport

#endif
