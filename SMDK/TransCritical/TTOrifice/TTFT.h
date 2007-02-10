//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __TTECHFT_H
#define  __TTECHFT_H

#ifdef __TTECHFT_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(SMDKDemoU)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

DllImportExport void ForceLoad_TTechFT();

//===========================================================================

//.....

//---------------------------------------------------------------------------
#undef DllImportExport

#endif
