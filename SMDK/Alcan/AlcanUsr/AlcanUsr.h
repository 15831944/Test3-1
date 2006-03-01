//===========================================================================
//=== SysCAD SMDK - Hyprod Precipitator Model 2003 : Alcan, Kenwalt       ===
//===========================================================================

#ifndef  __ALCANUSR_H
#define  __ALCANUSR_H

#ifdef __ALCANUSR_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(AlcanU)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

//===========================================================================

//.....

//---------------------------------------------------------------------------
#undef DllImportExport

#endif
