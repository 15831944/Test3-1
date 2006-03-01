//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __KW_SMDK1_H
#define  __KW_SMDK1_H

#ifdef __KW_SMDK1_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(KW_SMDK1)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

//===========================================================================

//.....

//---------------------------------------------------------------------------
#undef DllImportExport

#endif
